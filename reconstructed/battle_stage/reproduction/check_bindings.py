#!/usr/bin/env python3
"""Check which original abilities reach the stat-stage handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that these are all
the entries of events 0x63, 0x64, 0x65 and 0x66 in every table. No original
data is retained; the output is a summary.
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

EVENTS = (0x63, 0x64, 0x65, 0x66)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108bd42c: {('ability', 86, 0x63)},                          # Simple
    0x108bef24: {('ability', 126, 0x63)},                         # Contrary
    0x108c2740: {('ability', 29, 0x64), ('ability', 73, 0x64)},   # Clear Body, White Smoke
    0x108c489c: {('ability', 52, 0x64)},                          # Hyper Cutter
    0x108c2bdc: {('ability', 51, 0x64)},                          # Keen Eye
    0x108c1e40: {('ability', 145, 0x64)},                         # Big Pecks
    0x108c30a4: {('ability', 166, 0x64)},                         # Flower Veil
    0x108c2790: {('ability', 29, 0x65), ('ability', 73, 0x65)},
    0x108c48fc: {('ability', 52, 0x65)},
    0x108c2c3c: {('ability', 51, 0x65)},
    0x108c1ea0: {('ability', 145, 0x65)},
    0x108c3134: {('ability', 166, 0x65)},
    0x108bdab0: {('ability', 128, 0x66)},                         # Defiant
    0x108bd070: {('ability', 172, 0x66)},                         # Competitive
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
                    check_tables.require(handler in found, 'Uncovered entry: %s %d event %#x %#x' % (name, ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'events_covered': [hex(e) for e in EVENTS], 'all_match': True}


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
