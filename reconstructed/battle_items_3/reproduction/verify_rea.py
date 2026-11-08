#!/usr/bin/env python3
"""Verify the battle item evidence (batch 3) against live REA observations and DllBattle.cro.

Checks the original module hash, that every retained instruction and literal
word equals the live REA bytes, and that each of those words equals the
original CRO word at its file offset, except recorded ABS32 relocations, whose
original word must match and whose value must equal symbol plus addend. It also
checks the instruction text and pseudocode against the REA observation.
Functions without a Ghidra function (decoded_by) are listed by the battle_power
milestone's decode_arm.py: their listing must equal the decoder's output, and
the decoder must reproduce every REA-listed instruction it supports. Each form
the decoded handlers use beyond those functions (here uxth r0, r0) is pinned
by a spelling reference: REA's listing of a word elsewhere in the module, which
the decoder must reproduce from the live bytes.
"""
import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'battle_power/reproduction'))
from decode_arm import decode  # noqa: E402


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
        decoded = 'decoded_by' in row
        original = None if decoded else next(r['normalized_result'] for r in records if r['evidence_id'] == row['evidence_id'])
        encoded, texts = bytearray(), []
        for line in (evidence / (row['address'] + '.asm')).read_text().splitlines():
            address, raw, text = line.split(None, 2)
            address, raw = int(address, 16), bytes.fromhex(raw)
            require(raw == check_word(address), 'Retained instruction differs from live bytes')
            try:
                listed = decode(address, struct.unpack('<I', raw)[0])
            except ValueError:
                require(not decoded, 'Decoder refuses a decoded listing at %#x' % address)
                listed = text   # a form the decoder refuses (VFP, multiplies)
            require(listed == text, 'Decoder differs from the listing at %#x' % address)
            encoded.extend(raw)
            texts.append(f'0x{address:x}: {text}')
        if not decoded:
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
    # Spelling references against REA's own listings.
    for reference in provenance['spelling_references']:
        address = int(reference['address'], 16)
        listed = next(r['normalized_result'] for r in records if r['evidence_id'] == reference['evidence_id'])['assembly']
        require(f'0x{address:x}: {reference["text"]}' in listed, 'Reference word is not REA-listed as recorded')
        require(check_word(address).hex() == reference['word'], 'Reference word differs from live bytes')
        require(decode(address, struct.unpack('<I', check_word(address))[0]) == reference['text'], 'Decoder differs from REA')
    # Retained data bytes: live bytes and the original read-only segment (no relocation inside).
    rodata_offset = provenance['module']['rodata_file_offset']
    rodata_address = int(provenance['module']['rodata_analysis_address'], 16)
    data = json.loads((evidence / 'data.json').read_text())
    data_bytes = 0
    for address, raw in data.items():
        address, raw = int(address, 16), bytes.fromhex(raw)
        offset = rodata_offset + address - rodata_address
        require(bytes(live[address + i] for i in range(len(raw))) == raw, 'Data differs from live bytes')
        require(cro[offset:offset + len(raw)] == raw, 'Data differs from original CRO')
        require(not any(a <= address + len(raw) - 1 and address < a + 4 for a in relocations), 'Relocation inside data')
        data_bytes += len(raw)
    return total, 4 * len(literals), len(relocations), data_bytes


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
    print(f'Verified {instructions} instruction bytes, {literals} literal bytes and {data} data bytes against live '
          f'REA and DllBattle.cro ({relocations} relocations, spelling references checked against REA)')


if __name__ == '__main__':
    main()
