#!/usr/bin/env python3
"""Check which original abilities reach the last ability handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that the three handlers are bound to
exactly the expected (ability, event) pairs, that the helper is bound to
nothing, that every ability entry of event 0x91 is one of them, and that every
handler of the whole ability table now appears in some milestone's
evidence/functions.json. No original data is retained; the output is a
summary.
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

EVENTS = (0x91,)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108beca4: {('ability', 170, 0x8c)},   # Magician
    0x108c2ae8: {('ability', 113, 0x4)},    # Scrappy
    0x108c1a68: {('ability', 184, 0x91), ('ability', 182, 0x91), ('ability', 174, 0x91)},  # Aerilate, Pixilate, Refrigerate
    0x1087a19c: set(),                      # helper: called directly, never bound
}
DEFERRED = set()


def reconstructed_handlers(root):
    """Every function address any milestone's evidence/functions.json lists."""
    done = set()
    for path in sorted((root / 'reconstructed').glob('*/evidence/functions.json')):
        for row in json.loads(path.read_text()):
            done.add(int(row['address'], 16))
    return done


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    done, ability_handlers = reconstructed_handlers(root), set()
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
                if name == 'ability':
                    ability_handlers.add(handler)
                if handler in found:
                    found[handler].add((name, ident, event))
    missing = sorted(hex(h) for h in ability_handlers - done)
    check_tables.require(not missing, 'Ability handlers not reconstructed: %s' % missing)
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': sum(1 for v in EXPECTED.values() if v), 'helpers_unbound': sum(1 for v in EXPECTED.values() if not v), 'ability_table_handlers': len(ability_handlers), 'ability_table_handlers_reconstructed': len(ability_handlers & done),
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
