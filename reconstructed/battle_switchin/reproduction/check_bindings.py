#!/usr/bin/env python3
"""Check which original abilities reach the switch-in handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs, and that the other
entries of event 0x94 are exactly the five deferred handlers. No original
data is retained; the output is a summary.
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

EVENTS = (0x94,)
# handler -> {(table, id, event)}
EXPECTED = {
    0x1084aca8: {('ability', 70, 0x94)},    # Drought
    0x108bef5c: {('ability', 2, 0x94)},     # Drizzle
    0x108bf8f8: {('ability', 45, 0x94)},    # Sand Stream
    0x108c02dc: {('ability', 117, 0x94)},   # Snow Warning
    0x108c06c8: {('ability', 7, 0x94)},     # Limber
    0x108bfd2c: {('ability', 15, 0x94), ('ability', 72, 0x94)},  # Insomnia, Vital Spirit
    0x108c072c: {('ability', 17, 0x94)},    # Immunity
    0x108c07b0: {('ability', 20, 0x94)},    # Own Tempo
    0x108c2b50: {('ability', 41, 0x94)},    # Water Veil
    0x108c4368: {('ability', 40, 0x94)},    # Magma Armor
    0x108c0518: {('ability', 12, 0x94)},    # Oblivious
    0x108c1ee8: {('ability', 22, 0x94)},    # Intimidate
    0x108c202c: {('ability', 127, 0x94)},   # Unnerve
    0x108c2210: {('ability', 59, 0x94)},    # Forecast
    0x108c393c: {('ability', 122, 0x94)},   # Flower Gift
    0x108c4500: {('ability', 112, 0x94)},   # Slow Start
    0x108c0074: {('ability', 54, 0x94)},    # Truant
    0x108c4918: {('ability', 104, 0x94), ('ability', 163, 0x94), ('ability', 164, 0x94)},  # Mold Breaker family
    0x108c3c3c: {('ability', 46, 0x94)},    # Pressure
    0x108c3800: {('ability', 186, 0x94)},   # Dark Aura
    0x108c4028: {('ability', 187, 0x94)},   # Fairy Aura
    0x108c3f58: {('ability', 188, 0x94)},   # Aura Break
    0x108c3fe0: {('ability', 103, 0x94)},   # Klutz
}
# Most of these handlers also serve event 0x5e (the ability taking effect mid-battle).
for _h in (0x1084aca8, 0x108bef5c, 0x108bf8f8, 0x108c02dc, 0x108c06c8, 0x108bfd2c, 0x108c072c, 0x108c2b50, 0x108c4368,
           0x108c1ee8, 0x108c4500, 0x108c4918, 0x108c3c3c, 0x108c3800, 0x108c4028, 0x108c3f58):
    EXPECTED[_h] |= {(t, i, 0x5e) for t, i, _ in EXPECTED[_h]}
# Handlers of event 0x94 not reconstructed yet: Trace, Download, Forewarn, Anticipation, Frisk.
DEFERRED = {0x108bcb0c, 0x108be1dc, 0x108bd460, 0x108bf390, 0x108be6bc}


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
                    check_tables.require(handler in found or handler in DEFERRED, 'Uncovered entry: %s %d event %#x %#x' % (name, ident, event, handler))
                    if handler in DEFERRED:
                        entries['deferred'] += 1
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
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
