#!/usr/bin/env python3
"""Verify retained instructions, selector-5 dispatch and PP division."""
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
    # LDRCC pc,[pc,r1,lsl #2] at 00176330: ARM PC reads as address+8.
    def word(address):
        return struct.unpack_from('<I', code, address - 0x100000)[0]
    if word(0x176330) != 0x379ff101 or word(0x176338 + 5 * 4) != 0x17641c:
        raise SystemExit('Selector 5 does not dispatch to the reviewed byte +5 getter')
    if word(0x15025c) != 0x51eb851f:
        raise SystemExit('Unexpected maximum-PP division constant')
    if any((n * 0x51eb851f) >> 37 != n // 100 for n in range(15301)):
        raise SystemExit('Maximum-PP division reduction failed')
    print('Verified selector-5 jump-table entry and division for all 15,301 bounded numerators')


if __name__ == '__main__':
    main()
