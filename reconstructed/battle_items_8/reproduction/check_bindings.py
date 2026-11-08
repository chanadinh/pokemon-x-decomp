#!/usr/bin/env python3
"""Check which original items reach the batch-8 after-hit item handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event) pairs, in the item table only,
and that every entry of the covered items is one of them, except the resist
berries' event-0x4f handlers, which battle_modifier reconstructs. No original data
is retained; the output is a summary.
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

# Handler -> (event, items).
RESIST = (184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 686)
HANDLERS = {0x108a6e58: (0x53, (540,)),        # Rocky Helmet
            0x108a8f24: (0x5e, (541,)),        # Air Balloon announcement
            0x108aa524: (0x12, (541,)),        # Air Balloon floating
            0x108ab254: (0x53, (541,)),        # Air Balloon popping
            0x108aaa9c: (0x53, (545,)),        # Absorb Bulb check
            0x108a7130: (0x7b, (545,)),        # Absorb Bulb use
            0x108aaf64: (0x53, (546,)),        # Cell Battery check
            0x108ab194: (0x53, (649,)),        # Snowball check
            0x108a7788: (0x7b, (546, 649)),    # Cell Battery and Snowball use
            0x108ab61c: (0x53, (648,)),        # Luminous Moss check
            0x108a8530: (0x7b, (648,)),        # Luminous Moss use
            0x108aafe8: (0x53, (639,)),        # Weakness Policy check
            0x108a9fc0: (0x7b, (639,)),        # Weakness Policy use
            0x108ab960: (0x53, RESIST),        # the resist berries, eaten
            0x108a6310: (0x53, (208,)),        # Enigma Berry check
            0x108a7840: (0x7b, (208,)),        # Enigma Berry use
            0x108a8f98: (0x54, (211,)),        # Jaboca Berry check
            0x108a99a8: (0x54, (212,)),        # Rowap Berry check
            0x108a76a0: (0x7b, (211, 212))}    # Jaboca and Rowap use
# The resist berries' event-0x4f handlers (battle_modifier).
ELSEWHERE = {0x108a638c, 0x108a6630, 0x108a6cc8, 0x108a6794, 0x108a697c, 0x108a6474, 0x108a6120, 0x108a68e4,
             0x108a6494, 0x108a6454, 0x108a6904, 0x108a6df0, 0x108a6abc, 0x108a6610, 0x108a62f0, 0x108a6bec,
             0x108a6188, 0x108a6c0c}
EXPECTED = {h: {('item', i, e) for i in items} for h, (e, items) in HANDLERS.items()}
ITEMS = {i for _, items in HANDLERS.values() for i in items}


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    module = check_tables.Module(cro, manifest, relocations)
    found = {h: set() for h in EXPECTED}
    entries, items = collections.Counter(), set()
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
                if name == 'item' and ident in ITEMS:
                    items.add(ident)
                    entries['item 0x%02x' % event] += 1
                    check_tables.require(handler in found or handler in ELSEWHERE, 'Uncovered entry: item %d event %#x %#x' % (ident, event, handler))
                if handler in found:
                    found[handler].add((name, ident, event))
    check_tables.require(items == ITEMS, 'Missing items: %s' % sorted(ITEMS - items))
    for handler, expected in EXPECTED.items():
        check_tables.require(found[handler] == expected, 'Bindings of %#x differ: %s' % (handler, sorted(found[handler])))
    return {'module_sha256': check_tables.hashlib.sha256(cro).hexdigest(), 'handlers': len(EXPECTED),
            'items': sorted(ITEMS),
            'bindings': sum(len(v) for v in EXPECTED.values()),
            'entries': dict(sorted(entries.items())), 'all_match': True}


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
