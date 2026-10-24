"""Black-box checks of --zone wild encounters and battles: tall-grass tiles and
a ZO section-3 encounter table added to the synthetic RomFS of
test_bch_zone_viewer, with hand-built personal, move and learnset archives, a
type chart inside a CRO image and English name files. No game data."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_lighting_viewer import lit_fixture
from test_bch_talk_viewer import text_file, units
from test_bch_walk_viewer import FLOOR
from test_bch_zone_viewer import NPC, entities, write_romfs
from test_field_viewer import collision, garc, minipack
from test_model_viewer import VIEWER
from test_viewer import bmp_pixel

GRASS = 0x20002004
SKY_ROW = 10  # in the 800 x 480 window: the battle screen's sky


def grid():
    words = [0x01000021] * 1600
    for x in (20, 21):
        words[21 * 40 + x] = 0x20
    for x in (22, 23, 24):
        words[21 * 40 + x] = GRASS
    return struct.pack('<HH', 40, 40) + struct.pack('<1600I', *words)


def encounters(species=2, level=(3, 3)):
    slots = bytearray(4 * 94)
    for i in range(12):
        struct.pack_into('<HBB', slots, 4 * i, species, *level)
    return bytes([1] + [0] * 15) + bytes(slots)


def personal(stats, kind):
    entry = bytearray(0x40)
    entry[0:6] = bytes(stats)
    entry[6] = entry[7] = kind
    return bytes(entry)


def move(kind, category, power, accuracy, pp, target=0, stat=0, stage=0):
    entry = bytearray(0x24)
    entry[0:6] = bytes([kind, 0, category, power, accuracy, pp])
    entry[20] = target
    entry[21] = stat
    entry[24] = stage & 0xff
    return bytes(entry)


def learnset(*pairs):
    return b''.join(struct.pack('<Hh', m, l) for m, l in pairs) + struct.pack('<HH', 0xffff, 0xffff)


def write_battle_romfs(root, rate_zone=True):
    write_romfs(root)
    zone0 = minipack(b'ZO', [bytes(56), entities([], [{'zone': 1, 'warp': 0, 'face': 1, 'x': 21, 'z': 23}]),
                             b'', encounters() if rate_zone else b''])
    other = minipack(b'ZO', [bytearray(struct.pack('<HHH', 0, 0, 1)) + bytes(50), entities([]), b'', b''])
    (root / 'a/0/1/2').write_bytes(garc([zone0, other, other]))
    field = minipack(b'GR', [grid(), lit_fixture().finish(), collision(FLOOR), b'', b'', b''], 128)
    (root / 'a/0/4/1').write_bytes(garc([field, field]))
    # Species 1 (the partner, Water), 2 (wild, Normal); move 1 Pound-like,
    # 2 Growl-like (foe Attack -1).
    people = [personal([1] * 6, 0), personal([41, 56, 40, 71, 62, 44], 10), personal([38, 36, 38, 57, 32, 36], 0)]
    (root / 'a/2/1').mkdir(parents=True, exist_ok=True)
    (root / 'a/2/1/8').write_bytes(garc(people + [b''.join(people)]))
    (root / 'a/2/1/2').write_bytes(garc([move(0, 0, 0, 0, 0), move(0, 1, 40, 100, 35),
                                         move(0, 0, 0, 100, 40, target=5, stat=1, stage=-1)]))
    (root / 'a/2/1/4').write_bytes(garc([learnset(), learnset((1, 1)), learnset((2, 1), (1, 3))]))
    cro = bytearray(0xD12A8 + 324)
    cro[0x80:0x84] = b'CRO0'
    cro[0xD12A8:] = bytes([4] * 324)
    (root / 'DllBattle.cro').write_bytes(bytes(cro))
    names = [b'\0'] * 81
    names[80] = text_file([units(n) for n in ('Egg', 'Froakie', 'Bunnelby')])
    names[14] = text_file([units(n) for n in ('-', 'Pound', 'Growl')])
    names[17] = text_file([units(n) for n in ('Normal', 'Fighting')])
    (root / 'a/0/7/4').parent.mkdir(parents=True, exist_ok=True)
    (root / 'a/0/7/4').write_bytes(garc(names))


class BCHBattleViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run([str(VIEWER), '--romfs', str(root), *map(str, arguments)], text=True,
                                capture_output=True, env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=120)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def lines(self, text, prefix):
        return [line for line in text.split('\n') if line.startswith(prefix)]

    def test_encounter_on_grass(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_battle_romfs(root)
            common = ('--zone', 0, '--partner', 1, '--encounter-rate', 100)
            out = self.run_viewer(root, *common, '--steps', 'R', '--inspect').stdout
            self.assertNotIn('encounter ', out)  # not grass
            out = self.run_viewer(root, *common, '--steps', 'RRRR', '--inspect').stdout
            self.assertIn('battle_data personal=3 moves=3 chart=DllBattle.cro:0xD12A8 natures=neutral '
                          'names=3,3,2 formulas=reconstructed-stats,community-damage\n', out)
            # Every completed grass step encounters at rate 100: cells 22, 23, 24.
            self.assertEqual([l.split(' slot=')[0] for l in self.lines(out, 'encounter ')],
                             ['encounter zone=0 cell=22,21', 'encounter zone=0 cell=23,21', 'encounter zone=0 cell=24,21'])
            self.assertIn('species=2 level=3\n', out)
            partner = self.lines(out, 'partner ')
            self.assertEqual(len(partner), 1)  # made once
            self.assertIn('partner species=1 name=Froakie level=5 ', partner[0])
            wild = self.lines(out, 'wild ')[0]
            self.assertIn('wild species=2 name=Bunnelby level=3 ', wild)
            self.assertTrue(wild.endswith(' moves=2,1,0,0'), wild)  # Growl, then Pound at level 3
            # Seeds decide; the same seed gives the same Pokémon.
            again = self.run_viewer(root, *common, '--steps', 'RRRR', '--inspect').stdout
            self.assertEqual(self.lines(again, 'wild '), self.lines(out, 'wild '))
            other = self.run_viewer(root, *common, '--seed', 7, '--steps', 'RRRR', '--inspect').stdout
            self.assertNotEqual(self.lines(other, 'wild '), self.lines(out, 'wild '))
            none = self.run_viewer(root, '--zone', 0, '--partner', 1, '--encounter-rate', 0, '--steps', 'RRRR',
                                   '--inspect').stdout
            self.assertNotIn('encounter ', none)
            for arguments in (('--encounter-rate', 101), ('--partner', 0), ('--partner-level', 101),
                              ('--seed', 1, '--seed', 2), ('--play', 'RX')):
                with self.subTest(arguments=arguments):
                    self.run_viewer(root, '--zone', 0, *arguments, '--inspect', success=False)
            # A zone without wild Pokémon: grass encounters nothing.
            write_battle_romfs(root, rate_zone=False)
            out = self.run_viewer(root, *common, '--steps', 'RRRR', '--inspect').stdout
            self.assertNotIn('encounter ', out)

    def test_battle_in_display(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_battle_romfs(root)
            common = ('--zone', 0, '--partner', 1, '--encounter-rate', 100, '--software')
            # Two steps onto grass, then: two intro pages, FIGHT, Pound, and
            # pages until the battle ends.
            play = 'RR' + '..' + ('..' + '.' * 8) * 12
            result = self.run_viewer(root, *common, '--play', play, '--frames', 400)
            self.assertIn('battle turn=1 message="Froakie used Pound!"\n', result.stdout)
            end = self.lines(result.stdout, 'battle_end ')
            self.assertEqual(len(end), 1, result.stdout)
            self.assertIn('battle_end result=won ', end[0])
            # Running from the command menu (cursor right): the faster partner escapes.
            run = self.run_viewer(root, *common, '--play', 'RR..R..', '--frames', 200).stdout
            self.assertIn('battle turn=1 message="Got away safely!"\n', run)
            self.assertIn('battle_end result=escaped turns=1 ', run)
            # The moves menu cancels back to the command menu ('x').
            cancel = self.run_viewer(root, *common, '--play', 'RR...xR..', '--frames', 200).stdout
            self.assertIn('battle_end result=escaped turns=1 ', cancel)
            # The battle screen replaces the field while it lasts.
            shots = {}
            for name, frame in (('field', 5), ('battle', 60)):
                capture = root / f'{name}.bmp'
                self.run_viewer(root, *common, '--play', 'RR', '--frames', frame + 1, '--capture-frame', capture,
                                '--capture-at', frame)
                shots[name] = capture.read_bytes()
            sky = bmp_pixel(shots['battle'], 400, SKY_ROW)
            self.assertGreater(sky[2], 200)  # blue sky
            self.assertNotEqual(bmp_pixel(shots['field'], 400, SKY_ROW), sky)


if __name__ == '__main__':
    unittest.main()
