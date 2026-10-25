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
from test_bch_zone_viewer import NPC, character_pack, entities, lit_skinned_bch, write_romfs
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


def personal(stats, kind, catch_rate=0, base_exp=0):
    entry = bytearray(0x40)
    entry[0:6] = bytes(stats)
    entry[6] = entry[7] = kind
    entry[8] = catch_rate
    entry[0x22:0x24] = struct.pack('<H', base_exp)
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


def motion_pack():
    """The GF1 pack of the zone fixture's character (slots 1, 3 and 4 over the
    bone Arm), taken from its MM section 1."""
    pack = character_pack()
    start, end = struct.unpack_from('<II', pack, 8)
    return bytes(pack[start:end])


def write_models(root, motions=False):
    """a/0/0/7: the species index (species 1 -> model 0, species 2 -> model 1),
    then eight files per model. Model 0 is the skinned fixture with its own
    textures (PC and PT packs) and, with motions, a PB pack (+5) holding
    motion_pack(); model 1's PC pack is empty, so species 2 keeps its
    placeholder."""
    index = struct.pack('<HBBHBB', 0, 1, 1, 1, 1, 1)
    model = [minipack(b'PC', [lit_skinned_bch()]), b'\0', minipack(b'PT', [lit_skinned_bch()]), b'\0', b'\0',
             minipack(b'PB', [motion_pack()]) if motions else b'\0', b'\0', b'\0']
    empty = [minipack(b'PC', [b'']), b'\0', minipack(b'PT', [b''])] + [b'\0'] * 5
    (root / 'a/0/0/7').write_bytes(garc([index, b'\0', b'\0', b'\0'] + model + empty))


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
    people = [personal([1] * 6, 0), personal([41, 56, 40, 71, 62, 44], 10), personal([38, 36, 38, 57, 32, 36], 0, 255, 255)]
    (root / 'a/2/1').mkdir(parents=True, exist_ok=True)
    (root / 'a/2/1/8').write_bytes(garc(people + [b''.join(people)]))
    (root / 'a/2/1/2').write_bytes(garc([move(0, 0, 0, 0, 0), move(0, 1, 40, 100, 35),
                                         move(0, 0, 0, 100, 40, target=5, stat=1, stage=-1)]))
    # Growth group 0 (every species here): exp = level cubed.
    cubes = struct.pack('<101I', *(l ** 3 for l in range(101)))
    (root / 'a/2/1/7').write_bytes(garc([cubes] + [bytes(404)] * 7))
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
                          'names=3,3,2 growth=a/2/1/7:8 learnsets=3 balls=10 '
                          'formulas=reconstructed-stats-and-levels,community-damage-and-catch\n', out)
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
            # Lv5 (125 exp) beats a Lv3 foe with base exp 255: +109, Lv6 at 216.
            self.assertIn('Froakie gained 109 Exp. Points!', result.stdout)
            self.assertIn('battle turn=', result.stdout)
            self.assertIn('Froakie grew to Lv. 6!', result.stdout)
            self.assertIn(' level=6 exp=234 ', end[0])
            # Running from the command menu (cursor right): the faster partner escapes.
            run = self.run_viewer(root, *common, '--play', 'RR..RR..', '--frames', 200).stdout
            self.assertIn('battle turn=1 message="Got away safely!"\n', run)
            self.assertIn('battle_end result=escaped turns=1 ', run)
            # The moves menu cancels back to the command menu ('x').
            cancel = self.run_viewer(root, *common, '--play', 'RR...xRR..', '--frames', 200).stdout
            self.assertIn('battle_end result=escaped turns=1 ', cancel)
            # Throwing balls (cursor on BALL) until one holds; '.x' pads safely:
            # 'x' is ignored by pages and the command menu and cancels FIGHT.
            throw = 'RR' + '..' + ('R.' + '.x' * 10) * 10
            caught = self.run_viewer(root, *common, '--balls', 5, '--play', throw, '--frames', 600).stdout
            self.assertIn('You threw a Poké Ball!', caught)
            # Leftover 'R's walk on into more grass; the first battle is checked.
            end = self.lines(caught, 'battle_end ')
            self.assertGreaterEqual(len(end), 1, caught)
            self.assertIn('result=caught', end[0])
            self.assertIn('party count=2 0:Froakie:Lv5:', caught)
            self.assertIn(' 1:Bunnelby:Lv3:', caught)
            self.assertNotIn('party count=7', caught)  # six at most
            empty = self.run_viewer(root, *common, '--balls', 0, '--play', 'RR..R.' + '.x' * 4, '--frames', 200)
            self.assertIn('You have no Poké Balls left!', empty.stdout)
            self.run_viewer(root, *common, '--balls', 1000, '--steps', 'R', '--inspect', success=False)
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
            # Without a/0/0/7 both sides are placeholders; with it the
            # partner's model replaces its circle, the wild one keeps it.
            out = self.run_viewer(root, *common, '--play', 'RR', '--frames', 61).stdout
            self.assertIn('battle_model side=partner species=1 model=none\n', out)
            write_models(root)
            out = self.run_viewer(root, *common, '--play', 'RR', '--frames', 61, '--capture-frame',
                                  root / 'model.bmp', '--capture-at', 60).stdout
            self.assertRegex(out, r'battle_model side=partner species=1 model=0 entry=4 texture_entry=6 '
                                  r'meshes=1 vertices=\d+ pose=rest\n')
            self.assertIn('battle_model side=wild species=2 model=1 entry=12 failed\n', out)
            model = (root / 'model.bmp').read_bytes()
            # Window pixels are twice the 400 x 240 frame: the partner's circle
            # centre (100, 140) and the wild one's (300, 82).
            self.assertNotEqual(bmp_pixel(model, 200, 280), bmp_pixel(shots['battle'], 200, 280))
            self.assertEqual(bmp_pixel(model, 600, 164), bmp_pixel(shots['battle'], 600, 164))
            # With a PB pack (+5) the model idles on slot 1; its other slots are listed.
            write_models(root, motions=True)
            out = self.run_viewer(root, *common, '--play', 'RR', '--frames', 61).stdout
            self.assertRegex(out, r'battle_model side=partner species=1 model=0 entry=4 texture_entry=6 '
                                  r'meshes=1 vertices=\d+ motion_entry=9 motions=3 slots=1:2,3:4,4:4 pose=idle\n')
            won = self.run_viewer(root, *common, '--play', play, '--frames', 400).stdout
            self.assertIn('battle_end result=won ', won)


if __name__ == '__main__':
    unittest.main()
