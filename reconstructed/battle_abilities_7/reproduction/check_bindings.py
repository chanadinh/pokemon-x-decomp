#!/usr/bin/env python3
"""Check which original abilities reach the trapping, escape, priority and endurance handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that every ability
entry of the eleven events they serve is one of them. No original data is retained; the output is a summary.
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

EVENTS = (0xc, 0xb, 0xd, 0x95, 0x12, 0x1b, 0x56, 0x11, 0x79, 0x7d, 0x7e)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108be518: {('ability', 23, 0xc)},     # Shadow Tag
    0x108bef74: {('ability', 71, 0xc)},     # Arena Trap
    0x108bd98c: {('ability', 42, 0xc)},     # Magnet Pull
    0x108bdd94: {('ability', 50, 0xb)},     # Run Away
    0x108c0828: {('ability', 50, 0xd)},     # Run Away
    0x108bda2c: {('ability', 21, 0x95)},    # Suction Cups
    0x108bc89c: {('ability', 26, 0x12)},    # Levitate
    0x108bfd64: {('ability', 26, 0x1b)},    # Levitate
    0x108be848: {('ability', 46, 0x56)},    # Pressure
    0x108c1728: {('ability', 158, 0x11)},   # Prankster
    0x108c297c: {('ability', 177, 0x11)},   # Gale Wings
    0x108c2850: {('ability', 5, 0x79)},     # Sturdy
    0x108c3a18: {('ability', 5, 0x7d)},     # Sturdy
    0x108c28e4: {('ability', 5, 0x7e)},     # Sturdy
}
# Every ability entry of these events is reconstructed here.
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
