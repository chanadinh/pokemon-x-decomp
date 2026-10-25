"""Black-box checks of --save / --load: the viewer save file written from the
display loop and read back, on the synthetic RomFS of the battle viewer tests
and a synthetic code.bin (a block-order table and neutral natures). No game
data."""
import itertools
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_battle_viewer import write_battle_romfs
from test_model_viewer import VIEWER

SIZE = 36 + 6 * 0x104


def write_code_bin(path):
    code = bytearray(0x470f6c + 125)
    rows = [bytes(p) for p in itertools.permutations(range(4))]
    code[0x470eec:0x470eec + 128] = b''.join(rows + rows[:8])
    path.write_bytes(bytes(code))


def decrypt(slot):
    """One stored record decoded independently: XOR stream, checksum."""
    key = struct.unpack_from('<I', slot, 0)[0]
    out, k, total = bytearray(slot[:8]), key, 0
    for i in range(8, 0xe8, 2):
        k = (k * 0x41c64e6d + 0x6073) & 0xffffffff
        h = struct.unpack_from('<H', slot, i)[0] ^ (k >> 16)
        out += struct.pack('<H', h)
        total += h
    return key, bytes(out), (total & 0xffff) == struct.unpack_from('<H', slot, 6)[0]


def extension_plain(key, data):
    """The party extension: the same XOR stream, restarted from the key."""
    out, k = bytearray(), key
    for i in range(0, len(data), 2):
        k = (k * 0x41c64e6d + 0x6073) & 0xffffffff
        out += struct.pack('<H', struct.unpack_from('<H', data, i)[0] ^ (k >> 16))
    return bytes(out)


class BCHSaveViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run([str(VIEWER), '--romfs', str(root), *map(str, arguments)], text=True,
                                capture_output=True, env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=120)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def lines(self, text, prefix):
        return [line for line in text.split('\n') if line.startswith(prefix)]

    def test_save_and_load(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_battle_romfs(root)
            code = root / 'code.bin'
            write_code_bin(code)
            save = root / 'game.sav'
            common = ('--partner', 1, '--encounter-rate', 100, '--code-bin', code, '--software')
            # Option contract.
            self.run_viewer(root, '--zone', 0, '--save', save, '--inspect', success=False)  # no code.bin
            self.run_viewer(root, '--zone', 0, '--load', save, '--code-bin', code, '--inspect', success=False)
            self.run_viewer(root, '--load', save, '--code-bin', code, '--partner', 1, '--inspect', success=False)
            self.run_viewer(root, '--load', save, '--code-bin', code, '--inspect', success=False)  # missing file
            # Saving where the walk starts makes the partner and writes the file.
            out = self.run_viewer(root, '--zone', 0, *common, '--save', save, '--play', 's', '--frames', 5).stdout
            self.assertIn(f'save path={save} zone=0 world_tile=20,21 facing=0 party=1 balls=10 bytes={SIZE}\n', out)
            self.assertEqual(len(self.lines(out, 'partner ')), 1)
            data = save.read_bytes()
            self.assertEqual(len(data), SIZE)
            self.assertEqual(data[:8], b'PXSV' + struct.pack('<HH', 1, 1))
            key, plain, ok = decrypt(data[36:36 + 0xe8])
            self.assertTrue(ok)
            self.assertNotEqual(key, 0)
            rows = list(itertools.permutations(range(4)))
            rows += rows[:8]
            block_a = rows[(key >> 13) & 31][0]
            self.assertEqual(struct.unpack_from('<H', plain, 8 + 0x38 * block_a)[0], 1)  # species
            extension = extension_plain(key, data[36 + 0xe8:36 + 0x104])
            self.assertEqual(extension[4], 5)  # the party level
            # Loading puts the walker back and restores the party.
            out = self.run_viewer(root, '--load', save, '--code-bin', code, '--inspect').stdout
            self.assertIn('load zone=0 world_tile=20,21 cell=20,21 facing=0 party=1 balls=10\n', out)
            self.assertRegex(out, r'party_member slot=0 species=1 level=5 exp=125 hp=(\d+)/\1 moves=1,0,0,0 pp=35,0,0,0\n')
            # A step and a turn are kept; continuing after loading repeats the
            # saved run exactly (the random state is in the file).
            run = self.run_viewer(root, '--zone', 0, *common, '--save', save, '--play', 'RsRR', '--frames', 60).stdout
            self.assertIn('world_tile=21,21 facing=1 party=1', run)
            resumed = self.run_viewer(root, '--load', save, '--code-bin', code, '--software', '--encounter-rate',
                                      100, '--play', 'RR', '--frames', 60).stdout
            self.assertIn('load zone=0 world_tile=21,21 cell=21,21 facing=1 party=1 balls=10\n', resumed)
            self.assertEqual(self.lines(resumed, 'wild '), self.lines(run, 'wild '))
            self.assertEqual(self.lines(resumed, 'encounter '), self.lines(run, 'encounter '))
            # F5 / s during a battle saves once back on the field: here after a catch.
            throw = 'RR' + 's' + '..' + ('R.' + '.x' * 10) * 10
            caught = self.run_viewer(root, '--zone', 0, *common, '--save', save, '--play', throw,
                                     '--frames', 600).stdout
            end = self.lines(caught, 'battle_end ')[0]
            self.assertIn('result=caught', end)
            saved = self.lines(caught, 'save ')
            self.assertEqual(len(saved), 1)
            self.assertIn(' party=2 ', saved[0])
            out = self.run_viewer(root, '--load', save, '--code-bin', code, '--inspect').stdout
            self.assertIn(' party=2 balls=', out)
            self.assertRegex(out, r'party_member slot=1 species=2 level=3 ')
            # With two in the party, POKéMON switches (a turn): from the loaded file.
            switch = 'RR' + '..' + 'RR.' + 'D.' + '....'
            out = self.run_viewer(root, '--load', save, '--code-bin', code, '--software', '--encounter-rate', 100,
                                  '--play', switch, '--frames', 120).stdout
            self.assertIn('battle turn=1 message="Come back, Froakie!"\n', out)
            self.assertIn('battle turn=1 message="Go, Bunnelby!"\n', out)
            self.assertIn('battle_model side=partner species=2 model=none\n', out)
            # A damaged record is refused.
            damaged = bytearray(save.read_bytes())
            damaged[36 + 0x50] ^= 1
            save.write_bytes(bytes(damaged))
            failed = self.run_viewer(root, '--load', save, '--code-bin', code, '--inspect', success=False)
            self.assertIn('checksum', failed.stderr)


if __name__ == '__main__':
    unittest.main()
