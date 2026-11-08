#!/usr/bin/env python3
"""Check which original moves reach the field- and side-condition move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that every move entry
of events 0xaa and 0xad is a handler of this milestone. Entries of the other tables are counted.
No original data is retained; the output is a summary.
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

EVENTS = (0xaa, 0xad)
# handler -> {(move, event)}
BINDINGS = {
    0x108af744: {(356, 0xaa)},   # Gravity
    0x108b14a8: {(300, 0xaa)},   # Mud Sport
    0x108b2940: {(346, 0xaa)},   # Water Sport
    0x108b1700: {(587, 0xaa)},   # Fairy Lock
    0x108b6294: {(569, 0xaa)},   # Ion Deluge
    0x108b3164: {(433, 0xaa)},   # Trick Room
    0x108b4988: {(472, 0xaa)},   # Wonder Room
    0x108b2738: {(478, 0xaa)},   # Magic Room
    0x108b3684: {(580, 0xaa)},   # Grassy Terrain
    0x108b28bc: {(581, 0xaa)},   # Misty Terrain
    0x108b15c8: {(604, 0xaa)},   # Electric Terrain
    0x108b2630: {(114, 0xaa)},   # Haze
    0x108b2d6c: {(115, 0xad)},   # Reflect
    0x108b59f0: {(113, 0xad)},   # Light Screen
    0x108b70bc: {(219, 0xad)},   # Safeguard
    0x108b2e34: {(54, 0xad)},    # Mist
    0x108aed68: {(366, 0xad)},   # Tailwind
    0x108b0db4: {(381, 0xad)},   # Lucky Chant
    0x108b4784: {(578, 0xad)},   # Crafty Shield
    0x108b176c: {(501, 0xad)},   # Quick Guard
    0x108b320c: {(469, 0xad)},   # Wide Guard
    0x108b6300: {(561, 0xad)},   # Mat Block
    0x108b092c: {(191, 0xad)},   # Spikes
    0x108b0140: {(390, 0xad)},   # Toxic Spikes
    0x108b5440: {(446, 0xad)},   # Stealth Rock
    0x108b5074: {(564, 0xad)},   # Sticky Web
    0x108b1ae0: {(603, 0xad)},   # Happy Hour
}
EXPECTED = {h: {('move', m, e) for m, e in pairs} for h, pairs in BINDINGS.items()}


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
                    if name == 'move':
                        check_tables.require(handler in found, 'Uncovered entry: move %d event %#x %#x' % (ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'bindings': sum(len(v) for v in EXPECTED.values()), 'events': [hex(e) for e in EVENTS],
            'entries': dict(sorted(entries.items())), 'move_entries_complete': True, 'all_match': True}


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
