#requires -Version 5.1
<#
Extract a decrypted Pokemon X cartridge dump into a small analysis upload.
Downloads only the pinned open-source 3dstool utility from its maintainer.
The game image is opened read-only. Outputs go into a new directory each run.
#>
[CmdletBinding()]
param(
    [string]$RomPath = 'D:\Downloads\Pokemon X (USA) (En,Ja,Fr,De,Es,It,Ko).3ds',
    [string]$OutputRoot = '',
    [string]$ToolPath = '',
    [switch]$AllowDifferentTitle
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Read-Range([System.IO.Stream]$Stream, [long]$Offset, [int]$Count) {
    if ($Offset -lt 0 -or $Offset + $Count -gt $Stream.Length) { throw 'Header extends beyond the input file.' }
    $null = $Stream.Seek($Offset, [System.IO.SeekOrigin]::Begin)
    $buffer = New-Object byte[] $Count
    $done = 0
    while ($done -lt $Count) {
        $read = $Stream.Read($buffer, $done, $Count - $done)
        if ($read -eq 0) { throw 'Unexpected end of input.' }
        $done += $read
    }
    return ,$buffer
}

function Invoke-Extractor([string[]]$ToolArgs) {
    & $script:Extractor @ToolArgs
    if ($LASTEXITCODE -ne 0) { throw "3dstool failed with exit code $LASTEXITCODE. See the console output above." }
}

$rom = Get-Item -LiteralPath $RomPath
if ($rom.PSIsContainer) { throw 'RomPath must name a file.' }
$stream = [System.IO.File]::Open($rom.FullName, 'Open', 'Read', 'Read')
try {
    $ncsd = Read-Range $stream 0 0x200
    if ([Text.Encoding]::ASCII.GetString($ncsd, 0x100, 4) -ne 'NCSD') {
        throw 'This is not an NCSD .3ds cartridge image. Send the filename and this message.'
    }
    if ($ncsd[0x18E] -gt 8) { throw 'Unsupported NCSD media unit size.' }
    [long]$unit = [long]512 -shl [int]$ncsd[0x18E]
    [long]$partitionOffset = [long][BitConverter]::ToUInt32($ncsd, 0x120) * $unit
    [long]$partitionSize = [long][BitConverter]::ToUInt32($ncsd, 0x124) * $unit
    if ($partitionOffset -lt 0x200 -or $partitionSize -lt 0xA00 -or $partitionOffset + $partitionSize -gt $stream.Length) {
        throw 'The main partition is absent, truncated, or has an unsupported layout.'
    }
    $ncch = Read-Range $stream $partitionOffset 0x200
    if ([Text.Encoding]::ASCII.GetString($ncch, 0x100, 4) -ne 'NCCH') { throw 'Partition 0 is not an NCCH executable.' }
    if (($ncch[0x18F] -band 4) -eq 0) {
        throw 'The main partition is marked encrypted. This helper requires a decrypted dump; it has not attempted extraction.'
    }
    $programId = [BitConverter]::ToUInt64($ncch, 0x118).ToString('X16')
    if ($programId -ne '0004000000055D00' -and -not $AllowDifferentTitle) {
        throw "Title ID $programId does not match the expected Pokemon X base game (0004000000055D00). Send this message before proceeding."
    }
    $productCode = [Text.Encoding]::ASCII.GetString($ncch, 0x150, 16).Trim([char]0)
} finally { $stream.Dispose() }

if ([string]::IsNullOrWhiteSpace($OutputRoot)) { $OutputRoot = $rom.DirectoryName }
$parent = [System.IO.Path]::GetFullPath($OutputRoot)
$null = New-Item -ItemType Directory -Path $parent -Force
$job = Join-Path $parent ('PokemonX-analysis-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '-' + [Guid]::NewGuid().ToString('N').Substring(0, 8))
$null = New-Item -ItemType Directory -Path $job
$work = Join-Path $job 'work'
$upload = Join-Path $job 'upload'
$null = New-Item -ItemType Directory -Path $work
$null = New-Item -ItemType Directory -Path $upload

$toolUrl = 'https://github.com/dnasdw/3dstool/releases/download/v1.2.6/3dstool.zip'
# Hash measured from the maintainer's HTTPS release asset; no publisher digest was provided.
$toolArchiveHash = '481e20f445eb2f0f506d0d88cd750385bc8377670d681d6f66f584a176027806'
if ([string]::IsNullOrWhiteSpace($ToolPath)) {
    [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12
    $archive = Join-Path $job '3dstool.zip'
    Write-Host 'Downloading 3dstool 1.2.6 from its maintainer...'
    Invoke-WebRequest -Uri $toolUrl -OutFile $archive -UseBasicParsing -TimeoutSec 120
    if ((Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant() -ne $toolArchiveHash) {
        throw 'The downloaded tool does not match the pinned archive hash. It will not be executed.'
    }
    $toolFolder = Join-Path $job 'tool'
    Expand-Archive -LiteralPath $archive -DestinationPath $toolFolder
    $script:Extractor = Join-Path $toolFolder '3dstool.exe'
} else {
    $script:Extractor = (Get-Item -LiteralPath $ToolPath).FullName
}

Write-Host "Reading input hash (2 GiB may take a little time): $($rom.FullName)"
$inputHash = (Get-FileHash -LiteralPath $rom.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
$mainCxi = Join-Path $work 'main.cxi'
$exefsBin = Join-Path $work 'exefs.bin'
$exefsDir = Join-Path $work 'exefs'
$exheader = Join-Path $upload 'exheader.bin'
$code = Join-Path $upload 'code.bin'
Push-Location $work
try {
    Invoke-Extractor -ToolArgs @('-xvtf', 'cci', $rom.FullName, '-0', $mainCxi)
    Invoke-Extractor -ToolArgs @('-xvtf', 'cxi', $mainCxi, '--exh', $exheader, '--exefs', $exefsBin)
    Invoke-Extractor -ToolArgs @('-xvtf', 'exefs', $exefsBin, '--exefs-dir', $exefsDir)
    $header = [IO.File]::ReadAllBytes($exheader)
    if ($header.Length -notin @(0x400, 0x800)) { throw 'Extracted ExHeader has an unexpected length.' }
    $headerId = [BitConverter]::ToUInt64($header, 0x200).ToString('X16')
    if ($headerId -ne $programId) { throw 'NCCH and ExHeader program IDs differ; extraction needs review.' }
    $candidates = @(Join-Path $exefsDir 'code.bin'; Join-Path $exefsDir '.code')
    $rawCode = $null
    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) { $rawCode = $candidate; break }
    }
    if ($null -eq $rawCode) { throw 'No .code/code.bin entry was found in ExeFS.' }
    $compressed = ($header[0x0D] -band 1) -ne 0
    if ($compressed) {
        Invoke-Extractor -ToolArgs @('-uvf', $rawCode, '--compress-type', 'blz', '--compress-out', $code)
    } else { Copy-Item -LiteralPath $rawCode -Destination $code }

    [long]$packedSize = 0
    [long]$paddedSize = 0
    foreach ($offset in @(0x10, 0x20, 0x30)) {
        [long]$pages = [BitConverter]::ToUInt32($header, $offset + 4)
        [long]$size = [BitConverter]::ToUInt32($header, $offset + 8)
        if ($size -gt $pages * 4096) { throw 'ExHeader section size exceeds its page allocation.' }
        $packedSize += $size
        $paddedSize += $pages * 4096
    }
    $codeFile = Get-Item -LiteralPath $code
    if ($codeFile.Length -le 0 -or $codeFile.Length -notin @($packedSize, $paddedSize)) {
        throw "Code length $($codeFile.Length) does not match ExHeader ($packedSize packed / $paddedSize padded). No upload ZIP was created."
    }
    $manifest = [ordered]@{
        schema_version = 1
        target = 'Pokemon X'
        created_utc = [DateTime]::UtcNow.ToString('o')
        input_filename = $rom.Name
        input_size = $rom.Length
        input_sha256 = $inputHash
        program_id = $programId
        product_code = $productCode
        remaster_version = [BitConverter]::ToUInt16($header, 0x0E)
        original_compression_flag = $compressed
        code_is_decompressed = $true
        region_note = 'Filename region is unverified; program ID does not independently establish region.'
        version_note = 'Main cartridge executable only; separately installed game updates and CRO modules are not included.'
        tool_version = '3dstool 1.2.6'
        tool_url = $toolUrl
        tool_sha256 = (Get-FileHash -LiteralPath $script:Extractor -Algorithm SHA256).Hash.ToLowerInvariant()
        files = @(
            @{ name = 'code.bin'; size = $codeFile.Length; sha256 = (Get-FileHash -LiteralPath $code -Algorithm SHA256).Hash.ToLowerInvariant() },
            @{ name = 'exheader.bin'; size = $header.Length; sha256 = (Get-FileHash -LiteralPath $exheader -Algorithm SHA256).Hash.ToLowerInvariant() }
        )
    }
    $jsonPath = Join-Path $upload 'manifest.json'
    $manifest | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $jsonPath -Encoding UTF8
    $zip = Join-Path $job 'PokemonX-analysis-upload.zip'
    Compress-Archive -LiteralPath @($code, $exheader, $jsonPath) -DestinationPath $zip -CompressionLevel Optimal
} finally { Pop-Location }

Write-Host ''
Write-Host 'Extraction complete. Upload this ZIP in the chat:'
Write-Host $zip
Write-Host ('ZIP size: {0:N2} MiB' -f ((Get-Item -LiteralPath $zip).Length / 1MB))
Write-Host 'The original game image was opened read-only. The work folder contains a full copy of partition 0.'
