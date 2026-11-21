#!/usr/bin/env python3
"""Check the battle_tables_1 table functions and registrars against the original handler tables.

Reads the five non-move handler tables of DllBattle.cro (ability 108d510c,
item 108d382c, field 108d6810, side-effect 108d4fe8 and position-effect
108d6768) with the battle_handler milestone's table-function reader and
confirms that:

- every row's table function is a regular four-instruction stub (a count
  constant or literal, a store, an entries literal, a return), except the one
  irregular terrain function 108bbf98 (position row 10), whose 18 words and
  four parameter-selected entry tables are checked directly, and the one item
  row that holds no function;
- each of the 383 distinct table functions is listed in this milestone's
  evidence/functions.json, so the whole set of stubs is covered;
- every count word has the form (priority << 16 | count) with a count from 1
  to 255 and a priority below 8, and the evidence holds exactly the table
  functions besides the registrars and helpers;
- the registrars' loop bounds, table literals and record layout match the
  constants of the reconstruction (read from their retained listings);
- no move-table function is claimed: the move table is outside this milestone.
No original data is retained; the output is a summary.
"""
import argparse
import collections
import json
import re
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
import check_tables  # noqa: E402

TABLES = {'ability': (0x108d510c, 0xb6, 8), 'item': (0x108d382c, 0xb6, 8), 'field': (0x108d6810, 5, 8),
          'side': (0x108d4fe8, 17, 12), 'position': (0x108d6768, 11, 8)}
TERRAIN = 0x108bbf98
TERRAIN_TABLES = (0x108d67d0, 0x108d67e0, 0x108d67f8, 0x108d380c)
TERRAIN_WORDS = [0xe3510001, 0x0a00000b, 0xe3510002, 0xe3a02003, 0x05802000, 0x059f002c, 0x0a000005, 0xe3510003,
                 0x13a01001, 0x15801000, 0x05802000, 0x159f0018, 0x059f0018, 0xe12fff1e,
                 0xe3a01002, 0xe5801000, 0xe59f000c, 0xe12fff1e]
# Registrar -> (its retained listing must contain each of these instruction texts).
REGISTRARS = {
    0x108acd0c: ['cmp r0, #0x2', 'cmp r4, #0x11', 'cmp r0, #0x11', 'add r1, r6, r6, lsl #0x4', 'add r5, r1, r4, lsl #0x4',
                 'ldr r1, [r5,#0xc]', 'add r0, r0, r0, lsl #0x1', 'mov r2, #0x2', 'mov r1, #0x6'],
    0x108bc690: ['cmp r0, #0xb', 'ldrb r2, [r1,r0,lsl #0x3]', 'mov r2, #0x3', 'cpy r0, r2'],
    0x108a5bd8: ['mov r1, #0x5', 'cmp r4, #0xb6', 'mov r2, #0x6', 'mov r0, #0x5'],
    0x108abf28: ['cmp r4, #0xb6', 'mov r2, #0x6', 'mov r0, #0x5', 'mov r1, #0x6', 'mov r2, #0x1'],
    0x1087c024: ['ldr r2, [r2,#0x60]', 'ldrb r3, [r2,#0x3b]', 'ldrbeq r3, [r2,#0x10]', 'bic r3, r3, #0x40000000'],
    0x10882674: ['orr r1, r1, #0x4000000'],
}
# Literal word addresses and the (static) addresses they relocate to.
LITERALS = {0x108ace44: 0x10b11c00, 0x108ace48: 0x108d4fe8, 0x108bc70c: 0x108d6768, 0x108a5c98: 0x108d382c,
            0x108abff0: 0x108d382c, 0x1087c070: 0x108fca00}


def instruction_texts(evidence, address):
    path = evidence / ('%08x.asm' % address)
    return {line.split(None, 2)[2] for line in path.read_text().splitlines()}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    evidence = HERE.parents[1] / 'evidence'
    covered = {int(r['address'], 16) for r in json.loads((evidence / 'functions.json').read_text())}
    per_table, distinct, counts, forms = {}, set(), collections.Counter(), collections.Counter()
    unrelocated = []
    for name, (base, rows, stride) in TABLES.items():
        found = 0
        for i in range(rows):
            at = base + stride * i
            if at + 4 not in module.relocated:
                unrelocated.append((name, i))
                continue
            function = module.relocated[at + 4]
            found += 1
            distinct.add(function)
            check_tables.require(function in covered, 'Table function %#x has no evidence' % function)
            if function == TERRAIN:
                continue
            word, _ = check_tables.decode_function(module, function)
            check_tables.require(word >> 16 < 8 and (word & 0xffff) < 256 and (word & 0xffff) > 0,
                                 'Count word %#x of %#x is out of range' % (word, function))
            counts['count %d' % (word & 0xffff)] += 1
            forms['priority %d' % (word >> 16)] += 1
        per_table[name] = found
    check_tables.require(unrelocated == [('item', 181)], 'Rows without a function: %s' % unrelocated)
    check_tables.require(len(distinct) == 383 and TERRAIN in distinct, 'Expected 383 distinct table functions, found %d' % len(distinct))
    stub_rows = {a for a in covered if a not in REGISTRARS}
    check_tables.require(stub_rows == distinct, 'The evidence stubs are not the table functions')
    # The irregular terrain function and its entry tables.
    words = [module.word(TERRAIN + 4 * k) for k in range(18)]
    check_tables.require(words == TERRAIN_WORDS, 'Terrain table function differs')
    check_tables.require([module.relocated[a] for a in (0x108bbfe0, 0x108bbfe4, 0x108bbfe8, 0x108bbfec)] ==
                         [0x108d67e0, 0x108d380c, 0x108d67f8, 0x108d67d0], 'Terrain literals differ')
    # Registrars: retained instructions and literal relocations.
    for entry, wanted in REGISTRARS.items():
        texts = instruction_texts(evidence, entry)
        check_tables.require(all(t in texts for t in wanted), 'A registrar instruction is missing from %#x: %s' % (
            entry, [t for t in wanted if t not in texts]))
    for address, target in LITERALS.items():
        check_tables.require(module.relocated[address] == target, 'Literal %#x differs' % address)
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'rows': {n: TABLES[n][1] for n in TABLES},
            'rows_with_function': per_table, 'distinct_table_functions': len(distinct), 'irregular': ['108bbf98'],
            'rows_without_function': ['item row 181'], 'registrars': ['%08x' % a for a in REGISTRARS],
            'count_values': dict(sorted(counts.items())), 'priorities': dict(sorted(forms.items())), 'all_match': True}


def main():
    root = HERE.parents[3]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cro', type=Path, default=root / 'inputs/pokemon-x-modules-a035d9775a1f/modules/DllBattle.cro')
    args = parser.parse_args()
    try:
        record = run(root, args.cro)
    except (OSError, ValueError, KeyError) as exc:
        parser.exit(1, f'error: {exc}\n')
    json.dump(record, sys.stdout, indent=2)
    print()


if __name__ == '__main__':
    main()
