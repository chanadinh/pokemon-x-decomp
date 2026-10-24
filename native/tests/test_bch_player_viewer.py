"""Black-box checks of --player-archive (a BCH drawn at the --walk marker with
its own textures) and --skinned, using hand-authored fixtures only."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_model import packet
from test_bch_textured_viewer import BACKGROUND, ETC_COLORS, textured_fixture
from test_bch_viewer import texture_pack
from test_bch_walk_viewer import MARKER, gr_pack
from test_model_viewer import VIEWER
from test_viewer import archive, bmp_pixel, etc_texture


def add_texture(f, name=b'etc\n"'):
    """A category-3 ETC1 8x8 texture in the same BCH as the model."""
    named = f.text(name)
    obj = f.alloc('contents', 32)
    payload = f.alloc('raw_data', etc_texture(False)[:-40])
    for unit, (dim, address, form) in enumerate([(0x82, 0x85, 0x8e), (0x92, 0x95, 0x96), (0x9a, 0x9d, 0x9e)]):
        stream = struct.pack('<8I', 8 << 16 | 8, 0xf0000 | dim, 0, 0xf0000 | address, 12,
                             0xf0000 | form, 1, 0xf023d)
        cmd = f.stream(obj + unit * 8, stream)
        f.pointer(cmd + 8, payload, 0x24)
    f.word(obj + 24, 0x10c)
    f.pointer(obj + 28, named, 1)
    f.dictionary(3, [obj], [named])
    return f


def player_bch(texture=True):
    f = textured_fixture()
    if texture:
        add_texture(f)
    return f.finish()


class BCHPlayerViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True, walk=True):
        result = subprocess.run(
            [str(VIEWER), '--romfs', str(root), '--archive', 'field.garc', '--entry', '0', '--section', '1',
             '--triangle-list', '--textured', '--texture-archive', 'textures.garc', '--texture-entry', '0',
             *(['--walk'] if walk else []), *map(str, arguments)], text=True, capture_output=True,
            env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=15)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def write(self, root, player=None):
        (root / 'field.garc').write_bytes(archive(gr_pack()))
        (root / 'textures.garc').write_bytes(archive(texture_pack()))
        (root / 'player.garc').write_bytes(archive(player_bch() if player is None else player))

    def colors(self, root, *arguments):
        capture = root / 'frame.bmp'
        if capture.exists():
            capture.unlink()
        result = self.run_viewer(root, '--software', '--frames', 1, '--capture-frame', capture, *arguments)
        data = capture.read_bytes()
        seen = {}
        for y in range(0, 640, 2):
            for x in range(0, 960, 2):
                rgb = bmp_pixel(data, x, y)
                if rgb != BACKGROUND:
                    seen[rgb] = seen.get(rgb, 0) + 1
        return result, seen

    def test_player_replaces_the_box_and_follows_steps(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root)
            player = ('--player-archive', 'player.garc', '--player-entry', 0)
            inspect = self.run_viewer(root, *player, '--inspect').stdout
            self.assertIn('player_archive=player.garc player_entry=0 player_section=none:0\n', inspect)
            self.assertIn('marker=cell:20,21', inspect)
            # The player's own texture pack is the same BCH (mixed reader).
            self.assertNotIn('texture_archive=player.garc', inspect)
            result, seen = self.colors(root, *player, '--self-test-input')
            self.assertIn('WASD', result.stderr)
            self.assertFalse(set(seen) & MARKER)
            self.assertTrue(set(seen) & ETC_COLORS)
            _, box = self.colors(root)
            self.assertTrue(set(box) & MARKER)
            # The camera follows the marker, so a step changes the frame.
            _, moved = self.colors(root, *player, '--steps', 'R')
            self.assertNotEqual(moved, seen)

    def test_failures_and_contract(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root, player_bch(texture=False))
            destination = root / 'never.bmp'
            result = self.run_viewer(root, '--player-archive', 'player.garc', '--player-entry', 0,
                                     '--software', '--frames', 1, '--capture-frame', destination, success=False)
            self.assertIn('BCH textures', result.stderr)
            self.assertNotIn('Video=', result.stderr)
            self.assertFalse(destination.exists())
            self.write(root)
            for arguments in (('--player-archive', 'player.garc'), ('--player-entry', 0),
                              ('--player-archive', '../player.garc', '--player-entry', 0)):
                with self.subTest(arguments=arguments):
                    self.run_viewer(root, *arguments, '--inspect', success=False)
            self.run_viewer(root, '--player-archive', 'player.garc', '--player-entry', 0, '--inspect',
                            walk=False, success=False)
            help_text = subprocess.run([str(VIEWER), '--help'], text=True, capture_output=True,
                                       check=True, timeout=15).stdout
            self.assertIn('--player-archive', help_text)
            self.assertIn('--skinned', help_text)

    def test_skinned_flag_accepts_unskinned_models(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'model.garc').write_bytes(archive(player_bch()))
            base = [str(VIEWER), '--romfs', str(root), '--archive', 'model.garc', '--entry', '0',
                    '--triangle-list', '--inspect']
            plain = subprocess.run(base, text=True, capture_output=True, timeout=15)
            skinned = subprocess.run(base + ['--skinned'], text=True, capture_output=True, timeout=15)
            self.assertEqual(plain.returncode, 0, plain.stderr)
            self.assertEqual(skinned.returncode, 0, skinned.stderr)
            self.assertEqual(plain.stdout, skinned.stdout)


if __name__ == '__main__':
    unittest.main()
