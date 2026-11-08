#!/usr/bin/env python3
"""Check which original abilities reach the move-calculation handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that every other
ability entry of events 0x39 (accuracy), 0x38 (stages), 0x3b (critical
hits) and 0x84 (weight) is Sand Veil or Snow Cloak, reconstructed in
battle_abilities_2. No original data is retained; the output is a summary.
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

EVENTS = (0x39, 0x38, 0x3b, 0x84)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108bd7cc: {('ability', 14, 0x39)},    # Compound Eyes
    0x108be2c0: {('ability', 162, 0x39)},   # Victory Star
    0x108c3ab8: {('ability', 55, 0x39)},    # Hustle
    0x108bf910: {('ability', 77, 0x39)},    # Tangled Feet
    0x108c0764: {('ability', 147, 0x39)},   # Wonder Skin
    0x108c3d64: {('ability', 51, 0x38)},    # Keen Eye
    0x108c23f4: {('ability', 109, 0x38)},   # Unaware
    0x108c239c: {('ability', 109, 0x3e)},   # Unaware
    0x108c23c8: {('ability', 109, 0x3f)},   # Unaware
    0x108bf554: {('ability', 113, 0x44)},   # Scrappy
    0x108bd17c: {('ability', 105, 0x3b)},   # Super Luck
    0x108c0700: {('ability', 4, 0x3b), ('ability', 75, 0x3b)},  # Battle Armor, Shell Armor
    0x108bd6b4: {('ability', 100, 0xf)},    # Stall
    0x108bd8a8: {('ability', 95, 0x13)},    # Quick Feet
    0x108c31ac: {('ability', 84, 0x13)},    # Unburden
    0x108c3cd0: {('ability', 112, 0x13)},   # Slow Start
    0x108c551c: {('ability', 84, 0xa8)},    # Unburden
    0x108bfde4: {('ability', 134, 0x84)},   # Heavy Metal
    0x108bfed8: {('ability', 135, 0x84)},   # Light Metal
    0x108bff04: {('ability', 98, 0x89)},    # Magic Guard
    0x108be4ec: {('ability', 69, 0x58)},    # Rock Head
    0x108bf8cc: {('ability', 92, 0x3a)},    # Skill Link
    0x108bddcc: {('ability', 185, 0x3a)},   # Parental Bond
    0x108c0a74: {('ability', 91, 0x49)},    # Adaptability
    0x108c09f8: {('ability', 39, 0x76)},    # Inner Focus
    0x108c0ab0: {('ability', 32, 0x6d), ('ability', 32, 0x59), ('ability', 32, 0xb5)},  # Serene Grace
    0x108c46dc: {('ability', 32, 0x75)},    # Serene Grace
}
# The other ability entries of these events, reconstructed in battle_abilities_2: Sand Veil and Snow Cloak (event 0x39).
DEFERRED = {0x108c01d8, 0x108c02f4}


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
                    if name == 'ability':
                        check_tables.require(handler in found or handler in DEFERRED,
                                             'Uncovered entry: %s %d event %#x %#x' % (name, ident, event, handler))
                        if handler in DEFERRED:
                            entries['battle_abilities_2'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
                        'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'other_milestone_handlers': sorted(hex(h) for h in DEFERRED), 'all_match': True}


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
