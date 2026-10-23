#!/usr/bin/env python3
"""Exercise real Ghidra using invented ARM instructions, not game data."""
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    with tempfile.TemporaryDirectory(prefix="synthetic-arm-", dir=ROOT / "inputs") as folder:
        path = Path(folder)
        header = bytearray(0x400)
        header[:8] = b"TESTONLY"
        struct.pack_into("<III", header, 0x10, 0x100000, 1, 8)
        struct.pack_into("<I", header, 0x1C, 0x1000)
        struct.pack_into("<III", header, 0x20, 0x101000, 1, 16)
        struct.pack_into("<III", header, 0x30, 0x102000, 1, 16)
        struct.pack_into("<I", header, 0x3C, 32)
        # ARM: add r0, r0, #1; bx lr. No Nintendo/game code.
        code = bytearray(3 * 0x1000)
        struct.pack_into("<II", code, 0, 0xE2800001, 0xE12FFF1E)
        code[0x1000:0x1010] = b"SYNTHETIC ONLY\0\0"
        (path / "code.bin").write_bytes(code)
        (path / "exheader.bin").write_bytes(header)
        result = subprocess.run([sys.executable, str(ROOT / "scripts/run_decompile.py"),
                                 "--code", str(path / "code.bin"),
                                 "--exheader", str(path / "exheader.bin"),
                                 "--max-functions", "10"], text=True, capture_output=True)
        print(result.stdout, end="")
        if result.returncode:
            print(result.stderr, file=sys.stderr)
            raise SystemExit(result.returncode)
        output = next(line.removeprefix("Output: ") for line in result.stdout.splitlines() if line.startswith("Output: "))
        summary = json.loads((Path(output) / "summary.json").read_text())
        assert summary["succeeded"] >= 1, summary
        functions = json.loads((Path(output) / "functions.json").read_text())
        entry = next(row for row in functions if int(row["address"], 16) == 0x100000)
        pseudocode = (Path(output) / entry["file"]).read_text()
        assert entry["mode"] == "ARM", entry
        assert "+ 1" in pseudocode and "return" in pseudocode, pseudocode
        (Path(output) / "SYNTHETIC_FIXTURE.txt").write_text("This run used an invented ARM add-one function. It contains no Omega Ruby data.\n")
        print("PASS: synthetic ARM add-one function imported and decompiled.")


if __name__ == "__main__":
    main()
