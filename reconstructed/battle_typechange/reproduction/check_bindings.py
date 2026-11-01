#!/usr/bin/env python3
"""Check which original abilities and moves reach the type-change handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected rows, only for event 0x28, and that these are
all of the event's entries in every table. No original data is retained; the
output is a summary.
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

EVENT = 0x28
# handler -> {(table, id)}
EXPECTED = {
    0x108c0114: {('ability', 96)},   # Normalize
    0x108bde48: {('ability', 184)},  # Aerilate
    0x108bf0b0: {('ability', 182)},  # Pixilate
    0x108bf0fc: {('ability', 174)},  # Refrigerate
    0x108b8750: {('move', 311)},     # Weather Ball
    0x108b8008: {('move', 174)},     # Curse
    0x108ba824: {('move', 165)},     # Struggle
    0x108b9a54: {('move', 237)},     # Hidden Power
    0x108b7708: {('move', 449)},     # Judgment
    0x108b9c28: {('move', 363)},     # Natural Gift
    0x108b6398: {('move', 546)},     # Techno Blast
}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    other_events, event_entries = [], collections.Counter()
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
                if event == EVENT:
                    event_entries[name] += 1
                    check_tables.require(handler in found, 'Uncovered event 0x28 entry: %s %d %#x' % (name, ident, handler))
                if handler in found:
                    if event != EVENT:
                        other_events.append((name, ident, event))
                    found[handler].add((name, ident))
    check_tables.require(not other_events, 'Handlers bound to other events: %s' % other_events)
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'bindings': sum(len(v) for v in EXPECTED.values()),
            'event_0x28_entries': dict(sorted(event_entries.items())), 'event_0x28_covered': True, 'all_match': True}


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
