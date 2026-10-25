#!/usr/bin/env python3
"""Verify retained ARM evidence and the name writer's encoding fall-through."""
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
    expected = {0x1685e8: 0xe8bd4070, 0x1685ec: 0xe1a00000, 0x1685f0: 0xe92d4010}
    for address, word in expected.items():
        if struct.unpack_from('<I', code, address - 0x100000)[0] != word:
            raise SystemExit(f'Name writer/encoder boundary mismatch at {address:08x}')
    print('Verified name writer falls through to manager encoding at 001685f0')


if __name__ == '__main__':
    main()
