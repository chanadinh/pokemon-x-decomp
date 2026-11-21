#!/usr/bin/env python3
"""Check which original moves reach the move handlers of events 0x2a-0x53.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of events 0x2a, 0x2b, 0x35, 0x36, 0x39, 0x3a, 0x42, 0x48 and 0x53 is a handler of this milestone. Entries
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

EVENTS = (0x2a, 0x2b, 0x35, 0x36, 0x39, 0x3a, 0x42, 0x48, 0x53)
# handler -> {(move, event)}
BINDINGS = {
    0x1084b458: {(68, 0x2a)},                                    # Counter
    0x108b8404: {(243, 0x2a)},                                   # Mirror Coat
    0x108b8f78: {(368, 0x2a)},                                   # Metal Burst
    0x108b5848: {(117, 0x2a)},                                   # Bide
    0x108b7a20: {(507, 0x2b)},                                   # Sky Drop
    0x108bb270: {(266, 0x2b), (476, 0x2b)},                      # Follow Me, Rage Powder
    0x108b6ff8: {(217, 0x35)},                                   # Present
    0x108b51d0: {(217, 0x36)},                                   # Present
    0x108b8d3c: {(87, 0x39), (542, 0x39)},                       # Thunder, Hurricane
    0x108b56f0: {(251, 0x3a)},                                   # Beat Up
    0x108ba7f8: {(167, 0x3a)},                                   # Triple Kick
    0x108bbdf4: {(215, 0x42), (312, 0x42)},                      # Heal Bell, Aromatherapy
    0x108ba004: {(518, 0x48), (519, 0x48), (520, 0x48)},         # Grass, Fire, Water Pledge
    0x108b1404: {(499, 0x53)},                                   # Clear Smog
    0x108b4db8: {(99, 0x53)},                                    # Rage
    0x108b57f8: {(117, 0x53)},                                   # Bide
    0x108b69f4: {(481, 0x53)},                                   # Flame Burst
    0x108b847c: {(288, 0x53)},                                   # Grudge
    0x108ba3cc: {(374, 0x53)},                                   # Fling
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
