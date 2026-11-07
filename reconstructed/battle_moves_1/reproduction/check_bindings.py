#!/usr/bin/env python3
"""Check which original moves reach the base-power move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs, and that the other move
entries of event 0x3c are exactly the one deferred handler (Eruption and
Water Spout). Entries of event 0x3c in the other tables are counted. No
original data is retained; the output is a summary.
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

EVENT = 0x3c
# handler -> move ids, all on event 0x3c only.
MOVES = {
    0x108ae96c: (265,),            # Smelling Salts
    0x108b6f0c: (358,),            # Wake-Up Slap
    0x108b0ee8: (506,),            # Hex
    0x108af4e4: (372,),            # Assurance
    0x108b44c8: (371,),            # Payback
    0x108b0f40: (16, 239),         # Gust, Twister
    0x108af494: (512,),            # Acrobatics
    0x108b2a04: (228,),            # Pursuit
    0x108b8090: (311,),            # Weather Ball
    0x108afb0c: (279, 419),        # Revenge, Avalanche
    0x108b53a0: (496,),            # Round
    0x108af994: (216,),            # Return
    0x108b32c4: (218,),            # Frustration
    0x108aedc0: (386,),            # Punishment
    0x108b33d0: (500,),            # Stored Power
    0x108b05f4: (175, 179),        # Flail, Reversal
    0x108b4250: (378, 462),        # Wring Out, Crush Grip
    0x108b03d4: (360,),            # Gyro Ball
    0x108b164c: (486,),            # Electro Ball
    0x108b068c: (67, 447),         # Low Kick, Grass Knot
    0x108b4d08: (484, 535),        # Heavy Slam, Heat Crash
    0x108b07c0: (376,),            # Trump Card
    0x108b59a8: (255,),            # Spit Up
    0x108b8f8c: (363,),            # Natural Gift
    0x108bad24: (374,),            # Fling
    0x108b8200: (251,),            # Beat Up
    0x108b150c: (497,),            # Echoed Voice
    0x108b5304: (210,),            # Fury Cutter
    0x108b5ea8: (205, 301),        # Rollout, Ice Ball
    0x108b47dc: (167,),            # Triple Kick
    0x108b6ecc: (222,),            # Magnitude
    0x108b52bc: (217,),            # Present
    0x108b67b0: (518, 519, 520),   # Grass, Fire and Water Pledge
}
EXPECTED = {h: {('move', m, EVENT) for m in moves} for h, moves in MOVES.items()}
# Move entries of event 0x3c not reconstructed yet: Eruption and Water Spout (a multiply the decoder refuses).
DEFERRED = {0x108adae8}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    deferred = collections.defaultdict(set)
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
                    if name == 'move':
                        check_tables.require(handler in found or handler in DEFERRED,
                                             'Uncovered entry: move %d event %#x %#x' % (ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
                if handler in DEFERRED:
                    deferred[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    check_tables.require(deferred[0x108adae8] == {('move', 284, EVENT), ('move', 323, EVENT)}, 'Deferred bindings differ')
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'bindings': sum(len(v) for v in EXPECTED.values()), 'moves': len({m for v in MOVES.values() for m in v}),
            'entries': dict(sorted(entries.items())), 'deferred_handlers': sorted(hex(h) for h in DEFERRED),
            'deferred_bindings': sum(len(v) for v in deferred.values()), 'all_match': True}


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
