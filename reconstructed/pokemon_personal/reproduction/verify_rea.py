#!/usr/bin/env python3
"""Verify personal-data assembly, literals and live REA load observations."""
import argparse
import hashlib
import json
from pathlib import Path


def require(condition, message):
    if not condition:
        raise ValueError(message)


def verify(code, evidence):
    pinned = '0f6d3e1d66cbcd55d46fa453db4b6aa489e03c3537c6b24026f2a53af142797a'
    provenance = json.loads((evidence / 'provenance.json').read_text())
    require(hashlib.sha256(code).hexdigest() == pinned == provenance['input']['sha256'],
            'Code differs from the pinned input')
    records = json.loads((evidence / 'rea-evidence.json').read_text())
    target = json.loads((evidence / 'target.json').read_text())
    require(target['elf']['sha256'] == provenance['target_sha256'], 'Target identity mismatch')
    loaded = next(r['normalized_result'] for r in records if r['operation'] == 'list_segments')
    for section in target['mapping']:
        actual = next(s for s in loaded if s['name'] == section['section'] and s['address_space'] == 'ram')
        start = int(section['address'], 16)
        require(int(actual['start'], 16) == start and int(actual['end'], 16) == start + section['size'],
                'Live Ghidra section address/size mismatch')
        require([actual[k] for k in ('readable', 'writable', 'executable')] ==
                [section[k] for k in ('read', 'write', 'execute')], 'Live section permissions mismatch')
        initialized = section['code_offset'] is not None
        require(actual['initialized'] == initialized and not actual['overlay'],
                'Live initialization/overlay mismatch')
        if initialized:
            offset = section['code_offset']
            require(hashlib.sha256(code[offset:offset + section['size']]).hexdigest() == section['sha256'],
                    'Original section digest mismatch')
    live = {}
    functions = {}
    for record in records:
        require(record['subject']['digest']['sha256'] == provenance['target_sha256'], 'Mixed targets')
        require(record['analysis_profile'] == provenance['analysis_profile'], 'Mixed analysis profiles')
        if record['operation'] == 'analyze_function':
            n = record['normalized_result']
            functions[record['evidence_id']] = n
        if record['operation'] != 'read_bytes':
            continue
        n = record['normalized_result']
        raw = bytes.fromhex(n['bytes_hex'])
        address = int(n['address'], 16)
        require(n['complete'] and len(raw) == n['requested_bytes'] == n['returned_bytes'],
                'Incomplete live byte response')
        offset = address - 0x100000
        require(offset >= 0 and raw == code[offset:offset + len(raw)],
                'Live REA bytes differ from original code')
        for i, byte in enumerate(raw):
            require(live.setdefault(address + i, byte) == byte, 'Conflicting live byte responses')
    for dependency in json.loads((evidence / 'dependencies.json').read_text()):
        functions[dependency['source_evidence_id']] = dependency
    total = implemented = 0
    for row in json.loads((evidence / 'functions.json').read_text()):
        original = functions[row['evidence_id']]
        require(original['procedure']['body']['ranges'] == row['body_ranges'], 'Body range mismatch')
        encoded = bytearray()
        assembly = []
        for line in (evidence / (row['address'] + '.asm')).read_text().splitlines():
            address, raw, text = line.split(None, 2)
            address, raw = int(address, 16), bytes.fromhex(raw)
            require(raw == bytes(live[address + i] for i in range(len(raw))),
                    'Retained instruction differs from live REA bytes')
            require(any(int(r['start'], 16) <= address and address + len(raw) - 1 <= int(r['end'], 16)
                        for r in row['body_ranges']), 'Instruction outside recorded function body')
            encoded.extend(raw)
            assembly.append(f'0x{address:x}: {text}')
        require(assembly == original['assembly'], 'Instruction text differs from REA observation')
        require((evidence / (row['address'] + '.c')).read_text().split('\n', 1)[1] == original['pseudocode'],
                'Pseudocode differs from REA observation')
        checked = next(v for v in provenance['verified_functions'] if v['address'] == row['address'])
        require(len(encoded) == row['body_bytes'] == original['procedure']['body']['total_bytes'],
                'Instruction size mismatch')
        require(hashlib.sha256(encoded).hexdigest() == checked['instruction_bytes_sha256'],
                'Instruction digest mismatch')
        total += len(encoded)
        if row['implemented']:
            implemented += len(encoded)
    literals = json.loads((evidence / 'literals.json').read_text())
    for address, raw in literals.items():
        require(bytes.fromhex(raw) == bytes(live[int(address, 16) + i] for i in range(4)),
                'Retained literal differs from live REA bytes')
    require(total == provenance['instruction_bytes'] and implemented == provenance['implemented_instruction_bytes']
            and 4 * len(literals) == provenance['literal_bytes'], 'Evidence totals mismatch')
    return total, implemented, 4 * len(literals)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--code', type=Path, required=True)
    parser.add_argument('--evidence', type=Path, default=Path(__file__).resolve().parents[1] / 'evidence')
    args = parser.parse_args()
    try:
        total, implemented, literals = verify(args.code.read_bytes(), args.evidence)
    except (OSError, ValueError, KeyError, StopIteration) as exc:
        parser.exit(1, f'error: {exc}\n')
    print(f'Verified {total} instruction bytes ({implemented} implemented), {literals} literal bytes against live REA and original code')


if __name__ == '__main__':
    main()
