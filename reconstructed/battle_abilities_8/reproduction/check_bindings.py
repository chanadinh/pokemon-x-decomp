#!/usr/bin/env python3
"""Check which original abilities reach the item and after-move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that every other
ability entry of the events covered is either a contact handler of
battle_abilities_1 (event 0x53) or one of the two deferred handlers, Magician
(event 0x8c) and Illusion (event 0x53). No original data is retained; the
output is a summary.
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

EVENTS = (0xa6, 0xa7, 0x90, 0x8a, 0x9b, 0x8c, 0x53)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108be5b8: {('ability', 60, 0xa6)},    # Sticky Hold
    0x108c3bb8: {('ability', 60, 0xa7)},    # Sticky Hold
    0x108c0aec: {('ability', 124, 0x90)},   # Pickpocket
    0x108c2444: {('ability', 170, 0x8a)},   # Magician
    0x108bdf78: {('ability', 180, 0x9b)},   # Symbiosis
    0x108bf22c: {('ability', 167, 0x9b)},   # Cheek Pouch
    0x108bfe10: {('ability', 153, 0x8c)},   # Moxie
    0x108be340: {('ability', 16, 0x8c)},    # Color Change
    0x108c10a0: {('ability', 130, 0x53)},   # Cursed Body
}
# Other ability entries of these events: the contact handlers of battle_abilities_1 (event 0x53) ...
EARLIER = {0x108bc8dc, 0x108bc9a8, 0x108bcd28, 0x108bd6e0, 0x108bde94, 0x108be5f8, 0x108be8f8, 0x108be9d0, 0x108bfcf8, 0x108c05ec, 0x108c0edc, 0x108c0f60, 0x108c1784, 0x108c19bc}
# ... and not reconstructed yet: Magician (event 0x8c) and Illusion (event 0x53).
DEFERRED = {0x108beca4, 0x108c29c0}


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
                        check_tables.require(handler in found or handler in DEFERRED or handler in EARLIER,
                                             'Uncovered entry: %s %d event %#x %#x' % (name, ident, event, handler))
                        if handler in DEFERRED:
                            entries['deferred'] += 1
                        if handler in EARLIER:
                            entries['battle_abilities_1'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': sum(1 for v in EXPECTED.values() if v), 'helpers_unbound': sum(1 for v in EXPECTED.values() if not v),
                        'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'battle_abilities_1_handlers': sorted(hex(h) for h in EARLIER), 'deferred_handlers': sorted(hex(h) for h in DEFERRED), 'all_match': True}


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
