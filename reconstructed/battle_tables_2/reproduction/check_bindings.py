#!/usr/bin/env python3
"""Check the battle_tables_2 move table functions against the original move table.

Reads the 296 rows of the move table of DllBattle.cro (108d3ddc, 8 bytes each:
a 16-bit move id and a table function) with the battle_handler milestone's
table-function reader and confirms that:

- every row names a table function and every function is a regular
  four-instruction stub (a count constant, a store, an entries literal, a
  return) whose count word is (priority << 16 | count) with a count from 1
  to 255 and a priority below 8;
- the 248 distinct functions are exactly the ones in this milestone's
  evidence/functions.json and none of them is also covered by an earlier
  milestone;
- the move registrar 10869dd4 is covered by some milestone's evidence, as
  are all of the earlier milestones' functions it calls.
No original data is retained; the output is a summary.
"""
import argparse
import collections
import glob
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve()
sys.path.insert(0, str(HERE.parents[2] / 'battle_handler/reproduction'))
import check_tables  # noqa: E402

TABLE, ROWS = 0x108d3ddc, 0x128
SHAPE_REFERENCE = 0x108a53cc
REGISTRAR = 0x10869dd4


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    evidence = HERE.parents[1] / 'evidence'
    mine = {int(r['address'], 16) for r in json.loads((evidence / 'functions.json').read_text())}
    elsewhere = set()
    for path in glob.glob(str(HERE.parents[2] / '*/evidence/functions.json')):
        if Path(path).parents[1].name != 'battle_tables_2':
            elsewhere |= {int(r['address'], 16) for r in json.loads(Path(path).read_text())}
    distinct, counts, priorities, ids = set(), collections.Counter(), collections.Counter(), set()
    for i in range(ROWS):
        at = TABLE + 8 * i
        check_tables.require(at + 4 in module.relocated, 'Move row %d has no table function' % i)
        ids.add(module.byte(at) | module.byte(at + 1) << 8)
        function = module.relocated[at + 4]
        distinct.add(function)
        word, _ = check_tables.decode_function(module, function)
        check_tables.require(0 < (word & 0xffff) < 256 and word >> 16 < 8, 'Count word %#x of %#x is out of range' % (word, function))
        counts['count %d' % (word & 0xffff)] += 1
        priorities['priority %d' % (word >> 16)] += 1
    check_tables.require(len(distinct) == 248, 'Expected 248 distinct move table functions, found %d' % len(distinct))
    check_tables.require(mine == distinct, 'The evidence stubs are not the move table functions')
    check_tables.require(not (distinct & elsewhere), 'A move table function is also covered by an earlier milestone')
    check_tables.require(SHAPE_REFERENCE in elsewhere and SHAPE_REFERENCE not in mine and SHAPE_REFERENCE not in distinct,
                         'The shape reference is not owned by an earlier milestone')
    field_functions = {module.relocated[0x108d6810 + 8 * i + 4] for i in range(5)}
    check_tables.require(SHAPE_REFERENCE in field_functions, 'The shape reference is not a field table function')
    check_tables.require(REGISTRAR in elsewhere, 'The move registrar is not covered')
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'rows': ROWS, 'distinct_move_ids': len(ids),
            'distinct_table_functions': len(distinct), 'shape_reference_owned_by_battle_tables_1': '%08x' % SHAPE_REFERENCE,
            'registrar_covered_elsewhere': '%08x' % REGISTRAR, 'count_values': dict(sorted(counts.items())),
            'priorities': dict(sorted(priorities.items())), 'all_match': True}


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
