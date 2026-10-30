#!/usr/bin/env python3
"""Verify the battle type-effectiveness evidence against live REA observations and DllBattle.cro.

Checks the original module hash, that every retained instruction and literal
word equals the live REA bytes, and that each of those words equals the
original CRO word at its file offset, except recorded ABS32 relocations, whose
original word must match and whose value must equal symbol plus addend. It also
checks the instruction text and pseudocode against the REA observation, and
that the type chart and power table equal the live bytes and the module bytes.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def require(condition, message):
    if not condition:
        raise ValueError(message)


def verify(cro, evidence):
    provenance = json.loads((evidence / 'provenance.json').read_text())
    require(hashlib.sha256(cro).hexdigest() == provenance['module']['sha256'], 'DllBattle.cro differs from the pinned module')
    records = json.loads((evidence / 'rea-evidence.json').read_text())
    live = {}
    for record in records:
        require(record['subject']['digest']['sha256'] == provenance['carrier']['sha256'], 'Mixed analysis targets')
        if record['operation'] != 'read_bytes':
            continue
        n = record['normalized_result']
        raw = bytes.fromhex(n['bytes_hex'])
        require(n['complete'] and len(raw) == n['returned_bytes'], 'Incomplete live byte response')
        for i, byte in enumerate(raw):
            require(live.setdefault(int(n['address'], 16) + i, byte) == byte, 'Conflicting live bytes')
    relocations = {r['address']: r for r in json.loads((evidence / 'relocations.json').read_text())}
    text_offset = provenance['module']['text_file_offset']
    text_address = int(provenance['module']['text_analysis_address'], 16)

    def check_word(address):
        word = bytes(live[address + i] for i in range(4))
        offset = text_offset + address - text_address
        original = cro[offset:offset + 4]
        r = relocations.get(address)
        if r:
            require(struct.unpack('<I', original)[0] == r['original_word'], 'Relocated word differs from original CRO')
            require(r['value'] == (r['symbol_address'] + r['addend']) & 0xffffffff, 'Relocation value is not S+A')
            require(struct.unpack('<I', word)[0] == r['value'], 'Live word differs from relocation value')
        else:
            require(word == original, 'Live word %#x differs from original CRO' % address)
        return word

    total = 0
    for row in json.loads((evidence / 'functions.json').read_text()):
        original = next(r['normalized_result'] for r in records if r['evidence_id'] == row['evidence_id'])
        encoded, texts = bytearray(), []
        for line in (evidence / (row['address'] + '.asm')).read_text().splitlines():
            address, raw, text = line.split(None, 2)
            address, raw = int(address, 16), bytes.fromhex(raw)
            require(raw == check_word(address), 'Retained instruction differs from live bytes')
            encoded.extend(raw)
            texts.append(f'0x{address:x}: {text}')
        require(texts == original['assembly'], 'Instruction text differs from REA observation')
        require((evidence / (row['address'] + '.c')).read_text().split('\n', 1)[1] == original['pseudocode'],
                'Pseudocode differs from REA observation')
        checked = next(v for v in provenance['verified_functions'] if v['address'] == row['address'])
        require(len(encoded) == row['body_bytes'] == checked['instruction_bytes'] and
                hashlib.sha256(encoded).hexdigest() == checked['instruction_bytes_sha256'], 'Instruction digest mismatch')
        total += len(encoded)
    require(total == provenance['instruction_bytes'], 'Instruction total mismatch')
    literals = json.loads((evidence / 'literals.json').read_text())
    for address, raw in literals.items():
        require(check_word(int(address, 16)).hex() == raw, 'Literal differs from live bytes')
    data = 0
    for address, block in json.loads((evidence / 'data.json').read_text()).items():
        start, size = int(address, 16), block['size']
        raw = bytes(live[start + i] for i in range(size))
        offset = text_offset + start - text_address
        require(raw == cro[offset:offset + size], 'Data block %s differs from original CRO' % address)
        require(hashlib.sha256(raw).hexdigest() == block['sha256'], 'Data block digest mismatch')
        require(not [a for a in relocations if start <= a < start + size], 'Relocation inside data block')
        data += size
    require(data == provenance['data_bytes'], 'Data total mismatch')
    table = struct.unpack('<14I', bytes(live[0x108d2248 + i] for i in range(56)))
    require(table == tuple([0] + [1 << i for i in range(13)]), 'Power table differs')
    require(len(relocations) == len(literals) == 17, 'Expected 3 literal and 14 jump-table relocations')
    return total, 4 * len(literals), len(relocations), data


def main():
    root = Path(__file__).resolve().parents[3]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cro', type=Path, default=root / 'inputs/pokemon-x-modules-a035d9775a1f/modules/DllBattle.cro')
    parser.add_argument('--evidence', type=Path, default=Path(__file__).resolve().parents[1] / 'evidence')
    args = parser.parse_args()
    try:
        instructions, literals, relocations, data = verify(args.cro.read_bytes(), args.evidence)
    except (OSError, ValueError, KeyError, StopIteration) as exc:
        parser.exit(1, f'error: {exc}\n')
    print(f'Verified {instructions} instruction bytes, {literals} literal bytes and {data} data bytes against '
          f'live REA and DllBattle.cro ({relocations} relocations)')


if __name__ == '__main__':
    main()
