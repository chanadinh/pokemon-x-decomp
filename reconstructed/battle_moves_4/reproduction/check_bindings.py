#!/usr/bin/env python3
"""Check which original moves reach the accuracy, added-effect and cleanup move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of events 0x37, 0x62, 0x7f and 0xa5 is a handler of this milestone. Entries of the other tables are counted.
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

EVENTS = (0x37, 0x62, 0x7f, 0xa5)
# handler -> {(move, event)}
BINDINGS = {
    0x108b0e58: {(327, 0xa5)},                                        # Sky Uppercut
    0x108b97c0: {(87, 0xa5), (479, 0xa5), (542, 0xa5), (614, 0xa5)},  # Thunder, Smack Down, Hurricane, Thousand Arrows
    0x108b9d28: {(16, 0xa5), (239, 0xa5)},                            # Gust, Twister
    0x108b7b14: {(89, 0xa5), (222, 0xa5)},                            # Earthquake, Magnitude
    0x108b8714: {(250, 0xa5)},                                        # Whirlpool
    0x108b9b08: {(57, 0xa5)},                                         # Surf
    0x108b9d64: {(215, 0xa5), (270, 0xa5), (312, 0xa5)},              # Heal Bell, Helping Hand, Aromatherapy
    0x108b9cb0: {(563, 0xa5), (579, 0xa5)},                           # Rototiller, Flower Shield
    0x108ae714: {(59, 0x37)},                                         # Blizzard
    0x108bb520: {(87, 0x37), (542, 0x37)},                            # Thunder, Hurricane
    0x108b76c8: {(228, 0x37)},                                        # Pursuit
    0x108bb1f0: {(m, 0x37) for m in (23, 34, 407, 467, 535, 537, 560, 566)},  # Stomp and the other minimize hitters
    0x108afc4c: {(74, 0x62)},                                         # Growth
    0x108b0e94: {(563, 0x62)},                                        # Rototiller
    0x108b1314: {(599, 0x62)},                                        # Venom Drench
    0x108b3324: {(602, 0x62)},                                        # Magnetic Flux
    0x108b69b8: {(246, 0x62), (318, 0x62), (466, 0x62)},              # AncientPower, Silver Wind, Ominous Wind
    0x108b8dcc: {(203, 0x7f)},                                        # Endure
    0x108b9dbc: {(270, 0x7f)},                                        # Helping Hand
    0x108b9ee4: {(289, 0x7f)},                                        # Snatch
    0x108ba38c: {(277, 0x7f)},                                        # Magic Coat
    0x108b8318: {(588, 0x7f), (596, 0x7f)},                           # King's Shield, Spiky Shield
    0x108bbc64: {(266, 0x7f), (476, 0x7f)},                           # Follow Me, Rage Powder
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
