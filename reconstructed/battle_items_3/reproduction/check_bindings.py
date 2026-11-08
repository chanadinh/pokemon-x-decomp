#!/usr/bin/env python3
"""Check which original items reach the batch-3 power-boosting item handlers.

Decodes all four lookup tables of DllBattle.cro with the battle_handler
milestone's table reader and confirms that every reconstructed handler is
bound to exactly the expected (item, event 0x3d) pairs, plus Poison Barb's
event-0x7c handler, in the item table only, and that every entry of the covered items (the type items, plates and
incenses, the three orbs, Muscle Band and Wise Glasses) is one of them. No
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

# Handler -> items bound on event 0x3d.
HANDLERS = {0x108a9268: (251,),        # Silk Scarf, Normal
            0x108a5de0: (241, 303),    # Black Belt, Fist Plate, Fighting
            0x108a92b0: (244, 306),    # Sharp Beak, Sky Plate, Flying
            0x108a6140: (245, 304),    # Poison Barb, Toxic Plate, Poison
            0x108a82b8: (237, 305),    # Soft Sand, Earth Plate, Ground
            0x108a6254: (238, 309),    # Hard Stone, Stone Plate, Rock
            0x108a6e10: (315,),        # Rock Incense
            0x108a65c8: (222, 308),    # Silver Powder, Insect Plate, Bug
            0x108a78c4: (247, 310),    # Spell Tag, Spooky Plate, Ghost
            0x108a6650: (233, 313),    # Metal Coat, Iron Plate, Steel
            0x108a5e28: (249, 298),    # Charcoal, Flame Plate, Fire
            0x108a8170: (243, 299),    # Mystic Water, Splash Plate, Water
            0x108a6d50: (254,),        # Sea Incense
            0x108a8940: (317,),        # Wave Incense
            0x108a77e4: (239, 301),    # Miracle Seed, Meadow Plate, Grass
            0x108a72bc: (318,),        # Rose Incense
            0x108a6080: (242, 300),    # Magnet, Zap Plate, Electric
            0x108a8058: (248, 307),    # Twisted Spoon, Mind Plate, Psychic
            0x108a699c: (314,),        # Odd Incense
            0x108a7b98: (246, 302),    # Never-Melt Ice, Icicle Plate, Ice
            0x108a6c2c: (250, 311),    # Dragon Fang, Draco Plate, Dragon
            0x108a70e8: (240, 312),    # Black Glasses, Dread Plate, Dark
            0x108a7398: (644,),        # Pixie Plate, Fairy
            0x108a6b14: (135,),        # Adamant Orb
            0x108a63e4: (136,),        # Lustrous Orb
            0x108a69e4: (112,),        # Griseous Orb
            0x108aa384: (266,),        # Muscle Band
            0x108a8820: (267,)}        # Wise Glasses
EXPECTED = {h: {('item', i, 0x3d) for i in items} for h, items in HANDLERS.items()}
EXPECTED[0x108a8ddc] = {('item', 245, 0x7c)}   # Poison Barb's use (as when flung)
ITEMS = {i for items in HANDLERS.values() for i in items}


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
                    check_tables.require(handler in found, 'Uncovered entry: item %d event %#x %#x' % (ident, event, handler))
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
