#!/usr/bin/env python3
"""Check which original moves reach the lower half of the last event-0xac move handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (move, event) pairs. It then lists every move
entry that no evidence outside the three split participants (this milestone,
battle_moves_b6 and battle_moves_b7) covers, whatever is on disk, and checks that
the sorted list splits at ceil(n/2) into this milestone's handlers and exactly the
move-table handlers of b6 and b7 (b7's shared body is not a table handler and is
ignored); together with all three every move-table handler is covered.
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

EVENT = 0xac
# handler -> {(move, event)}: the lower half (this milestone)
BINDINGS = {
    0x1084bd3c: {(182, EVENT), (197, EVENT)},      # Protect, Detect
    0x108add30: {(174, EVENT)},                    # Curse
    0x108adfb4: {(271, EVENT), (415, EVENT)},      # Trick, Switcheroo
    0x108ae1a0: {(180, EVENT)},                    # Spite
    0x108aeb70: {(156, EVENT)},                    # Rest
    0x108aeff8: {(166, EVENT)},                    # Mimic
    0x108b0248: {(516, EVENT)},                    # Bestow
    0x108b0468: {(244, EVENT)},                    # Psych Up
    0x108b1898: {(385, EVENT)},                    # Guard Swap
    0x108b1c7c: {(391, EVENT)},                    # Heart Swap
    0x108b1e4c: {(293, EVENT)},                    # Camouflage
}
# The upper half (recorded; checked against the move-table handlers of battle_moves_b6 and b7).
UPPER = (0x108b23bc, 0x108b2b2c, 0x108b2eb8, 0x108b3458, 0x108b40fc, 0x108b42b4, 0x108b481c, 0x108b50c8, 0x108b5620, 0x108b87e0)
EXPECTED = {h: {('move', m, e) for m, e in pairs} for h, pairs in BINDINGS.items()}


def evidence_handlers(root, skip):
    found = set()
    for path in sorted(root.glob('reconstructed/*/evidence/functions.json')):
        if path.parts[-3] in skip:
            continue
        for row in json.loads(path.read_text()):
            if isinstance(row, dict) and 'address' in row:
                found.add(int(row['address'], 16))
    return found


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    entries = collections.Counter()
    move_handlers = {}
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
                    move_handlers.setdefault(handler, set()).add((ident, event))
                if handler in found:
                    found[handler].add((name, ident, event))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    participants = {HERE.parents[1].name, 'battle_moves_b6', 'battle_moves_b7'}
    done = evidence_handlers(root, participants)
    left = sorted(h for h in move_handlers if h not in done)
    check_tables.require(all(any(e == EVENT for _, e in move_handlers[h]) for h in left), 'A remaining handler is not on event 0xac')
    half = (len(left) + 1) // 2
    check_tables.require(left[:half] == sorted(EXPECTED),
                         'The lower half of the sorted list is not this milestone')
    # The upper half must be exactly the move-table handlers of battle_moves_b6 and battle_moves_b7 (b7's shared
    # body 108b87e4 is not a table handler and is ignored).
    upper = set()
    for name in ('battle_moves_b6', 'battle_moves_b7'):
        path = root / 'reconstructed' / name / 'evidence/functions.json'
        check_tables.require(path.exists(), '%s is not on disk' % name)
        upper |= {int(r['address'], 16) for r in json.loads(path.read_text())} & set(move_handlers)
    check_tables.require(left[half:] == sorted(upper), 'The upper half is not exactly the move-table handlers of b6 and b7')
    check_tables.require(set(UPPER) == upper, 'The recorded upper half differs from b6 and b7')
    covered = set(EXPECTED) | upper | done
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'bindings': sum(len(v) for v in EXPECTED.values()), 'event': hex(EVENT),
            'entries': dict(sorted(entries.items())),
            'unreconstructed_elsewhere': len(left), 'split_after': half,
            'upper_half_is_exactly_b6_union_b7_table_handlers': True, 'upper_half_handlers': len(upper),
            'move_table_handlers': len(move_handlers),
            'every_move_table_handler_covered_with_b6_and_b7': all(h in covered for h in move_handlers), 'all_match': True}


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
