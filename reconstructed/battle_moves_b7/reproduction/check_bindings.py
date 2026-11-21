#!/usr/bin/env python3
"""Check which original moves reach the 2 handlers of event 0xac in battle_moves_b7.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that the 2 handlers are bound to
exactly the expected (move, event) pairs (on any event). It also confirms that every move-table handler (all events) appears in some
evidence/functions.json under reconstructed/ (battle_moves_19 included). No original data is
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
    0x108b87e0: {('move', 215, 0xac)},  # Heal Bell (nop word, falls into 108b87e4)
    0x108b5620: {('move', 312, 0xac)},  # Aromatherapy (branch to 108b87e4)
}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    move_handlers = set()
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
                if name == 'move':
                    move_handlers.add(handler)
    covered = set()
    for functions in sorted((root / 'reconstructed').glob('*/evidence/functions.json')):
        covered.update(int(row['address'], 16) for row in json.loads(functions.read_text()))
    check_tables.require(not (move_handlers - covered),
                         'Uncovered move handlers: %s' % sorted(hex(h) for h in move_handlers - covered))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED), 'move_table_handlers': len(move_handlers), 'uncovered_move_table_handlers': 0,
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
