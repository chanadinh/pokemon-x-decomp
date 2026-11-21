#!/usr/bin/env python3
"""Check which original moves reach the 8 handlers of event 0xac in battle_moves_b6.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that the 8 handlers are bound to
exactly the expected (move, event) pairs (on any event) and that the Acupressure stat table equals the table the host copy uses. The other move entries
of event 0xac are counted as not covered by this milestone. No original data is
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
    0x108b23bc: {('move', 432, 0xac)},  # Defog
    0x108b2b2c: {('move', 384, 0xac)},  # Power Swap
    0x108b2eb8: {('move', 254, 0xac)},  # Stockpile
    0x108b3458: {('move', 226, 0xac)},  # Baton Pass
    0x108b40fc: {('move', 375, 0xac)},  # Psycho Shift
    0x108b42b4: {('move', 502, 0xac)},  # Ally Switch
    0x108b481c: {('move', 367, 0xac)},  # Acupressure
    0x108b50c8: {('move', 596, 0xac)},  # Spiky Shield
}
# The Acupressure stat table the relocated literal at 108b4930 points to (segment_1 + 19526).
TABLE_ADDRESS = 0x108d2c46
TABLE = (1, 2, 5, 3, 4, 6, 7)


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    stat_table = tuple(module.byte(TABLE_ADDRESS + k) for k in range(len(TABLE)))
    check_tables.require(stat_table == TABLE, "Acupressure table differs: %s" % (stat_table,))
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
                    if name == 'move':
                        if handler not in found:
                            entries['other handlers'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED), 'acupressure_table': list(stat_table),
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
