#!/usr/bin/env python3
"""Check which original abilities reach the status-cure and secondary-effect handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs, that the four helpers
are bound to nothing, and that every other ability entry of the events
covered is either reconstructed in battle_abilities_4 (Inner Focus, Serene
Grace) or one of the two deferred event-0x02 handlers of Forecast and Flower
Gift. No original data is retained; the output is a summary.
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

EVENTS = (0x2, 0x5c, 0x74, 0x52, 0x59, 0x6d, 0x75, 0x76, 0x8b)
# handler -> {(table, id, event)}
EXPECTED = {
    0x1084ab94: {('ability', 15, 0x2), ('ability', 72, 0x2)},  # Insomnia, Vital Spirit
    0x108c1774: {('ability', 7, 0x2)},      # Limber
    0x108c18b8: {('ability', 17, 0x2)},     # Immunity
    0x108c18c8: {('ability', 20, 0x2)},     # Own Tempo
    0x108c3ba8: {('ability', 41, 0x2)},     # Water Veil
    0x108c4f50: {('ability', 40, 0x2)},     # Magma Armor
    0x108c1578: {('ability', 12, 0x2)},     # Oblivious
    0x108c18d8: {('ability', 54, 0x2)},     # Truant
    0x108c08b8: {('ability', 19, 0x59)},    # Shield Dust
    0x108c08f4: {('ability', 19, 0x6d)},    # Shield Dust
    0x108c11c0: {('ability', 19, 0x52)},    # Shield Dust
    0x108c1990: {('ability', 19, 0x76)},    # Shield Dust
    0x108c43a0: {('ability', 19, 0x8b)},    # Shield Dust
    0x108c3e88: {('ability', 125, 0x8b)},   # Sheer Force
    0x108c5178: {('ability', 125, 0x6d), ('ability', 125, 0x59)},  # Sheer Force
    0x108c566c: {('ability', 125, 0x75)},   # Sheer Force
    0x108bd678: {('ability', 1, 0x75)},     # Stench
    0x108c11ec: {('ability', 30, 0x5c)},    # Natural Cure
    0x108c0940: {('ability', 144, 0x5c)},   # Regenerator
    0x108c0140: {('ability', 90, 0x74)},    # Poison Heal
    0x108c349c: {('ability', 85, 0x74)},    # Heatproof
    # Helpers: called directly, never bound.
    0x1084aba4: set(), 0x1084a890: set(), 0x1084d318: set(), 0x1089e6fc: set(),
}
# Other ability entries of these events: reconstructed in battle_abilities_4 ...
EARLIER = {0x108c09f8, 0x108c0ab0, 0x108c46dc}
# ... and not reconstructed yet (Forecast and Flower Gift on event 0x02).
DEFERRED = {0x108c2cec, 0x108c4108}


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
                            entries['battle_abilities_4'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': sum(1 for v in EXPECTED.values() if v), 'helpers_unbound': sum(1 for v in EXPECTED.values() if not v),
                        'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'battle_abilities_4_handlers': sorted(hex(h) for h in EARLIER), 'deferred_handlers': sorted(hex(h) for h in DEFERRED), 'all_match': True}


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
