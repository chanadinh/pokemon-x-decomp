#!/usr/bin/env python3
"""Check which original moves reach Sky Drop and the lock-release move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of events 0x21, 0x23, 0x26, 0x91 and 0xb6 is a handler of this milestone, and Sky Drop's
handlers of events 0xa0 and 0xa3 are bound to exactly move 507. Entries of the other tables are counted.
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

EVENTS = (0x21, 0x23, 0x26, 0x91, 0xb6)
# handler -> {(move, event)}
BINDINGS = {
    0x108b95a0: {(507, 0xa0)},                                   # Sky Drop (charging turn; the rest of 0xa0 is battle_moves_6)
    0x108ba8b4: {(507, 0xa3)},                                   # Sky Drop (release turn; the rest of 0xa3 is battle_moves_6)
    0x108b9558: {(507, 0x26)},                                   # Sky Drop (missed)
    0x108ba140: {(507, 0x91), (507, 0xb6)},                      # Sky Drop (interrupted)
    0x108b3618: {(117, 0x21)},                                   # Bide
    0x108b7610: {(m, 0x21) for m in (182, 197, 203, 561, 588, 596)},  # Protect, Detect, Endure, Mat Block, King's Shield, Spiky Shield
    0x108b8df8: {(205, 0x21), (205, 0x26), (301, 0x21), (301, 0x26)},  # Rollout, Ice Ball
    0x108b9138: {(26, 0x26), (136, 0x26)},                       # Jump Kick, High Jump Kick
    0x108b7adc: {(215, 0x23)},                                   # Heal Bell
    0x108b80d8: {(312, 0x23)},                                   # Aromatherapy
    0x108b8110: {(518, 0x23), (519, 0x23), (520, 0x23)},         # Grass, Fire, Water Pledge
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
