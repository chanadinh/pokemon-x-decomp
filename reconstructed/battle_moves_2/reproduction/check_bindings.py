#!/usr/bin/env python3
"""Check which original moves reach the damage-calculation move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of events 0x3c-0x40 is a handler of this milestone or of battle_moves_1, so
those events are complete for moves. Entries of the other tables are counted.
No original data is retained; the output is a summary.
"""
import argparse
import collections
import importlib.util
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
import check_tables  # noqa: E402

_spec = importlib.util.spec_from_file_location('moves_1_bindings', HERE.parents[2] / 'battle_moves_1/reproduction/check_bindings.py')
moves_1 = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(moves_1)

EVENTS = (0x3c, 0x3d, 0x3e, 0x3f, 0x40)
# handler -> {(move, event)}
BINDINGS = {
    0x108adae8: {(284, 0x3c), (323, 0x3c)},               # Eruption, Water Spout
    0x108afc9c: {(362, 0x3d)},                            # Brine
    0x108b2200: {(268, 0x3d)},                            # Charge
    0x108b2248: {(514, 0x3d)},                            # Retaliate
    0x108b3548: {(474, 0x3d)},                            # Venoshock
    0x108b682c: {(558, 0x3d), (559, 0x3d)},               # Fusion Flare, Fusion Bolt
    0x108b7850: {(382, 0x3d)},                            # Me First
    0x108b8048: {(76, 0x3d)},                             # Solar Beam
    0x108b86e4: {(270, 0x3d)},                            # Helping Hand
    0x108b8d7c: {(263, 0x3d)},                            # Facade
    0x108ba1dc: {(282, 0x3d)},                            # Knock Off
    0x108af710: {(492, 0x3e)},                            # Foul Play
    0x108b420c: {(473, 0x3f), (540, 0x3f), (548, 0x3f)},  # Psyshock, Psystrike, Secret Sword
    0x108b9b44: {(498, 0x3f), (533, 0x3f)},               # Chip Away, Sacred Sword
    0x108b8340: {(263, 0x40)},                            # Facade
}
EXPECTED = {h: {('move', m, e) for m, e in pairs} for h, pairs in BINDINGS.items()}
EARLIER = set(moves_1.EXPECTED)


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
                        check_tables.require(handler in found or handler in EARLIER,
                                             'Uncovered entry: move %d event %#x %#x' % (ident, event, handler))
                        if handler in EARLIER:
                            entries['battle_moves_1'] += 1
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
