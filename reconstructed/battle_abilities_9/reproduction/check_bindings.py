#!/usr/bin/env python3
"""Check which original abilities reach the hook handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that the other ability entries of events 0x03, 0x04,
0x05, 0xb7, 0xaf and 0x5f are exactly the deferred Scrappy and Stance Change
handlers. No original data is retained; the output is a summary.
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

EVENTS = (0x3, 0x4, 0x5, 0xb7, 0xaf, 0x5f)
# handler -> {(table, id, event)}
MB = (104, 163, 164)   # Mold Breaker, Turboblaze, Teravolt
EXPECTED = {
    0x108c3cc4: {('ability', 6, 0x3)},      # Damp
    0x1084aee8: {('ability', 6, 0x4)},      # Damp
    0x108c1a3c: {('ability', 6, 0x73)},     # Damp
    0x108c21dc: {('ability', 151, 0x3)},    # Infiltrator
    0x108c12a8: {('ability', 151, 0x4)},    # Infiltrator
    0x108c5640: {('ability', 151, 0x5)},    # Infiltrator
    0x108c3200: {('ability', a, 0x3) for a in MB},
    0x108c1fbc: {('ability', a, 0x4) for a in MB},
    0x108c2aac: {('ability', a, 0x73) for a in MB},
    0x108c1ff8: {('ability', 113, 0xb7)},   # Scrappy
    0x108c1ebc: {('ability', 64, 0xaf)},    # Liquid Ooze
    0x108c2f4c: {('ability', 103, 0x1f)},   # Klutz
    0x108c26a0: {('ability', 103, 0x21)},   # Klutz
    0x108c36c8: {('ability', 103, 0x73)},   # Klutz
    0x108c3724: {('ability', 103, 0x93)},   # Klutz
    0x108c4c80: {('ability', 103, 0x5f), ('ability', 103, 0x61)},  # Klutz
    0x108c49c8: {('ability', 127, 0x5e)},   # Unnerve
    0x108c5324: {('ability', 127, 0x61), ('ability', 127, 0x5f)},  # Unnerve
    0x108c503c: {('ability', 112, 0x61)},   # Slow Start
    0x108c44ac: {('ability', 112, 0x40)},   # Slow Start
}
# Ability entries of events 0x03-0x05, 0xb7, 0xaf and 0x5f not reconstructed yet: Scrappy (0x04) and Stance Change (0x03).
DEFERRED = {0x108c2ae8, 0x108c0e14}


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
                            entries['deferred'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': sum(1 for v in EXPECTED.values() if v), 'helpers_unbound': sum(1 for v in EXPECTED.values() if not v),
                        'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'deferred_handlers': sorted(hex(h) for h in DEFERRED), 'all_match': True}


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
