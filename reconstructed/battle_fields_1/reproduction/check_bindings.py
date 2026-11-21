#!/usr/bin/env python3
"""Check which original side-effect rows reach the battle_fields_1 handlers.

Decodes the 17 rows of the side-effect table of DllBattle.cro (108d4fe8, 12
bytes each: id, table function, maximum layers) with the battle_handler
milestone's table-function reader, and confirms that every row's handlers are
exactly the reconstructed ones, bound to the expected events, that the
handlers of no other row exist, that the four helpers are bound to nothing,
that the Stealth Rock jump table has the expected 14 targets, and that every
handler of the table is listed in this milestone's evidence/functions.json.
No original data is retained; the output is a summary.
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

TABLE, ROWS, STRIDE = 0x108d4fe8, 17, 12
# Row id -> (maximum layers, {event: handler}); the names are analyst interpretations.
EXPECTED = {
    0: (1, {(0x4f,): 0x108ac688}),                                     # Reflect
    1: (1, {(0x4f,): 0x108aca28}),                                     # Light Screen
    2: (1, {(0x6e,): 0x108acc94, (0x70,): 0x108acc24}),                # Safeguard
    3: (1, {(0x64,): 0x108acbc8, (0x65,): 0x108acb2c}),                # Mist
    4: (1, {(0x13,): 0x108ac16c}),                                     # Tailwind
    5: (1, {(0x3b,): 0x108ac568}),                                     # Lucky Chant
    6: (3, {(0x5e,): 0x108ac460}),                                     # Spikes
    7: (2, {(0x5e,): 0x108ac300}),                                     # Toxic Spikes
    8: (1, {(0x5e,): 0x108ac8fc}),                                     # Stealth Rock
    9: (1, {(0x2e,): 0x108ac704}),                                     # Wide Guard
    10: (1, {(0x2e, 0xb2): 0x108ac5e4}),                               # Quick Guard
    11: (1, {(0x6d, 0x59): 0x108ac02c, (0x75,): 0x108ac5b0}),          # Rainbow
    12: (1, {(0x7f,): 0x108ac1c8}),                                    # Sea of Fire
    13: (1, {(0x13,): 0x108ac0d4}),                                    # Swamp
    14: (1, {(0x5e,): 0x108ac850}),                                    # Sticky Web
    15: (3, {(0x2f, 0xb2): 0x108acaa4}),                               # Mat Block
    16: (1, {(0x2e,): 0x108ac7a4}),                                    # Crafty Shield
}
HELPERS = (0x1084bfc0, 0x1084bf04, 0x1084bf14, 0x108802a8)
JUMP_TABLE = (0x108ac950, 14)
JUMP_TARGETS = [0x108ac988, 0x108ac9b0, 0x108ac9b0, 0x108ac9a8, 0x108ac9a0, 0x108ac998, 0x108ac990, 0x108ac988,
                0x108ac9b8, 0x108ac9c0, 0x108ac9c8, 0x108ac9c8, 0x108ac9c8, 0x108ac9c8]


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found, maxima, entries = {}, {}, collections.Counter()
    for i in range(ROWS):
        at = TABLE + STRIDE * i
        ident = module.byte(at)
        check_tables.require(ident == i, 'Row %d has id %d' % (i, ident))
        word, table = check_tables.decode_function(module, module.relocated[at + 4])
        maxima[i] = module.word(at + 8)
        row = {}
        for k in range(word & 0xffff):
            event, handler = module.byte(table + 8 * k), module.relocated[table + 8 * k + 4]
            row.setdefault(handler, []).append(event)
            entries['event 0x%02x' % event] += 1
        found[i] = row
    for i, (maximum, expected) in EXPECTED.items():
        check_tables.require(maxima[i] == maximum, 'Row %d maximum layers %d' % (i, maxima[i]))
        want = {handler: sorted(events) for events, handler in expected.items()}
        got = {handler: sorted(events) for handler, events in found[i].items()}
        check_tables.require(got == want, 'Row %d handlers differ: %s' % (i, {hex(h): e for h, e in got.items()}))
    reconstructed = {int(r['address'], 16) for r in json.loads((HERE.parents[1] / 'evidence/functions.json').read_text())}
    handlers = {h for _, e in EXPECTED.values() for h in e.values()}
    check_tables.require(handlers <= reconstructed, 'A side-effect handler is not in functions.json')
    check_tables.require(set(HELPERS) <= reconstructed, 'A helper is not in functions.json')
    # The helpers are bound to nothing in any of the five tables read here.
    bound = set(handlers)
    for name, (base, count, key) in check_tables.TABLES.items():
        size = {'B': 1, 'H': 2, 'I': 4}[key]
        for i in range(count):
            at = base + 8 * i
            if at + 4 not in module.relocated:
                continue
            word, table = check_tables.decode_function(module, module.relocated[at + 4])
            for k in range(word & 0xffff):
                bound.add(module.relocated[table + 8 * k + 4])
    check_tables.require(not (set(HELPERS) & bound), 'A helper is bound in a handler table')
    # The Stealth Rock jump table.
    start, count = JUMP_TABLE
    targets = [module.relocated[start + 4 * k] for k in range(count)]
    check_tables.require(targets == JUMP_TARGETS, 'Jump table differs: %s' % [hex(t) for t in targets])
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'rows': ROWS,
            'handlers': len(handlers), 'bindings': sum(len(e) for _, ex in EXPECTED.values() for e in ex),
            'unbound_helpers': ['%08x' % h for h in HELPERS], 'maximum_layers': {str(i): m for i, m in sorted(maxima.items()) if m != 1},
            'entries': dict(sorted(entries.items())), 'jump_table_targets': len(targets), 'all_match': True}


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
