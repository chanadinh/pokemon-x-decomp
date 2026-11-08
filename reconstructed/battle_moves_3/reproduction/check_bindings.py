#!/usr/bin/env python3
"""Check which original moves reach the fixed-damage and type-matchup move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of events 0x38, 0x43-0x46, 0x4e and 0x50 is a handler of this milestone. Entries of the other tables are counted.
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

EVENTS = (0x38, 0x43, 0x44, 0x45, 0x46, 0x4e, 0x50)
# handler -> {(move, event)}
BINDINGS = {
    0x108b2e8c: {(49, 0x4e)},                 # Sonic Boom
    0x108b5414: {(82, 0x4e)},                 # Dragon Rage
    0x108b4578: {(69, 0x4e), (101, 0x4e)},    # Seismic Toss, Night Shade
    0x108b2cfc: {(149, 0x4e)},                # Psywave
    0x108b5b88: {(162, 0x4e)},                # Super Fang
    0x108b1824: {(283, 0x4e)},                # Endeavor
    0x108bab60: {(515, 0x4e)},                # Final Gambit
    0x108b68c4: {(117, 0x4e)},                # Bide
    0x108b94c0: {(68, 0x4e)},                 # Counter
    0x108bacfc: {(243, 0x4e)},                # Mirror Coat
    0x108bb3a4: {(368, 0x4e)},                # Metal Burst
    0x108ba304: {(280, 0x4e)},                # Brick Break
    0x108bb244: {(280, 0x50)},                # Brick Break
    0x108af540: {(86, 0x43)},                 # Thunder Wave
    0x108bb87c: {(614, 0x43)},                # Thousand Arrows
    0x108b020c: {(573, 0x44)},                # Freeze-Dry
    0x108b4cdc: {(560, 0x44)},                # Flying Press
    0x108bbcf8: {(614, 0x45)},                # Thousand Arrows
    0x108bbccc: {(614, 0x46)},                # Thousand Arrows
    0x108ba544: {(498, 0x38), (533, 0x38)},   # Chip Away, Sacred Sword
}
EXPECTED = {h: {('move', m, e) for m, e in pairs} for h, pairs in BINDINGS.items()}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    entries = collections.Counter()
    for name, (base, count, key) in check_tables.TABLES.items():
        size = {'B': 1, 'H': 2, 'I': 4}[key]
        for i in range(count):
            at = base + 8 * i
            if at + 4 not in module.relocated:
                continue
            ident = int.from_bytes(bytes(module.byte(at + k) for k in range(size)), 'little')
            word, table = check_tables.decode_function(module, module.relocated[at + 4])
            for k in range(word & 0xffff):
                event, handler = module.byte(table + 8 * k), module.relocated[table + 8 * k + 4]
                if event in EVENTS:
                    entries['%s 0x%x' % (name, event)] += 1
                    if name == 'move':
                        check_tables.require(handler in found, 'Uncovered entry: move %d event %#x %#x' % (ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'bindings': sum(len(v) for v in EXPECTED.values()), 'events': [hex(e) for e in EVENTS],
            'entries': dict(sorted(entries.items())), 'move_entries_complete': True, 'all_match': True}


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
