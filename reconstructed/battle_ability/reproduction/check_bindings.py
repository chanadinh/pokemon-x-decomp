#!/usr/bin/env python3
"""Check which original abilities and events reach the reconstructed handlers.

Decodes the original ability table of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound exactly to the expected abilities and events, and that no other row of
any lookup table (ability, item, move or field) uses it. No original data is
retained; the output is a summary.
"""
import argparse
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
import check_tables  # noqa: E402

# handler -> {(ability, event)}
EXPECTED = {
    0x108bf010: {(47, 0x40)},             # Thick Fat
    0x108c0258: {(37, 0x40), (74, 0x40)},  # Huge Power, Pure Power
    0x108bca9c: {(66, 0x40)},             # Blaze
    0x108bd838: {(67, 0x40)},             # Torrent
    0x108bea50: {(65, 0x40)},             # Overgrow
    0x108c1030: {(68, 0x40)},             # Swarm
    0x108bd110: {(62, 0x40)},             # Guts
    0x108c1648: {(63, 0x41)},             # Marvel Scale
    0x108c3a64: {(55, 0x40)},             # Hustle
    0x1084cd44: {(57, 0x40), (58, 0x40)},  # Plus, Minus
    0x108bd618: {(129, 0x40)},            # Defeatist
    0x108bd7fc: {(169, 0x41)},            # Fur Coat
    0x108c0f10: {(179, 0x41)},            # Grass Pelt
}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    elsewhere = []
    for name, (base, count, key) in check_tables.TABLES.items():
        size = {'B': 1, 'H': 2, 'I': 4}[key]
        for i in range(count):
            at = base + 8 * i
            if at + 4 not in module.relocated:
                continue
            ident = int.from_bytes(bytes(module.byte(at + k) for k in range(size)), 'little')
            word, table = check_tables.decode_function(module, module.relocated[at + 4])
            for k in range(word & 0xffff):
                handler = module.relocated[table + 8 * k + 4]
                if handler in found:
                    if name == 'ability':
                        found[handler].add((ident, module.byte(table + 8 * k)))
                    else:
                        elsewhere.append((name, ident, handler))
    check_tables.require(not elsewhere, 'Handlers used by other tables: %s' % elsewhere)
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'abilities': sum(len(v) for v in EXPECTED.values()),
            'event_0x40_bindings': sum(1 for v in EXPECTED.values() for _, e in v if e == 0x40),
            'event_0x41_bindings': sum(1 for v in EXPECTED.values() for _, e in v if e == 0x41),
            'other_table_uses': 0, 'all_match': True}


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
