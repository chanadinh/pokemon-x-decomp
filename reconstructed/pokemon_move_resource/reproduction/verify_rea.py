#!/usr/bin/env python3
"""Verify retained instructions/literals against live REA replies and code.bin."""
import argparse
import hashlib
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--code', type=Path, required=True)
    parser.add_argument('--evidence', type=Path, default=Path(__file__).resolve().parents[1] / 'evidence')
    args = parser.parse_args()
    code = args.code.read_bytes()
    provenance = json.loads((args.evidence / 'provenance.json').read_text())
    pinned = '0f6d3e1d66cbcd55d46fa453db4b6aa489e03c3537c6b24026f2a53af142797a'
    if hashlib.sha256(code).hexdigest() != pinned or provenance['input']['sha256'] != pinned:
        raise SystemExit('Code differs from the recorded input')
    live = {}
    records = json.loads((args.evidence / 'rea-evidence.json').read_text())
    target = json.loads((args.evidence / 'target.json').read_text())
    if target['elf']['sha256'] != provenance['target_sha256']:
        raise SystemExit('Analysis carrier identity mismatch')
    loaded = next(r['normalized_result'] for r in records if r['operation'] == 'list_segments')
    for section in target['mapping']:
        actual = next(s for s in loaded if s['name'] == section['section'] and s['address_space'] == 'ram')
        start = int(section['address'], 16)
        if int(actual['start'], 16) != start or int(actual['end'], 16) != start + section['size']:
            raise SystemExit('Live Ghidra section address/size mismatch')
        if [actual[k] for k in ('readable', 'writable', 'executable')] != [section[k] for k in ('read', 'write', 'execute')]:
            raise SystemExit('Live Ghidra permissions differ from ExHeader mapping')
        initialized = section['code_offset'] is not None
        if actual['initialized'] != initialized or actual['overlay']:
            raise SystemExit('Live Ghidra initialization/overlay mismatch')
        if initialized:
            offset = section['code_offset']
            if hashlib.sha256(code[offset:offset + section['size']]).hexdigest() != section['sha256']:
                raise SystemExit('Original section digest mismatch')
    for record in records:
        if record['subject']['digest']['sha256'] != provenance['target_sha256']:
            raise SystemExit('Mixed analysis targets')
        if record['operation'] != 'read_bytes':
            continue
        n = record['normalized_result']
        raw = bytes.fromhex(n['bytes_hex'])
        address = int(n['address'], 16)
        if not n['complete'] or len(raw) != n['requested_bytes'] or len(raw) != n['returned_bytes']:
            raise SystemExit('Incomplete live byte response')
        if raw != code[address - 0x100000:address - 0x100000 + len(raw)]:
            raise SystemExit('Live REA bytes differ from original code')
        for offset, byte in enumerate(raw):
            if live.setdefault(address + offset, byte) != byte:
                raise SystemExit('Conflicting live byte responses')
    total = 0
    for row in json.loads((args.evidence / 'functions.json').read_text()):
        encoded = bytearray()
        for line in (args.evidence / (row['address'] + '.asm')).read_text().splitlines():
            address, raw, _ = line.split(None, 2)
            address, raw = int(address, 16), bytes.fromhex(raw)
            if raw != bytes(live[address + i] for i in range(len(raw))):
                raise SystemExit('Retained instruction differs from live REA bytes')
            encoded.extend(raw)
        verified = next(v for v in provenance['verified_functions'] if v['address'] == row['address'])
        if len(encoded) != row['body_bytes'] or hashlib.sha256(encoded).hexdigest() != verified['instruction_bytes_sha256']:
            raise SystemExit('Instruction size/hash mismatch')
        total += len(encoded)
    literals = json.loads((args.evidence / 'literals.json').read_text())
    for address, raw in literals.items():
        if bytes.fromhex(raw) != bytes(live[int(address, 16) + i] for i in range(4)):
            raise SystemExit('Retained literal differs from live REA bytes')
    if total != provenance['instruction_bytes'] or 4 * len(literals) != provenance['literal_bytes']:
        raise SystemExit('Evidence totals differ')
    print(f'Verified live REA evidence: {total} instruction bytes, {4 * len(literals)} literal bytes')


if __name__ == '__main__':
    main()
