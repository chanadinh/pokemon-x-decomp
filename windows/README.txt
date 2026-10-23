Pokemon X: prepare a small analysis upload

1. Extract PokemonX-extraction-kit.zip to D:\Downloads\PokemonX-extraction-kit.
2. Open Windows PowerShell and run:

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "D:\Downloads\PokemonX-extraction-kit\Extract-PokemonX.ps1"

The default ROM is:
D:\Downloads\Pokemon X (USA) (En,Ja,Fr,De,Es,It,Ko).3ds

For another filename, append: -RomPath "D:\path\game.3ds"

Allow roughly 2 GiB of extra disk space for the temporary main-partition copy.
The helper downloads 3dstool 1.2.6 from its maintainer's GitHub release and checks
a pinned archive hash. It reads the original game without changing it.

On success, upload the PokemonX-analysis-upload.zip whose path it prints.
That ZIP contains only code.bin, exheader.bin, and a provenance manifest.
If it reports an encrypted image or another error, paste the error in the chat.
Do not upload the entire 2 GiB game image.

Tests: synthetic compressed/uncompressed extraction and encrypted-header
rejection passed under PowerShell 7.4.6/Linux with 3dstool 1.2.6. Native Windows
and your actual dump have not been run here.

Tool: https://github.com/dnasdw/3dstool/releases/tag/v1.2.6
Guide: https://github.com/ihaveamac/3DS-rom-tools/wiki/Extract-a-game-or-application-in-.3ds-or-.cci-format
