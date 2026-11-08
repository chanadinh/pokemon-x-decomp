#!/usr/bin/env python3
"""Check which original field ids reach the field handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that the five field handlers are bound
to exactly the expected (field, event) pairs and that every entry of the field
table is one of them. No original data is retained; the output is a summary.
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

# handler -> {(table, id, event)}
EXPECTED = {
    0x108a5430: {('field', 0, 0x7f)},                       # Wish
    0x108a5890: {('field', 1, 0x5e), ('field', 1, 0xb0)},  # Lunar Dance
    0x108a5714: {('field', 2, 0x5e), ('field', 2, 0xb0)},  # Healing Wish
    0x108a55e0: {('field', 3, 0x7f)},                       # Future Sight / Doom Desire
    0x108a555c: {('field', 4, 0x5d)},                       # the entry effect
}
DEFERRED = set()


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
                if name == 'field':
                    entries['%s 0x%x' % (name, event)] += 1
                    check_tables.require(handler in found or handler in DEFERRED,
                                         'Uncovered entry: %s %d event %#x %#x' % (name, ident, event, handler))
                    if handler in DEFERRED:
                        entries['deferred'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
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
