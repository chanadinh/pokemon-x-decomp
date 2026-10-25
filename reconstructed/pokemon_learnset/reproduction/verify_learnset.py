#!/usr/bin/env python3
"""Verify retained instructions and the two form-selector dispatch words."""
import argparse
from pathlib import Path
import struct
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--code', type=Path, required=True)
    parser.add_argument('--evidence', type=Path, default=Path(__file__).resolve().parents[1] / 'evidence')
    args = parser.parse_args()
    verifier = Path(__file__).resolve().parents[2] / 'pokemon_stats/reproduction/verify_evidence.py'
    subprocess.run([sys.executable, str(verifier), '--code', str(args.code),
                    '--evidence', str(args.evidence)], check=True)
    code = args.code.read_bytes()
    expected = {0x175c80: 0x379ff101, 0x175d00: 0x175ecc, 0x175d08: 0x175ee4}
    for address, word in expected.items():
        if struct.unpack_from('<I', code, address - 0x100000)[0] != word:
            raise SystemExit(f'Personal selector dispatch mismatch at {address:08x}')
    print('Verified personal selectors 0x1e -> unsigned +0x1c and 0x20 -> unsigned +0x20')


if __name__ == '__main__':
    main()
