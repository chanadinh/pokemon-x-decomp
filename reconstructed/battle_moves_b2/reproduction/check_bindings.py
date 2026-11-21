#!/usr/bin/env python3
"""Check which original moves reach the 17 handlers of event 0xac in battle_moves_b2.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that the 17 handlers are bound to
exactly the expected (move, event) pairs (on any event). The other move entries
of event 0xac are counted as not covered by this milestone. No original data is
retained; the output is a summary.
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

EVENTS = (0xac,)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108b0f9c: {('move', 100, 0xac)},                         # Teleport
    0x108b4524: {('move', 607, 0xac)},                         # Hold Hands
    0x108b29a8: {('move', 273, 0xac)},                         # Wish
    0x108b5948: {('move', 588, 0xac)},                         # King's Shield
    0x108b5168: {('move', 288, 0xac)},                         # Grudge
    0x108b987c: {('move', 266, 0xac), ('move', 476, 0xac)},    # Follow Me, Rage Powder
    0x108b2dc4: {('move', 511, 0xac)},                         # Quash
    0x108b26c4: {('move', 277, 0xac)},                         # Magic Coat
    0x108afa98: {('move', 287, 0xac)},                         # Refresh
    0x108b3974: {('move', 571, 0xac)},                         # Forest's Curse
    0x108ae854: {('move', 144, 0xac)},                         # Transform
    0x108b359c: {('move', 393, 0xac)},                         # Magnet Rise
    0x108b11e0: {('move', 289, 0xac)},                         # Snatch
    0x108adb64: {('move', 286, 0xac)},                         # Imprison
    0x108af690: {('move', 259, 0xac)},                         # Torment
    0x108b3be8: {('move', 487, 0xac)},                         # Soak
    0x108afcec: {('move', 160, 0xac)},                         # Conversion
}


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
                        if handler not in found:
                            entries['other handlers'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
                        'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'all_match': True}


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
