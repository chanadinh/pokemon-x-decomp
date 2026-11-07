#!/usr/bin/env python3
"""Check which original abilities reach the contact handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs, that the helpers are
bound to nothing, and that the other ability entries of event 0x53 are
exactly the two deferred handlers. No original data is retained; the output
is a summary.
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

EVENT = 0x53
# handler -> {(table, id, event)}
EXPECTED = {
    0x108be9d0: {('ability', 9, EVENT)},     # Static
    0x108bfcf8: {('ability', 38, EVENT)},    # Poison Point
    0x108c0edc: {('ability', 49, EVENT)},    # Flame Body
    0x108bc8dc: {('ability', 27, EVENT)},    # Effect Spore
    0x108c0f60: {('ability', 56, EVENT)},    # Cute Charm
    0x108bd6e0: {('ability', 143, EVENT)},   # Poison Touch
    0x108be8f8: {('ability', 24, EVENT), ('ability', 160, EVENT)},  # Rough Skin, Iron Barbs
    0x108bde94: {('ability', 106, EVENT)},   # Aftermath
    0x108bc9a8: {('ability', 152, EVENT)},   # Mummy
    0x108be5f8: {('ability', 183, EVENT)},   # Gooey
    0x108bcd28: {('ability', 155, EVENT)},   # Rattled
    0x108c19bc: {('ability', 154, EVENT)},   # Justified
    0x108c05ec: {('ability', 83, EVENT)},    # Anger Point
    0x108c1784: {('ability', 133, EVENT)},   # Weak Armor
    # Shared helpers: called directly, never bound.
    0x1084ad50: set(), 0x1085f844: set(), 0x1084d2f4: set(),
}
# Ability handlers of event 0x53 not reconstructed yet: Cursed Body, Illusion.
DEFERRED = {0x108c10a0, 0x108c29c0}


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
                if event == EVENT:
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
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': sum(1 for v in EXPECTED.values() if v),
            'helpers_unbound': sum(1 for v in EXPECTED.values() if not v),
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
