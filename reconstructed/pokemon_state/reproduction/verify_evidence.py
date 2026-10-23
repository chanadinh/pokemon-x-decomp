#!/usr/bin/env python3
"""Compare exported ARM instructions and literal words with the pinned input."""
import argparse
import hashlib
import json
from pathlib import Path


def digest(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--code', type=Path, required=True)
    parser.add_argument('--evidence', type=Path, required=True)
    parser.add_argument('--write-provenance', action='store_true')
    args = parser.parse_args()
    code = args.code.read_bytes()
    expected = '0f6d3e1d66cbcd55d46fa453db4b6aa489e03c3537c6b24026f2a53af142797a'
    if digest(code) != expected:
        raise SystemExit('Input does not match the pinned Pokemon X executable')
    rows = json.loads((args.evidence / 'functions.json').read_text())
    verified = []
    for row in rows:
        instructions = bytearray()
        for line in (args.evidence / (row['address'] + '.asm')).read_text().splitlines():
            address, encoded, _ = line.split(None, 2)
            data = bytes.fromhex(encoded)
            offset = int(address, 16) - 0x100000
            if offset < 0 or code[offset:offset + len(data)] != data:
                raise SystemExit(f'Instruction mismatch at {address}')
            instructions.extend(data)
        verified.append({'address': row['address'], 'instruction_bytes': len(instructions),
                         'instruction_bytes_sha256': digest(instructions),
                         'matches_original_input': True, 'decompiled': row['decompiled']})
    literals = json.loads((args.evidence / 'literals.json').read_text())
    for address, encoded in literals.items():
        offset = int(address, 16) - 0x100000
        if offset < 0 or code[offset:offset + 4] != bytes.fromhex(encoded):
            raise SystemExit(f'Literal mismatch at {address}')
    result = {
        'input': {'file': 'code.bin', 'size': len(code), 'sha256': digest(code)},
        'verified_functions': verified,
        'verified_literal_words': literals,
        'instruction_bytes': sum(row['instruction_bytes'] for row in verified),
        'literal_bytes': len(literals) * 4,
        'notes': [
            'Each instruction and exported PC-relative literal compared at address minus 0x00100000.',
            'Read-only, noanalysis Ghidra export; no project repair or saved project mutation.',
            'Exports include explicit external dependencies, not only implemented functions.',
            'Static evidence and synthetic host tests do not establish original-game execution equivalence.'
        ]
    }
    if args.write_provenance:
        (args.evidence / 'provenance.json').write_text(json.dumps(result, indent=2) + '\n')
    elif json.loads((args.evidence / 'provenance.json').read_text()) != result:
        raise SystemExit('Provenance differs from the current evidence verification')
    print(f"Verified {len(verified)} functions, {result['instruction_bytes']} instruction bytes, "
          f"{result['literal_bytes']} literal bytes against {expected}")


if __name__ == '__main__':
    main()
