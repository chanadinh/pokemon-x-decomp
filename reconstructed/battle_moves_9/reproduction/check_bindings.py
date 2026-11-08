#!/usr/bin/env python3
"""Check which original moves reach the move handlers of event 0x24 and Secret Power.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of event 0x24 is a handler of this milestone; Secret Power's event-0x8c
handler is bound to exactly move 290. Entries of the other tables are counted.
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

EVENTS = (0x24,)
# handler -> {(move, event)}
BINDINGS = {
    0x108adbe8: {(117, 0x24)},                            # Bide
    0x108af3e0: {(37, 0x24), (80, 0x24), (200, 0x24)},    # Thrash, Petal Dance, Outrage
    0x108b7b94: {(205, 0x24), (301, 0x24)},               # Rollout, Ice Ball
    0x108b1f70: {(99, 0x24)},                             # Rage
    0x108b2168: {(268, 0x24)},                            # Charge
    0x108afbcc: {(496, 0x24)},                            # Round
    0x108b0194: {(92, 0x24)},                             # Toxic
    0x108b39e8: {(111, 0x24)},                            # Defense Curl
    0x108b55d4: {(107, 0x24)},                            # Minimize
    0x108b836c: {(222, 0x24)},                            # Magnitude
    0x108b9f9c: {(518, 0x24), (519, 0x24), (520, 0x24)},  # Grass, Fire and Water Pledge
    0x108bab20: {(290, 0x24)},                            # Secret Power
    0x108bb0c8: {(120, 0x24), (153, 0x24)},               # Selfdestruct, Explosion
    0x108b741c: {(290, 0x8c)},                            # Secret Power (event 0x8c; the rest of that event is battle_moves_7)
}
EXPECTED = {h: {('move', m, e) for m, e in pairs} for h, pairs in BINDINGS.items()}
DEFERRED = {}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    deferred = {h: set() for h in DEFERRED}
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
                        check_tables.require(handler in found or handler in DEFERRED,
                                             'Uncovered entry: move %d event %#x %#x' % (ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
                if handler in DEFERRED:
                    deferred[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    for handler, expected in DEFERRED.items():
        check_tables.require(deferred[handler] == expected, 'Deferred bindings of %#x differ' % handler)
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'bindings': sum(len(v) for v in EXPECTED.values()), 'events': [hex(e) for e in EVENTS],
            'entries': dict(sorted(entries.items())), 'deferred_handlers': sorted(hex(h) for h in DEFERRED),
            'move_entries_complete': True, 'all_match': True}


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
