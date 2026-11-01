#!/usr/bin/env python3
"""Check which original abilities reach the reconstructed move-power handlers.

Every ability entry for event 0x3d in the original table must be one of them.

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
    0x108bf95c: {(79, 0x3d)},   # Rivalry
    0x108c021c: {(101, 0x3d)},  # Technician
    0x108c12d4: {(89, 0x3d)},   # Iron Fist
    0x108bd2dc: {(120, 0x3d)},  # Reckless
    0x108c34f4: {(85, 0x3d)},   # Heatproof
    0x108c4244: {(87, 0x3d)},   # Dry Skin
    0x108c4708: {(125, 0x3d)},  # Sheer Force
    0x108bfc9c: {(137, 0x3d)},  # Toxic Boost
    0x108c00b8: {(138, 0x3d)},  # Flare Boost
    0x108bd200: {(148, 0x3d)},  # Analytic
    0x108c1244: {(159, 0x3d)},  # Sand Force
    0x108be18c: {(186, 0x3d)},  # Dark Aura
    0x108bf060: {(187, 0x3d)},  # Fairy Aura
    0x108bf148: {(173, 0x3d)},  # Strong Jaw
    0x108bf344: {(181, 0x3d)},  # Tough Claws
    0x108c0a24: {(184, 0x3d)},  # Aerilate
    0x108c15a8: {(182, 0x3d)},  # Pixilate
    0x108c15f8: {(174, 0x3d)},  # Refrigerate
    0x108c32e4: {(178, 0x3d)},  # Mega Launcher
}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    elsewhere, event_entries = [], 0
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
                if name == 'ability' and module.byte(table + 8 * k) == 0x3d:
                    event_entries += 1
                if handler in found:
                    if name == 'ability':
                        found[handler].add((ident, module.byte(table + 8 * k)))
                    else:
                        elsewhere.append((name, ident, handler))
    check_tables.require(not elsewhere, 'Handlers used by other tables: %s' % elsewhere)
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    bindings = sum(len(v) for v in EXPECTED.values())
    # Every ability entry for event 0x3d in the original table is covered.
    check_tables.require(event_entries == bindings, 'Event 0x3d has %d ability entries' % event_entries)
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'abilities': bindings, 'event_0x3d_ability_entries': event_entries, 'event_0x3d_covered': True,
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
