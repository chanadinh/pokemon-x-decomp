#!/usr/bin/env python3
"""Check which original abilities reach the transformation, status and move-reaction handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (ability, event) pairs and that every ability
entry of events 0x1e, 0x71, 0x3c, 0x23 and 0x99 is one of them (the other
events they serve are shared with earlier milestones). No original data is retained; the output is a summary.
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

EVENTS = (0x1e, 0x71, 0x3c, 0x23, 0x99)
# handler -> {(table, id, event)}
EXPECTED = {
    0x108bcf0c: {('ability', 150, 0x5e)},   # Imposter
    0x108bd1b4: {('ability', 54, 0x1e)},    # Truant
    0x108c190c: {('ability', 54, 0x21)},    # Truant
    0x108bd33c: {('ability', 28, 0x71)},    # Synchronize
    0x108bf054: {('ability', 188, 0x3c)},   # Aura Break
    0x108c0578: {('ability', 168, 0x23)},   # Protean
    0x108c0e14: {('ability', 176, 0x3)},    # Stance Change
    0x108c16a0: {('ability', 149, 0x73)},   # Illusion
    0x108c29c0: {('ability', 149, 0x53)},   # Illusion
    0x108c419c: {('ability', 149, 0x93)},   # Illusion
    0x108c27ac: {('ability', 80, 0x21)},    # Steadfast
    0x108c4f60: {('ability', 178, 0x99)},   # Mega Launcher
}
# Every ability entry of events 0x1e, 0x71, 0x3c, 0x23 and 0x99 is reconstructed here.
DEFERRED = set()


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
                        check_tables.require(handler in found or handler in DEFERRED,
                                             'Uncovered entry: %s %d event %#x %#x' % (name, ident, event, handler))
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
