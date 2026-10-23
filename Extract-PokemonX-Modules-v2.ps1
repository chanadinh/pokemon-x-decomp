#requires -Version 5.1
<#
Reuse the previous Pokemon X extraction to package CRO/CRS/CRR code modules.
Reads work/main.cxi and uses the existing 3dstool utility. No download or upload.
RomFS is extracted into a fresh folder; the ZIP contains only code modules and
a provenance manifest. Allow space for both a RomFS image and its extracted tree.
#>
[CmdletBinding()]
param(
    [string]$ExtractionFolder = 'D:\Downloads\PokemonX-analysis-20261023-102030-ff2c2690',
    [string]$OutputRoot = '',
    [string]$ToolPath = '',
    [switch]$ResumeLatest
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

# Windows PowerShell 5.1 does not reliably load Compression through FileSystem.
# Resolve both assemblies and all ZIP types before hashing or extracting RomFS.
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$null = [IO.Compression.ZipArchive]
$null = [IO.Compression.ZipArchiveMode]::Create
$null = [IO.Compression.CompressionLevel]::Optimal
$null = [IO.Compression.ZipFileExtensions]

function Read-Range([IO.Stream]$Stream, [long]$Offset, [int]$Count) {
    if ($Offset -lt 0 -or $Offset + $Count -gt $Stream.Length) { throw 'A header extends beyond main.cxi.' }
    $null = $Stream.Seek($Offset, [IO.SeekOrigin]::Begin)
    $buffer = New-Object byte[] $Count
    $done = 0
    while ($done -lt $Count) {
        $read = $Stream.Read($buffer, $done, $Count - $done)
        if ($read -eq 0) { throw 'Unexpected end of main.cxi.' }
        $done += $read
    }
    return ,$buffer
}

function Invoke-Extractor([string[]]$ToolArgs) {
    & $script:Extractor @ToolArgs
    if ($LASTEXITCODE -ne 0) { throw "3dstool failed with exit code $LASTEXITCODE. No upload ZIP was completed." }
}

$sourceFolder = (Get-Item -LiteralPath $ExtractionFolder).FullName
$mainCxi = Get-Item -LiteralPath (Join-Path $sourceFolder 'work/main.cxi')
if ($mainCxi.PSIsContainer) { throw 'work/main.cxi must be a file.' }
$stream = [IO.File]::Open($mainCxi.FullName, 'Open', 'Read', 'Read')
try {
    $ncch = Read-Range $stream 0 0x200
    if ([Text.Encoding]::ASCII.GetString($ncch, 0x100, 4) -ne 'NCCH') { throw 'work/main.cxi is not an NCCH image.' }
    if (($ncch[0x18F] -band 4) -eq 0) { throw 'The main partition is marked encrypted; a decrypted extraction is required.' }
    $programId = [BitConverter]::ToUInt64($ncch, 0x118).ToString('X16')
    if ($programId -ne '0004000000055D00') { throw "Unexpected title ID $programId; this helper targets the Pokemon X base game." }
    if ($ncch[0x18E] -gt 8) { throw 'Unsupported NCCH media unit size.' }
    [long]$unit = [long]512 -shl [int]$ncch[0x18E]
    [long]$romfsOffset = [long][BitConverter]::ToUInt32($ncch, 0x1B0) * $unit
    [long]$romfsSize = [long][BitConverter]::ToUInt32($ncch, 0x1B4) * $unit
    if ($romfsOffset -lt 0x200 -or $romfsSize -lt 4 -or $romfsOffset + $romfsSize -gt $stream.Length) {
        throw 'The RomFS region is absent, truncated, or has an unsupported layout.'
    }
    $magic = Read-Range $stream $romfsOffset 4
    if ([Text.Encoding]::ASCII.GetString($magic) -ne 'IVFC') { throw 'RomFS is not a readable IVFC image; extraction needs review.' }
    $productCode = [Text.Encoding]::ASCII.GetString($ncch, 0x150, 16).Trim([char]0)
} finally { $stream.Dispose() }

$toolOverride = $false
$toolHash = $null
if (-not $ResumeLatest) {
    $toolOverride = -not [string]::IsNullOrWhiteSpace($ToolPath)
    if (-not $toolOverride) { $ToolPath = Join-Path $sourceFolder 'tool/3dstool.exe' }
    $script:Extractor = (Get-Item -LiteralPath $ToolPath).FullName
    $toolHash = (Get-FileHash -LiteralPath $script:Extractor -Algorithm SHA256).Hash.ToLowerInvariant()
    # Measured from the same pinned Windows release used by Extract-PokemonX.ps1.
    if (-not $toolOverride -and $toolHash -ne '967fd5ec6476df1fa6a01da0df5a1fea339aa488c10be218d38e07f4b8143b7e') {
        throw 'The existing 3dstool.exe differs from the pinned release. It will not be executed.'
    }
}

if ([string]::IsNullOrWhiteSpace($OutputRoot)) { $OutputRoot = (Get-Item -LiteralPath $sourceFolder).Parent.FullName }
$parent = [IO.Path]::GetFullPath($OutputRoot)
$null = New-Item -ItemType Directory -Path $parent -Force
$resumeManifest = $null
if ($ResumeLatest) {
    # The old script wrote manifest.json only after extraction and module hashing.
    # Exclude the kit folder and unfinished extractions without that manifest.
    $candidates = @(Get-ChildItem -LiteralPath $parent -Directory |
        Where-Object {
            $_.Name -match '^PokemonX-modules-\d{8}-\d{6}-[0-9a-f]{8}$' -and
            (Test-Path -LiteralPath (Join-Path $_.FullName 'manifest.json') -PathType Leaf) -and
            (Test-Path -LiteralPath (Join-Path $_.FullName 'work/romfs') -PathType Container)
        } | Sort-Object Name -Descending)
    if ($candidates.Count -eq 0) {
        throw "No completed module extraction was found in $parent. Run without -ResumeLatest for a fresh extraction."
    }
    $job = $candidates[0].FullName
    $resumeManifest = Get-Content -LiteralPath (Join-Path $job 'manifest.json') -Raw | ConvertFrom-Json
    if ($resumeManifest.schema_version -ne 1 -or $resumeManifest.program_id -ne $programId) {
        throw 'The saved module manifest has an unsupported schema or a different title ID.'
    }
    if (Test-Path -LiteralPath (Join-Path $job 'PokemonX-modules-upload.zip')) {
        throw "An upload ZIP already exists in $job. It has not been overwritten."
    }
    Write-Host "Reusing the completed RomFS extraction: $job"
} else {
    $job = Join-Path $parent ('PokemonX-modules-' + (Get-Date -Format 'yyyyMMdd-HHmmss') + '-' + [Guid]::NewGuid().ToString('N').Substring(0, 8))
    $null = New-Item -ItemType Directory -Path $job
    $null = New-Item -ItemType Directory -Path (Join-Path $job 'work')
    Write-Host ('RomFS extraction will use approximately {0:N2} GiB of additional disk space.' -f (2 * $romfsSize / 1GB))
}
$work = Join-Path $job 'work'
$romfsBin = Join-Path $work 'romfs.bin'
$romfsDir = Join-Path $work 'romfs'
Write-Host 'Hashing the existing main.cxi...'
$mainHash = (Get-FileHash -LiteralPath $mainCxi.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
if ($ResumeLatest) {
    if ($resumeManifest.source_main_cxi.sha256 -ne $mainHash -or $resumeManifest.source_main_cxi.size -ne $mainCxi.Length) {
        throw 'The saved extraction does not match main.cxi. No upload ZIP was created.'
    }
    Write-Host 'Verifying the extracted module files; RomFS extraction will be skipped.'
} else {
    Push-Location $work
    try {
        Write-Host 'Extracting the RomFS image...'
        Invoke-Extractor -ToolArgs @('-xtf', 'cxi', $mainCxi.FullName, '--romfs', $romfsBin)
        Write-Host 'Extracting the RomFS files; this may take a few minutes...'
        Invoke-Extractor -ToolArgs @('-xtf', 'romfs', $romfsBin, '--romfs-dir', $romfsDir)
    } finally { Pop-Location }
}

$modules = @(Get-ChildItem -LiteralPath $romfsDir -Recurse -File -Force |
    Where-Object { $_.Extension.ToLowerInvariant() -in @('.cro', '.crs', '.crr') } |
    Sort-Object FullName)
if ($modules.Count -eq 0) { throw 'No CRO, CRS or CRR files were found. No upload ZIP was created.' }
$basePath = (Get-Item -LiteralPath $romfsDir).FullName.TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
$entries = @()
foreach ($file in $modules) {
    if (-not $file.FullName.StartsWith($basePath, [StringComparison]::OrdinalIgnoreCase)) { throw 'A module path is outside the extracted RomFS.' }
    $relative = $file.FullName.Substring($basePath.Length).Replace('\', '/')
    $entries += [ordered]@{
        path = 'modules/' + $relative
        romfs_path = $relative
        size = $file.Length
        sha256 = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    }
}
$sourceManifest = Join-Path $sourceFolder 'upload/manifest.json'
$sourceProvenance = $null
if (Test-Path -LiteralPath $sourceManifest -PathType Leaf) {
    $sourceProvenance = [ordered]@{
        name = 'original-extraction-manifest.json'
        sha256 = (Get-FileHash -LiteralPath $sourceManifest -Algorithm SHA256).Hash.ToLowerInvariant()
    }
}
$manifest = [ordered]@{
    schema_version = 1
    target = 'Pokemon X'
    created_utc = [DateTime]::UtcNow.ToString('o')
    program_id = $programId
    product_code = $productCode
    source_main_cxi = @{ name = 'main.cxi'; size = $mainCxi.Length; sha256 = $mainHash }
    source_extraction_manifest = $sourceProvenance
    tool = @{ expected_version = '3dstool 1.2.6'; sha256 = $toolHash; explicit_override = $toolOverride }
    counts = @{
        cro = @($modules | Where-Object { $_.Extension -ieq '.cro' }).Count
        crs = @($modules | Where-Object { $_.Extension -ieq '.crs' }).Count
        crr = @($modules | Where-Object { $_.Extension -ieq '.crr' }).Count
    }
    files = $entries
    scope = 'CRO/CRS/CRR files from the main cartridge RomFS. Separately installed updates are not included.'
}
$jsonPath = Join-Path $job 'manifest.json'
if ($ResumeLatest) {
    $savedFiles = @($resumeManifest.files)
    if ($savedFiles.Count -ne $entries.Count) { throw 'The module file count differs from the saved manifest.' }
    $savedByPath = New-Object 'System.Collections.Generic.Dictionary[string,object]' ([StringComparer]::Ordinal)
    foreach ($saved in $savedFiles) { $savedByPath.Add([string]$saved.path, $saved) }
    foreach ($entry in $entries) {
        if (-not $savedByPath.ContainsKey($entry.path)) { throw "Unrecorded module: $($entry.path)" }
        $saved = $savedByPath[$entry.path]
        if ($saved.romfs_path -cne $entry.romfs_path -or $saved.size -ne $entry.size -or $saved.sha256 -ne $entry.sha256) {
            throw "Module differs from the saved manifest: $($entry.path)"
        }
    }
    foreach ($kind in @('cro', 'crs', 'crr')) {
        if ($resumeManifest.counts.$kind -ne $manifest.counts[$kind]) { throw 'Module counts differ from the saved manifest.' }
    }
    if ($null -ne $resumeManifest.source_extraction_manifest) {
        if ($null -eq $sourceProvenance -or $resumeManifest.source_extraction_manifest.sha256 -ne $sourceProvenance.sha256) {
            throw 'The original extraction manifest is missing or has changed.'
        }
    } else { $sourceProvenance = $null }
    # Preserve the recorded extraction time and extractor provenance on recovery.
    $manifest = $resumeManifest
} else {
    $manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $jsonPath -Encoding UTF8
}

# ZipArchive includes dot-prefixed paths such as .crr/static.crr on every platform.
$zip = Join-Path $job 'PokemonX-modules-upload.zip'
# A failed old run may have left a .zip.partial file; leave it untouched.
$zipPartial = $zip + '.' + [Guid]::NewGuid().ToString('N') + '.partial'
$zipStream = [IO.File]::Open($zipPartial, 'CreateNew', 'Write', 'None')
$zipArchive = $null
try {
    $zipArchive = New-Object IO.Compression.ZipArchive($zipStream, [IO.Compression.ZipArchiveMode]::Create, $false)
    for ($i = 0; $i -lt $modules.Count; $i++) {
        $null = [IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zipArchive, $modules[$i].FullName, $entries[$i].path, [IO.Compression.CompressionLevel]::Optimal)
    }
    $null = [IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zipArchive, $jsonPath, 'manifest.json', [IO.Compression.CompressionLevel]::Optimal)
    if ($null -ne $sourceProvenance) {
        $null = [IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zipArchive, $sourceManifest, 'original-extraction-manifest.json', [IO.Compression.CompressionLevel]::Optimal)
    }
} finally {
    if ($null -ne $zipArchive) { $zipArchive.Dispose() }
    $zipStream.Dispose()
}
Move-Item -LiteralPath $zipPartial -Destination $zip
Write-Host ''
Write-Host ("Packaged {0} CRO, {1} CRS and {2} CRR files." -f $manifest.counts.cro, $manifest.counts.crs, $manifest.counts.crr)
Write-Host 'Upload this ZIP in the chat:'
Write-Host $zip
Write-Host ('ZIP size: {0:N2} MiB' -f ((Get-Item -LiteralPath $zip).Length / 1MB))
Write-Host 'The extracted inputs are unchanged. The work folder retains the full extracted RomFS for local inspection.'
