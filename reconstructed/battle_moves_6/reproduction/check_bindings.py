#!/usr/bin/env python3
"""Check which original moves reach the two-turn move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of events 0xa0, 0xa1 and 0xa3 is a handler of this milestone or one of
Sky Drop's two deferred handlers. Entries of the other tables are counted.
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

EVENTS = (0xa0, 0xa1, 0xa3)
# handler -> {(move, event)}
BINDINGS = {
    0x108baf70: {(19, 0xa0)},                  # Fly
    0x108baffc: {(340, 0xa0)},                 # Bounce
    0x108b8174: {(291, 0xa0)},                 # Dive
    0x108b9f10: {(91, 0xa0)},                  # Dig
    0x108baee4: {(467, 0xa0), (566, 0xa0)},    # Shadow Force, Phantom Force
    0x108ba2a0: {(13, 0xa0)},                  # Razor Wind
    0x108b8bd4: {(143, 0xa0)},                 # Sky Attack
    0x108ba794: {(76, 0xa0)},                  # Solar Beam
    0x108ba948: {(553, 0xa0), (554, 0xa0)},    # Freeze Shock, Ice Burn
    0x108ba9cc: {(601, 0xa0)},                 # Geomancy
    0x108bb5e8: {(130, 0xa1)},                 # Skull Bash
    0x108bb7d8: {(19, 0xa3)},                  # Fly
    0x108bb8a8: {(340, 0xa3)},                 # Bounce
    0x108b94e8: {(291, 0xa3)},                 # Dive
    0x108bb0c4: {(91, 0xa3)},                  # Dig
    0x108bb768: {(467, 0xa3), (566, 0xa3)},    # Shadow Force, Phantom Force
}
EXPECTED = {h: {('move', m, e) for m, e in pairs} for h, pairs in BINDINGS.items()}
# Sky Drop's handlers of these events (it also has handlers of events 0x1f, 0x26, 0x2e, 0x91 and 0xb6): deferred.
DEFERRED = {0x108b95a0: {('move', 507, 0xa0)}, 0x108ba8b4: {('move', 507, 0xa3)}}


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
            'move_entries_complete_but_deferred': True, 'all_match': True}


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
