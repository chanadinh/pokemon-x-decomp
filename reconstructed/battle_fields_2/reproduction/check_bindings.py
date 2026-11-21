#!/usr/bin/env python3
"""Check which original position-effect rows reach the battle_fields_2 handlers.

Decodes the 11 rows of the position-effect table of DllBattle.cro (108d6768,
8 bytes each: id, table function) with the battle_handler milestone's
table-function reader. Ten rows have regular table functions; row 10 (the
terrains) has the irregular table function 108bbf98, whose 14 instruction
words are checked here and whose four parameter-selected entry tables
(108d67d0, 108d67e0, 108d67f8, 108d380c) are read directly. The check confirms
that every row's handlers are exactly the reconstructed ones, bound to the
expected events, that no other row has handlers, and that every handler of the
whole table is listed in this milestone's evidence/functions.json or has its
instruction listing in the evidence folder. No original data is retained.
"""
import argparse
import collections
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
import check_tables  # noqa: E402

TABLE, ROWS, STRIDE = 0x108d6768, 11, 8
# Row index -> (id, {event: handler}); the names are analyst interpretations.
EXPECTED = {
    0: (0, {0x4f: 0x108bc154}),
    1: (1, {0x13: 0x108bc28c}),
    2: (2, {0x39: 0x108bc680, 0x44: 0x108bc658}),   # accuracy, Ground type
    3: (3, {0x00: 0x108bc07c}),
    4: (4, {0x3d: 0x108bc260}),                     # Water Sport
    5: (5, {0x3d: 0x108bc234}),                     # Mud Sport
    6: (6, {0x3f: 0x108bc3a4}),
    7: (7, {0x00: 0x108bc07c}),
    8: (9, {0x00: 0x108bc07c}),
    9: (8, {0x29: 0x108bc298}),                     # Ion Deluge
}
TERRAIN_FUNCTION = 0x108bbf98
TERRAIN_WORDS = [0xe3510001, 0x0a00000b, 0xe3510002, 0xe3a02003, 0x05802000, 0x059f002c, 0x0a000005, 0xe3510003,
                 0x13a01001, 0x15801000, 0x05802000, 0x159f0018, 0x059f0018, 0xe12fff1e]
# Parameter -> (entries table, {event: handler}); the names are analyst interpretations.
TERRAINS = {
    1: (0x108d67d0, {0x3d: 0x108bc158, 0x7f: 0x108bc500}),                 # Grassy
    2: (0x108d67e0, {0x3d: 0x108bc0f4, 0x6e: 0x108bc3c0, 0x70: 0x108bc5b0}),   # Misty
    3: (0x108d67f8, {0x3d: 0x108bc094, 0x6e: 0x108bc2c0, 0x70: 0x108bc458}),   # Electric
    0: (0x108d380c, {0x00: 0x108bc07c}),                                   # any other parameter
}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    entries = collections.Counter()
    ids = []
    for i in range(ROWS):
        at = TABLE + STRIDE * i
        ident = module.byte(at)
        ids.append(ident)
        function = module.relocated[at + 4]
        if i == 10:
            check_tables.require(ident == 10 and function == TERRAIN_FUNCTION, 'Row 10 is not the terrain row')
            words = [module.word(TERRAIN_FUNCTION + 4 * k) for k in range(14)]
            check_tables.require(words == TERRAIN_WORDS, 'Terrain table function differs')
            continue
        want_id, expected = EXPECTED[i]
        check_tables.require(ident == want_id, 'Row %d has id %d' % (i, ident))
        count, table = check_tables.decode_function(module, function)
        got = {}
        for k in range(count & 0xffff):
            got[module.byte(table + 8 * k)] = module.relocated[table + 8 * k + 4]
        check_tables.require(len(got) == (count & 0xffff) and got == expected,
                             'Row %d handlers differ: %s' % (i, {hex(e): hex(h) for e, h in got.items()}))
        for e in got:
            entries['event 0x%02x' % e] += 1
    check_tables.require(ids == [0, 1, 2, 3, 4, 5, 6, 7, 9, 8, 10], 'Row ids %s' % ids)
    # Row 10: the function selects among four tables by its parameter (1, 2, 3, other); the
    # two literals loaded for 2 and 3 and the default and the first one (108bbfec) are checked.
    check_tables.require(module.relocated[0x108bbfe0] == 0x108d67e0 and module.relocated[0x108bbfe4] == 0x108d380c
                         and module.relocated[0x108bbfe8] == 0x108d67f8 and module.relocated[0x108bbfec] == 0x108d67d0,
                         'Terrain literals differ')
    for parameter, (table, expected) in TERRAINS.items():
        got = {module.byte(table + 8 * k): module.relocated[table + 8 * k + 4] for k in range(len(expected))}
        check_tables.require(got == expected, 'Terrain %d handlers differ' % parameter)
        for e in got:
            entries['event 0x%02x' % e] += 1
    handlers = {h for _, ex in EXPECTED.values() for h in ex.values()} | {h for _, ex in TERRAINS.values() for h in ex.values()}
    reconstructed = {int(r['address'], 16) for r in json.loads((HERE.parents[1] / 'evidence/functions.json').read_text())}
    decoded = {int(p.stem, 16) for p in (HERE.parents[1] / 'evidence').glob('*.asm')}
    check_tables.require(handlers <= reconstructed | decoded, 'A position-effect handler has no evidence')
    check_tables.require(len(handlers) == 17, 'Expected 17 distinct handlers, found %d' % len(handlers))
    bindings = sum(len(e) for _, e in EXPECTED.values()) + sum(len(e) for _, e in TERRAINS.values())
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'rows': ROWS, 'row_ids': ids,
            'handlers': len(handlers), 'bindings': bindings, 'terrain_tables': len(TERRAINS),
            'events': dict(sorted(entries.items())), 'all_match': True}


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
