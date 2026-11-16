#!/usr/bin/env python3
"""Check which original moves reach the move handlers of events 0x01-0x0e.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of events 0x01, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0d and 0x0e is a handler of this milestone. Entries
of the other tables are counted.
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

EVENTS = (0x01, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0d, 0x0e)
# handler -> {(move, event)}
BINDINGS = {
    0x108b6d8c: {(99, 0x01)},                                    # Rage
    0x108b9ab8: {(194, 0x01), (288, 0x01)},                      # Destiny Bond, Grudge
    0x108b7b50: {(268, 0x03)},                                   # Charge
    0x108b7db8: {(m, 0x03) for m in (182, 197, 203, 561, 588, 596)},  # Protect, Detect, Endure, Mat Block, King's Shield, Spiky Shield
    0x108b6e48: {(268, 0x04)},                                   # Charge
    0x108b7058: {(253, 0x04)},                                   # Uproar
    0x108b7240: {(37, 0x04), (80, 0x04), (200, 0x04)},           # Thrash, Petal Dance, Outrage
    0x108b7c58: {(205, 0x04), (301, 0x04)},                      # Rollout, Ice Ball
    0x108b27d4: {(248, 0x06)},                                   # Future Sight
    0x108b6ca4: {(353, 0x06)},                                   # Doom Desire
    0x108b8418: {(248, 0x07)},                                   # Future Sight
    0x108baabc: {(353, 0x07)},                                   # Doom Desire
    0x108b6494: {(289, 0x08)},                                   # Snatch
    0x108b8ec0: {(277, 0x09)},                                   # Magic Coat
    0x108b7958: {(100, 0x0d)},                                   # Teleport
    0x108ba778: {(253, 0x0e)},                                   # Uproar
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
