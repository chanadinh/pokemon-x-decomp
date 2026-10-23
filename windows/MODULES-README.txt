Pokemon X module extraction helper

RECOVER FROM THE ZipArchiveMode ERROR

Replace the previous script with this updated copy in
D:\Downloads\PokemonX-modules-kit, then run:

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "D:\Downloads\PokemonX-modules-kit\Extract-PokemonX-Modules.ps1" -ResumeLatest

This selects the latest completed module extraction beside your original
extraction, verifies its manifest and module hashes, and creates the upload ZIP
without extracting RomFS again. It retains the original manifest and ignores
any abandoned .partial ZIP. A complete ZIP is never overwritten. If an existing
ZIP is reported, use the PokemonX-modules-upload.zip in the printed folder.

The fix explicitly loads both .NET compression assemblies before extraction.
The earlier version loaded only System.IO.Compression.FileSystem, which did not
make ZipArchiveMode available in the user's Windows PowerShell 5.1 session.

FRESH EXTRACTION

1. Extract this ZIP into D:\Downloads\PokemonX-modules-kit.
   The script should be directly inside that folder.
2. Run this in PowerShell (no administrator session needed):

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "D:\Downloads\PokemonX-modules-kit\Extract-PokemonX-Modules.ps1"

3. Upload the PokemonX-modules-upload.zip whose path the script prints.

The helper uses your existing extraction:
D:\Downloads\PokemonX-analysis-20261023-102030-ff2c2690

It expects work\main.cxi and tool\3dstool.exe there. If you moved that folder,
append -ExtractionFolder "D:\path\your-extraction-folder" to the command.

The script verifies the header and pinned extractor, creates a new output
folder, extracts RomFS, and packages CRO/CRS/CRR files with their relative paths
and SHA-256 hashes. It includes .crr\static.crr. It also includes the original
extraction manifest when available. Separately installed updates are excluded.

Allow roughly twice the RomFS size in additional disk space; the script prints
the estimate. Extraction may take a few minutes. The new folder retains both
the RomFS image and extracted files. The previous extraction is read-only.
No download or upload occurs. The execution-policy option applies only to this
PowerShell process.

Tested with synthetic RomFS data under PowerShell 7.4.6 and real 3dstool 1.2.6
on Linux. Exact module bytes, paths, hashes, asset exclusion, and preservation
of the input passed the integration test. Recovery also passed with a leftover
partial ZIP and an extractor that fails if invoked. It preserved original
manifest bytes and rejected an altered module before creating a ZIP.
Native Windows execution of this
updated helper is pending; it targets Windows PowerShell 5.1 or newer.
