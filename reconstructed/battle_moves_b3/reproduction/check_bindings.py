#!/usr/bin/env python3
"""Check which original moves reach the 12 handlers of event 0xac in battle_moves_b3.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that the 12 handlers are bound to
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
    0x108ae9c8: {('move', 170, 0xac), ('move', 199, 0xac)},  # Mind Reader, Lock-On
    0x108af13c: {('move', 582, 0xac)},                         # Electrify
    0x108af7d8: {('move', 203, 0xac)},                         # Endure
    0x108af9f0: {('move', 278, 0xac)},                         # Recycle
    0x108b0718: {('move', 116, 0xac)},                         # Focus Energy
    0x108b0ba0: {('move', 272, 0xac)},                         # Role Play
    0x108b38a4: {('move', 50, 0xac)},                          # Disable
    0x108b3b18: {('move', 513, 0xac)},                         # Reflect Type
    0x108b402c: {('move', 379, 0xac)},                         # Psych Up
    0x108b5be0: {('move', 361, 0xac)},                         # Healing Wish
    0x108b6f68: {('move', 461, 0xac)},                         # Lunar Dance
    0x108b787c: {('move', 270, 0xac)},                         # Helping Hand
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
