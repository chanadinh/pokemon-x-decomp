"""Black-box checks of the model viewer's --walk inspection marker, using a
hand-authored GR pack: terrain grid, BCH model and collision triangles."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_textured_viewer import textured_fixture
from test_bch_viewer import texture_pack
from test_field_viewer import collision, minipack
from test_model_viewer import VIEWER
from test_viewer import archive, bmp_pixel

# Cells near the fixture triangle; centers are (cell + 0.5) * 18 - 360.
WALKABLE = ((20, 21), (21, 21), (21, 22), (22, 22))
FLOOR = [(0, -14, 0), (40, -14, 0), (0, -14, 40)]
# Marker face colors: (0.96, 0.33, 0.25) x shade {1, 0.45, 0.7, 0.85}, rounded.
MARKER = {tuple(round(255 * c * s) for c in (0.96, 0.33, 0.25)) for s in (1, 0.45, 0.7, 0.85)}


def gr_pack(walkable=WALKABLE, magic=b'GR'):
    words = [0x01000021] * 1600
    for x, z in walkable:
        words[z * 40 + x] = 0x20
    words[21 * 40 + 23] = 0x01000020  # unknown full word: excluded by policy
    tiles = struct.pack('<HH', 40, 40) + struct.pack('<1600I', *words)
    return minipack(magic, [tiles, textured_fixture().finish(), collision(FLOOR), b'', b'', b''], 128)


class BCHWalkViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run(
            [str(VIEWER), '--romfs', str(root), '--archive', 'field.garc', '--entry', '0', '--section', '1',
             '--triangle-list', '--textured', '--texture-archive', 'textures.garc', '--texture-entry', '0',
             *map(str, arguments)], text=True, capture_output=True,
            env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=15)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def write(self, root, pack=None):
        (root / 'field.garc').write_bytes(archive(gr_pack() if pack is None else pack))
        (root / 'textures.garc').write_bytes(archive(texture_pack()))

    def marker_pixels(self, root, *arguments):
        capture = root / 'walk.bmp'
        if capture.exists():
            capture.unlink()
        result = self.run_viewer(root, '--software', '--frames', 1, '--capture-frame', capture, *arguments)
        data = capture.read_bytes()
        count = sum(bmp_pixel(data, x, y) in MARKER for y in range(0, 640, 2) for x in range(0, 960, 2))
        return result, count

    def test_start_steps_height_and_frame(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root)
            start = self.run_viewer(root, '--walk', '--inspect').stdout
            # Exactly one shading summary: the walk block must not detach the
            # CGFX branch from the BCH one (regression in the first walk commit).
            self.assertEqual(start.count('\nmode='), 1)
            self.assertEqual(self.run_viewer(root, '--inspect').stdout.count('\nmode='), 1)
            self.assertIn('walk policy=exact-cell-0x00000020 movement=inspection-not-game-rules grid=40x40 '
                          'standard_cells=4 collision_vertices=3 marker=cell:20,21 local=9,27 height=-14 '
                          'moves=0 blocked=0\n', start)
            # R (21,21); D (21,22); R (22,22); U blocked; R blocked (unknown word
            # rows differ); L (21,22); U (21,21); L (20,21); L blocked.
            moved = self.run_viewer(root, '--walk', '--steps', 'RDRURLULL', '--inspect').stdout
            self.assertIn('marker=cell:20,21 local=9,27 height=-14 moves=6 blocked=3\n', moved)
            outside = self.run_viewer(root, '--walk', '--steps', 'RDR', '--inspect').stdout
            self.assertIn('marker=cell:22,22 local=45,45 height=none:0 moves=3 blocked=0\n', outside)
            result, count = self.marker_pixels(root, '--walk', '--self-test-input')
            self.assertIn('Input self-test passed: orbit, zoom, reset, Escape, WASD.', result.stderr)
            self.assertGreater(count, 50)
            _, without = self.marker_pixels(root)
            self.assertEqual(without, 0)

    def test_no_marker_and_failures(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root, gr_pack(walkable=()))
            none = self.run_viewer(root, '--walk', '--steps', 'UD', '--inspect').stdout
            self.assertIn('standard_cells=0 collision_vertices=3 marker=none\n', none)
            self.write(root)
            for arguments in (('--steps', 'U'), ('--walk', '--steps', 'UX'), ('--walk', '--steps', 'U', '--steps', 'D')):
                with self.subTest(arguments=arguments):
                    self.run_viewer(root, *arguments, '--inspect', success=False)
            no_section = subprocess.run(
                [str(VIEWER), '--romfs', str(root), '--archive', 'field.garc', '--entry', '0',
                 '--triangle-list', '--textured', '--texture-archive', 'textures.garc', '--texture-entry', '0',
                 '--walk', '--inspect'], text=True, capture_output=True, timeout=15)
            self.assertNotEqual(no_section.returncode, 0)
            self.write(root, gr_pack(magic=b'AD'))
            result = self.run_viewer(root, '--walk', '--software', '--frames', 1, success=False)
            self.assertIn('--walk GR terrain', result.stderr)
            self.assertNotIn('Video=', result.stderr)
            help_text = subprocess.run([str(VIEWER), '--help'], text=True, capture_output=True,
                                       check=True, timeout=15).stdout
            self.assertIn('--walk', help_text)
            self.assertIn('--steps UDLR', help_text)


if __name__ == '__main__':
    unittest.main()
