#!/usr/bin/env python3
"""Check which original moves reach the move handlers of event 0x1f (the check before a move).

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of event 0x1f is a handler of this milestone. Entries of the other tables are counted.
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

EVENTS = (0x1f,)
# handler -> {(move, event)}
BINDINGS = {
    0x1084b2c4: {(68, 0x1f)},                                          # Counter
    0x108b9af4: {(243, 0x1f)},                                         # Mirror Coat
    0x108ba3b8: {(368, 0x1f)},                                         # Metal Burst
    0x108b10e8: {(387, 0x1f)},                                         # Last Resort
    0x108b73c8: {(117, 0x1f)},                                         # Bide
    0x108b7d30: {(m, 0x1f) for m in (182, 197, 203, 588, 596)},        # Protect, Detect, Endure, King's Shield, Spiky Shield
    0x108b7f2c: {(156, 0x1f)},                                         # Rest
    0x108b8c38: {(255, 0x1f), (256, 0x1f)},                            # Spit Up, Swallow
    0x108b8cc4: {(173, 0x1f), (214, 0x1f)},                            # Snore, Sleep Talk
    0x108b9080: {(100, 0x1f)},                                         # Teleport
    0x108b94ec: {(507, 0x1f)},                                         # Sky Drop
    0x108b98e8: {(277, 0x1f)},                                         # Magic Coat
    0x108b9cdc: {(254, 0x1f)},                                         # Stockpile
    0x108b9e78: {(469, 0x1f), (501, 0x1f), (578, 0x1f)},               # Wide Guard, Quick Guard, Crafty Shield
    0x108ba4a0: {(374, 0x1f)},                                         # Fling
    0x108ba850: {(393, 0x1f)},                                         # Magnet Rise
    0x108bb3cc: {(561, 0x1f)},                                         # Mat Block
    0x108bb76c: {(363, 0x1f)},                                         # Natural Gift
    0x108bbb4c: {(266, 0x1f), (476, 0x1f)},                            # Follow Me, Rage Powder
    0x108b3c6c: {(252, 0x1f)},                                         # Fake Out
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
