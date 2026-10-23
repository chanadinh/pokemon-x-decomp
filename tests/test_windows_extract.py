"""Real extraction smoke tests with tiny invented NCSD/NCCH/ExeFS fixtures.

No game code or assets are used. Requires portable PowerShell and 3dstool under
tools/, or override POKEMON_X_PWSH and POKEMON_X_3DSTOOL environment variables.
"""
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest
import zipfile


ROOT = Path(__file__).resolve().parents[1]
PWSH = Path(os.environ.get("POKEMON_X_PWSH", ROOT / "tools/pwsh/pwsh"))
TOOL = Path(os.environ.get("POKEMON_X_3DSTOOL", ROOT / "tools/3dstool-linux/3dstool"))
SCRIPT = ROOT / "windows/Extract-PokemonX.ps1"
PROGRAM_ID = 0x00040000ABCDEF00
PARTITION_OFFSET = 0x4000


def align(value, alignment=0x200):
    return (value + alignment - 1) // alignment * alignment


def make_fixture(folder, compressed=False, encrypted=False):
    """Return (ROM path, decompressed code bytes, original ExHeader bytes)."""
    code = bytearray(0x3000)
    # ARM add r0,r0,#1; bx lr. The remaining bytes are invented padding/data.
    struct.pack_into("<II", code, 0, 0xE2800001, 0xE12FFF1E)
    code[0x1000:0x1010] = b"SYNTHETIC ONLY\0\0"
    code[0x2000:0x2010] = bytes(range(16))
    raw_code = bytes(code)
    stored_code = raw_code
    if compressed:
        raw_path = folder / "fixture-raw.bin"
        blz_path = folder / "fixture-blz.bin"
        raw_path.write_bytes(raw_code)
        result = subprocess.run(
            [str(TOOL), "-zvf", str(raw_path), "--compress-type", "blz", "--compress-out", str(blz_path)],
            text=True, capture_output=True, timeout=30,
        )
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
        stored_code = blz_path.read_bytes()
        if not 0 < len(stored_code) < len(raw_code):
            raise AssertionError("Synthetic BLZ fixture did not compress")

    header = bytearray(0x800)
    header[:8] = b"TESTONLY"
    header[0x0D] = int(compressed)
    struct.pack_into("<III", header, 0x10, 0x100000, 1, 8)
    struct.pack_into("<I", header, 0x1C, 0x1000)
    struct.pack_into("<III", header, 0x20, 0x101000, 1, 16)
    struct.pack_into("<III", header, 0x30, 0x102000, 1, 16)
    struct.pack_into("<I", header, 0x3C, 32)
    struct.pack_into("<Q", header, 0x200, PROGRAM_ID)

    exefs = bytearray(align(0x200 + len(stored_code)))
    exefs[:8] = b".code\0\0\0"
    struct.pack_into("<II", exefs, 8, 0, len(stored_code))
    # ExeFS hashes are stored in reverse entry order; entry0's hash is at0x1E0.
    exefs[0x1E0:0x200] = hashlib.sha256(stored_code).digest()
    exefs[0x200:0x200 + len(stored_code)] = stored_code

    ncch = bytearray(align(0xA00 + len(exefs)))
    ncch[0x100:0x104] = b"NCCH"
    struct.pack_into("<I", ncch, 0x104, len(ncch) // 0x200)
    struct.pack_into("<Q", ncch, 0x108, PROGRAM_ID)
    ncch[0x110:0x112] = b"00"
    struct.pack_into("<H", ncch, 0x112, 2)
    struct.pack_into("<Q", ncch, 0x118, PROGRAM_ID)
    ncch[0x150:0x15A] = b"CTR-P-TEST"
    ncch[0x160:0x180] = hashlib.sha256(header[:0x400]).digest()
    struct.pack_into("<I", ncch, 0x180, 0x400)
    ncch[0x18C] = 1  # CTR platform.
    ncch[0x18D] = 2  # Executable content without RomFS.
    ncch[0x18F] = 0 if encrypted else 4
    struct.pack_into("<III", ncch, 0x1A0, 0xA00 // 0x200, len(exefs) // 0x200, 1)
    ncch[0x1C0:0x1E0] = hashlib.sha256(exefs[:0x200]).digest()
    ncch[0x200:0xA00] = header
    ncch[0xA00:0xA00 + len(exefs)] = exefs

    rom = bytearray(PARTITION_OFFSET + len(ncch))
    rom[0x100:0x104] = b"NCSD"
    struct.pack_into("<I", rom, 0x104, len(rom) // 0x200)
    struct.pack_into("<Q", rom, 0x108, PROGRAM_ID)
    struct.pack_into("<II", rom, 0x120, PARTITION_OFFSET // 0x200, len(ncch) // 0x200)
    rom[0x18C] = 1
    rom[0x18D] = 1
    struct.pack_into("<Q", rom, 0x190, PROGRAM_ID)
    rom[PARTITION_OFFSET:] = ncch
    path = folder / "Synthetic X (not a game).3ds"
    path.write_bytes(rom)
    return path, raw_code, bytes(header)


class WindowsExtractionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        missing = [str(path) for path in (PWSH, TOOL, SCRIPT) if not path.is_file()]
        if missing:
            raise unittest.SkipTest("Required local tools/script missing: " + ", ".join(missing))

    def invoke(self, folder, path):
        # Keep PowerShell's Linux state inside the fixture, including when the
        # caller's home directory is read-only (as in the build sandbox).
        env = os.environ.copy()
        for key, suffix in [("XDG_CACHE_HOME", "cache"), ("XDG_CONFIG_HOME", "config"),
                            ("XDG_DATA_HOME", "data")]:
            state = folder / "powershell-state" / suffix
            state.mkdir(parents=True, exist_ok=True)
            env[key] = str(state)
        result = subprocess.run(
            [str(PWSH), "-NoLogo", "-NoProfile", "-NonInteractive", "-File", str(SCRIPT),
             "-RomPath", str(path), "-OutputRoot", str(folder / "out with spaces"),
             "-ToolPath", str(TOOL), "-AllowDifferentTitle"],
            text=True, capture_output=True, timeout=90, env=env,
        )
        return result

    def assert_round_trip(self, compressed):
        with tempfile.TemporaryDirectory(prefix="synthetic-x-") as temp:
            folder = Path(temp)
            rom, expected_code, expected_header = make_fixture(folder, compressed=compressed)
            original_hash = hashlib.sha256(rom.read_bytes()).hexdigest()
            result = self.invoke(folder, rom)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertEqual(hashlib.sha256(rom.read_bytes()).hexdigest(), original_hash)
            outputs = list((folder / "out with spaces").rglob("*.zip"))
            self.assertEqual(len(outputs), 1, result.stdout + result.stderr)
            with zipfile.ZipFile(outputs[0]) as archive:
                names = {Path(name).name: name for name in archive.namelist() if not name.endswith("/")}
                self.assertIn("code.bin", names)
                self.assertIn("exheader.bin", names)
                self.assertIn("manifest.json", names)
                self.assertEqual(archive.read(names["code.bin"]), expected_code)
                self.assertEqual(archive.read(names["exheader.bin"]), expected_header)
                manifest = json.loads(archive.read(names["manifest.json"]).decode("utf-8-sig"))
                self.assertIsInstance(manifest, dict)

    def test_uncompressed_code_round_trip_and_original_preserved(self):
        self.assert_round_trip(compressed=False)

    def test_blz_code_round_trip_and_original_preserved(self):
        self.assert_round_trip(compressed=True)

    def test_encrypted_header_rejected_without_changing_original(self):
        with tempfile.TemporaryDirectory(prefix="synthetic-x-encrypted-") as temp:
            folder = Path(temp)
            rom, _, _ = make_fixture(folder, encrypted=True)
            original_hash = hashlib.sha256(rom.read_bytes()).hexdigest()
            result = self.invoke(folder, rom)
            self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertRegex((result.stdout + result.stderr).lower(), "encrypt|decrypt")
            self.assertEqual(hashlib.sha256(rom.read_bytes()).hexdigest(), original_hash)
            self.assertEqual(list((folder / "out with spaces").rglob("*.zip")), [])


if __name__ == "__main__":
    unittest.main()
