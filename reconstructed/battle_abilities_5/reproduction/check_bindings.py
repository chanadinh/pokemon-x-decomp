#!/usr/bin/env python3
"""Check which original abilities reach the redirect, absorb and blocking handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that every other
ability entry of events 0x2b (redirection), 0x24 (targets chosen) and 0x0e
(status prevention) is Leaf Guard, reconstructed in battle_abilities_2. No original data is retained; the output is a summary.
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

EVENTS = (0x2b, 0x24, 0xe)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108be440: {('ability', 31, 0x2b)},    # Lightning Rod
    0x108bebf8: {('ability', 114, 0x2b)},   # Storm Drain
    0x108c5270: {('ability', 31, 0x24), ('ability', 114, 0x24)},  # Lightning Rod, Storm Drain
    0x108c5600: {('ability', 157, 0x30)},   # Sap Sipper
    0x108c20d4: {('ability', 18, 0x93)},    # Flash Fire
    0x108c33b0: {('ability', 18, 0x40)},    # Flash Fire
    0x108bcdbc: {('ability', 171, 0x2e)},   # Bulletproof
    0x108bea04: {('ability', 6, 0x1f)},     # Damp
    0x108c4408: {('ability', 6, 0x21)},     # Damp
    0x108c4eb4: {('ability', 156, 0x1f)},   # Magic Bounce
    0x108c4a88: {('ability', 156, 0x9)},    # Magic Bounce
    0x108c3d90: {('ability', 175, 0xe)},    # Sweet Veil
    0x108c4170: {('ability', 15, 0xe), ('ability', 72, 0xe)},  # Insomnia, Vital Spirit
    0x108c573c: {('ability', 166, 0xe)},    # Flower Veil
    0x108bf18c: {('ability', 64, 0x97)},    # Liquid Ooze
}
# The other ability entry of these events, reconstructed in battle_abilities_2: Leaf Guard (event 0x0e).
DEFERRED = {0x108c55c0}


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
