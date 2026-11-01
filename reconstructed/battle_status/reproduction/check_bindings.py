#!/usr/bin/env python3
"""Check which original abilities and moves reach the status-prevention handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that these are all
the entries of events 0x6e and 0x70 in every table. No original
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

EVENTS = (0x6e, 0x70)
SHARED_MESSAGE = 0x108c35f8   # one message handler for nine abilities
# handler -> {(table, id, event)}
EXPECTED = {
    0x108c2a58: {('ability', 7, 0x6e)},                           # Limber
    0x108c1cb8: {('ability', 15, 0x6e), ('ability', 72, 0x6e)},   # Insomnia, Vital Spirit
    0x108c556c: {('ability', 40, 0x6e)},                          # Magma Armor
    0x108c4b58: {('ability', 41, 0x6e)},                          # Water Veil
    0x108c2afc: {('ability', 17, 0x6e)},                          # Immunity
    0x108c2b88: {('ability', 20, 0x6e)},                          # Own Tempo
    0x108bce68: {('ability', 12, 0x6e)},                          # Oblivious
    0x108bf864: {('ability', 102, 0x6e)},                         # Leaf Guard
    0x108c4618: {('ability', 175, 0x6e)},                         # Sweet Veil
    0x108c4db8: {('ability', 166, 0x6e)},                         # Flower Veil
    0x108c333c: {('ability', 165, 0x6e)},                         # Aroma Veil
    0x108bae4c: {('move', 253, 0x6e)},                            # Uproar
    SHARED_MESSAGE: {('ability', i, 0x70) for i in (102, 12, 7, 15, 72, 40, 41, 17)},
    0x108c4fa4: {('ability', 20, 0x70)},
    0x108c3b00: {('ability', 165, 0x70)},
    0x108c51c8: {('ability', 166, 0x70)},
    0x108c5798: {('ability', 175, 0x70)},
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
