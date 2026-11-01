#!/usr/bin/env python3
"""Check which original abilities, items and moves reach the damage-modifier handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected rows, only for event 0x4f, and that these are
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

EVENT = 0x4f
BERRIES = {184: 0x108a638c, 185: 0x108a6630, 186: 0x108a6cc8, 187: 0x108a6794, 188: 0x108a697c, 189: 0x108a6474,
           190: 0x108a6120, 191: 0x108a68e4, 192: 0x108a6494, 193: 0x108a6454, 194: 0x108a6904, 195: 0x108a6df0,
           196: 0x108a6abc, 197: 0x108a6610, 198: 0x108a62f0, 199: 0x108a6bec, 200: 0x108a6188, 686: 0x108a6c0c}
# handler -> {(table, id)}
EXPECTED = {h: {('item', i)} for i, h in BERRIES.items()}
EXPECTED.update({
    0x108a8250: {('item', 268)},                      # Expert Belt
    0x108aaa10: {('item', 270)},                      # Life Orb
    0x108a6698: {('item', 277)},                      # Metronome
    0x108bd260: {('ability', 97)},                    # Sniper
    0x108be2fc: {('ability', 116), ('ability', 111)}, # Solid Rock, Filter
    0x108bf300: {('ability', 110)},                   # Tinted Lens
    0x108c002c: {('ability', 136)},                   # Multiscale
    0x108c07e8: {('ability', 132)},                   # Friend Guard
    0x108b0b4c: {('move', 57)},                       # Surf
    0x108b4934: {('move', 250)},                      # Whirlpool
    0x108b5c70: {('move', 89), ('move', 222)},        # Earthquake, Magnitude
    0x108ba188: {('move', m) for m in (23, 34, 407, 467, 535, 537, 560, 566)},
})


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
                    check_tables.require(handler in found, 'Uncovered event 0x4f entry: %s %d %#x' % (name, ident, handler))
                if handler in found:
                    if event != EVENT:
                        other_events.append((name, ident, event))
                    found[handler].add((name, ident))
    check_tables.require(not other_events, 'Handlers bound to other events: %s' % other_events)
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'bindings': sum(len(v) for v in EXPECTED.values()),
            'event_0x4f_entries': dict(sorted(event_entries.items())), 'event_0x4f_covered': True, 'all_match': True}


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
