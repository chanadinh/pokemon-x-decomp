#!/usr/bin/env python3
"""Verify the core battle-state helper evidence (batch 4) (battle_core_4) against live REA observations and DllBattle.cro.

Checks the original module hash, that every retained instruction and literal
word equals the live REA bytes, and that each of those words equals the
original CRO word at its file offset, except recorded ABS32 relocations, whose
original word must match and whose value must equal symbol plus addend. It also
checks the instruction text and pseudocode against the REA observation.
Functions without a Ghidra function (decoded_by) are listed by the battle_power
milestone's decode_arm.py: their listing must equal the decoder's output, and
the decoder must reproduce every REA-listed instruction it supports. Two further
rules spell the forms it refuses: decode_bx (bx<cond> Rm) and decode_wb (str or
strb with a pre-indexed, written-back immediate offset). Every instruction
shape of a decoded handler (registers, immediates, literal addresses and branch
targets abstracted; branches and calls excepted) must be listed by REA in an
REA-listed function of the same evidence or at a recorded spelling reference,
and each reference must be REA-listed and reproduced by the decoder from the
live word. Relocated words kept as data (a jump table) must be ABS32
relocations whose values point into their own handler.
"""
import argparse
import hashlib
import re
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


CONDITIONS = ['eq', 'ne', 'cs', 'cc', 'mi', 'pl', 'vs', 'vc', 'hi', 'ls', 'ge', 'lt', 'gt', 'le', '']


def decode_bx(word):
    """bx<cond> Rm, as the REA listing spells it (decode_arm refuses bx)."""
    if word & 0x0ffffff0 != 0x012fff10 or word >> 28 == 0xf:
        raise ValueError('not bx')
    reg = word & 0xf
    return 'bx%s %s' % (CONDITIONS[word >> 28], {13: 'sp', 14: 'lr', 15: 'pc'}.get(reg, 'r%d' % reg))


def decode_wb(word):
    """str/strb Rd, [Rn,#+-imm]! (pre-indexed with write-back), as the REA listing spells it."""
    if (word >> 26) & 3 != 1 or (word >> 25) & 1 or not (word >> 24) & 1 or not (word >> 21) & 1 or (word >> 20) & 1:
        raise ValueError('not a pre-indexed store')
    names = {13: 'sp', 14: 'lr', 15: 'pc'}
    reg = lambda r: names.get(r, 'r%d' % r)
    sign = '' if (word >> 23) & 1 else '-'
    return '%s%s %s, [%s,#%s0x%x]!' % ('strb' if (word >> 22) & 1 else 'str', CONDITIONS[word >> 28], reg((word >> 12) & 0xf),
                                       reg((word >> 16) & 0xf), sign, word & 0xfff)


def listing(address, word):
    """decode_arm text, or decode_bx for the bx form, or decode_wb for the pre-indexed store."""
    try:
        return decode(address, word)
    except ValueError:
        pass
    try:
        return decode_bx(word)
    except ValueError:
        return decode_wb(word)


def shape(text):
    """An instruction shape: immediates, literal addresses, branch targets and register numbers abstracted."""
    text = re.sub(r'#0x[0-9a-f]+', '#imm', text)
    text = re.sub(r'\[0x[0-9a-f]+\]', '[literal]', text)
    text = re.sub(r'^(b[a-z]*) 0x[0-9a-f]+$', r'\1 target', text)
    return re.sub(r'\br(1[0-2]|[0-9])\b', 'rN', text)


def normalize(entry, texts):
    """Branch and literal addresses relative to the entry; calls (absolute) are kept."""
    return [re.sub(r'0x([0-9a-f]+)', lambda m: '+%#x' % (int(m.group(1), 16) - entry), t)
            if re.match(r'(b(ne|eq) |ldr r1, \[0x)', t) else t for t in texts]


def asm_lines(evidence, row):
    """A row's retained listing: its own file, or its address range of a shared listing file."""
    if 'listing' not in row:
        return (evidence / (row['address'] + '.asm')).read_text().splitlines()
    low = int(row['address'], 16)
    return [line for line in (evidence / row['listing']).read_text().splitlines()
            if low <= int(line.split(None, 1)[0], 16) < low + row['body_bytes']]


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
    rows = json.loads((evidence / 'functions.json').read_text())
    shapes = {}
    for row in rows:
        decoded = 'decoded_by' in row
        original = None if decoded else next(r['normalized_result'] for r in records if r['evidence_id'] == row['evidence_id'])
        encoded, texts = bytearray(), []
        data = row.get('data_words')
        data_start = int(data['start'], 16) if data else 0
        data_end = data_start + 4 * data['count'] if data else 0
        entry = int(row['address'], 16)
        for line in asm_lines(evidence, row):
            address, raw, text = line.split(None, 2)
            address, raw = int(address, 16), bytes.fromhex(raw)
            require(raw == check_word(address), 'Retained instruction differs from live bytes')
            if data_start <= address < data_end:
                # A relocated code pointer kept as data, aimed into its own handler.
                value = struct.unpack('<I', raw)[0]
                require(address in relocations and entry <= value < entry + row['body_bytes'], 'Data word is not a pointer into its handler')
                require(text == '.word 0x%08x' % value, 'Data word text differs from its value')
                encoded.extend(raw)
                texts.append(f'0x{address:x}: {text}')
                continue
            try:
                listed = listing(address, struct.unpack('<I', raw)[0])
            except ValueError:
                require(not decoded, 'Decoder refuses a decoded listing at %#x' % address)
                listed = text   # a form the decoder refuses (VFP, multiplies)
            require(listed == text, 'Decoder differs from the listing at %#x' % address)
            encoded.extend(raw)
            texts.append(f'0x{address:x}: {text}')
        shapes[row['address']] = normalize(int(row['address'], 16), [t.split(': ', 1)[1] for t in texts])
        if not decoded:
            require(texts == original['assembly'], 'Instruction text differs from REA observation')
            require((evidence / (row['address'] + '.c')).read_text().split('\n', 1)[1] == original['pseudocode'],
                    'Pseudocode differs from REA observation')
        checked = next(v for v in provenance['verified_functions'] if v['address'] == row['address'])
        require(len(encoded) == row['body_bytes'] == checked['instruction_bytes'] and
                hashlib.sha256(encoded).hexdigest() == checked['instruction_bytes_sha256'], 'Instruction digest mismatch')
        total += len(encoded)
    for row in rows:
        if 'same_shape_as' not in row:
            continue
        sibling = next(r for r in rows if r['address'] == row['same_shape_as'])
        require('decoded_by' not in sibling, 'Shape sibling is not REA-listed')
        mine, theirs = shapes[row['address']], shapes[sibling['address']]
        differ = [(a, b) for a, b in zip(mine, theirs) if a != b]
        require(len(mine) == len(theirs) and len(differ) <= 1 and
                all(re.fullmatch(r'(cmp r0|mov r12), #0x[0-9a-f]+', a) and a.split('#')[0] == b.split('#')[0]
                    for a, b in differ), 'Decoded listing differs from its sibling at %s' % row['address'])
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
        require(listing(address, struct.unpack('<I', check_word(address))[0]) == reference['text'], 'Decoder differs from REA')
    # Every instruction shape of a decoded handler is REA-listed in this evidence or pinned by a reference.
    covered = {shape(reference['text']) for reference in provenance['spelling_references']}
    for row in rows:
        if 'decoded_by' not in row:
            for line in asm_lines(evidence, row):
                covered.add(shape(line.split(None, 2)[2]))
    for row in rows:
        if 'decoded_by' not in row:
            continue
        for line in asm_lines(evidence, row):
            text = line.split(None, 2)[2]
            if text.startswith(('.word', 'b ', 'bl ', 'bne ', 'beq ')):
                continue
            require(shape(text) in covered, 'Shape of %r at %s is neither REA-listed here nor pinned by a reference' % (text, row['address']))
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
          f'REA and DllBattle.cro ({relocations} relocations, decoded shapes REA-listed or referenced)')


if __name__ == '__main__':
    main()
