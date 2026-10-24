"""Black-box checks of --motion-section/--motion/--motion-frame/--walk-motion
with a hand-authored skinned BCH and GF1 motion pack in one MM pack."""
import hashlib
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_model import float_words, packet
from test_bch_player_viewer import add_texture
from test_bch_textured_viewer import textured_fixture
from test_bch_viewer import texture_pack
from test_bch_walk_viewer import gr_pack
from test_field_viewer import minipack
from test_model_viewer import VIEWER
from test_viewer import archive


def skinned_bch():
    """The textured triangle, skinned like assets/test_bch_model.c's fixture:
    bones Origin (root) and Arm (translation 1, 2, 3; inverse bind its
    inverse), one smooth submesh with palette (0, 1), u8 x2 indices and
    weights (scale 0.5): vertex 0 on Arm, vertex 1 half and half, vertex 2
    on Origin. Position scale and offset keep the triangle within a walk cell."""
    f = textured_fixture()
    add_texture(f)
    model, mesh, sub = f.labels['model'], f.labels['mesh'], f.labels['sub']
    f.relocations.pop(f.labels['enable'] + 16)
    skin = [(1, 0, 2, 0), (0, 1, 1, 1), (0, 0, 2, 0)]
    xyz_uv = [(-2, 3, 5, .5, -1), (7, -4, 2, 2, 3), (-1, 9, -6, -2, 4)]
    vertices = f.alloc('raw_data', b''.join(struct.pack('<5f', *p) + bytes(s) for p, s in zip(xyz_uv, skin)))
    enable = (packet(0x200, 0x03000000, 0x557B, 0x30000000, 0, 0x3210, 0x40180000, consecutive=True)
              + packet(0x2BB, 0x8740, 0, consecutive=True)
              + packet(0x242, 3, mask=1)
              + packet(0x2C0, 0x80000006, *float_words(0, 0, 6, 0), consecutive=True)
              + packet(0x2C0, 0x80000007)
              + packet(0x2C1, *float_words(1, .5, .5, .5, .5, 1, 1, .25)))
    enabled = f.stream(mesh + 8, enable)
    f.pointer(enabled + 16, vertices, 0x26)
    names = [f.text(b'Origin'), f.text(b'Arm')]
    bones = f.alloc('contents', 200)
    f.data[model] = 2
    f.data[model + 1] = 1
    f.pointer(model + 0x70, bones, 0)
    f.word(model + 0x74, 2)
    f.pointer(model + 0x78, f.tree(names), 0)
    for b in range(2):
        at = bones + b * 100
        f.word(at, 0x2800000)
        f.word(at + 4, 0 if b else 0xffff)
        struct.pack_into('<3f', f.data, at + 8, 1, 1, 1)
        struct.pack_into('<3f', f.data, at + 32, *((1, 2, 3) if b else (0, 0, 0)))
        struct.pack_into('<12f', f.data, at + 44, 1, 0, 0, -b, 0, 1, 0, -2 * b, 0, 0, 1, -3 * b)
        f.pointer(at + 92, names[b], 1)
    f.data[sub] = 1
    f.data[sub + 2] = 2
    f.data[sub + 6] = 1
    return f.finish()


def gf1_pack():
    """Slots: 0 skeleton (Origin, Arm), 1 motion of 4 frames moving Arm in x
    from 1 to 5 (linear), 2 empty, 3 motion of 2 frames holding Arm x at 1."""
    def motion(codes, frames, lists, values):
        out = bytearray(struct.pack('<HH', len(codes), frames))
        for i in range(0, len(codes), 8):
            word = sum(c << (3 * j) for j, c in enumerate(codes[i:i + 8]))
            out += struct.pack('<I', word)[:3]
        for keys in lists:
            out += bytes([len(keys)] + keys)
        out += bytes((-len(out)) % 4)
        return bytes(out + struct.pack('<%df' % len(values), *values))
    skeleton = bytes([2, 1, 0, 0, 0]) + b'Arm\0'
    skeleton += bytes((-len(skeleton)) % 4) + struct.pack('<14f', 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 0, 0, 0, 1)
    # Origin skips three groups; Arm TX linear (keys 0, 4), the rest absent.
    walk = motion([0, 0, 1, 1, 1, 6], 4, [[]], [1, 5])
    hold = motion([0, 0, 1, 1, 1, 5], 2, [], [1])
    table = 4 + 4 * 5
    offsets = [table, table + len(skeleton), 0, table + len(skeleton) + len(walk)]
    offsets.append(offsets[3] + len(hold))
    return struct.pack('<I5I', 4, *offsets) + skeleton + walk + hold


def mm_pack(motions=True):
    return minipack(b'MM', [skinned_bch(), gf1_pack() if motions else b'\0' * 8])


class BCHMotionViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run([str(VIEWER), '--romfs', str(root), *map(str, arguments)], text=True,
                                capture_output=True, env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=15)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def standalone(self, root, *arguments, success=True):
        return self.run_viewer(root, '--archive', 'player.garc', '--entry', 0, '--section', 0, '--skinned',
                               '--triangle-list', '--textured', '--texture-archive', 'player.garc',
                               '--texture-entry', 0, '--texture-section', 0, *arguments, success=success)

    def walk(self, root, *arguments, success=True):
        return self.run_viewer(root, '--archive', 'field.garc', '--entry', 0, '--section', 1, '--triangle-list',
                               '--textured', '--texture-archive', 'textures.garc', '--texture-entry', 0, '--walk',
                               '--player-archive', 'player.garc', '--player-entry', 0, '--player-section', 0,
                               *arguments, success=success)

    def write(self, root, motions=True):
        (root / 'player.garc').write_bytes(archive(mm_pack(motions)))
        (root / 'field.garc').write_bytes(archive(gr_pack()))
        (root / 'textures.garc').write_bytes(archive(texture_pack()))

    def capture(self, run, root, *arguments):
        path = root / 'frame.bmp'
        if path.exists():
            path.unlink()
        run(root, *arguments, '--software', '--frames', 1, '--capture-frame', path)
        return hashlib.sha256(path.read_bytes()).hexdigest()

    def motion_line(self, text):
        return [line for line in text.split('\n') if line.startswith('motion_section=')][0]

    def test_standalone_frames(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root)
            lines = {}
            for frame in (0, 2, 4):
                lines[frame] = self.motion_line(self.standalone(root, '--motion-section', 1, '--motion', 1,
                                                                '--motion-frame', frame, '--inspect').stdout)
            self.assertIn('motion_section=1 motion=1 frames=4 pack_slots=4 pack_motions=2 motion_bones=2 '
                          'model_bones=2 matched=2 world_space=0 start_frame=0 ', lines[0])
            self.assertIn(' attach=none residual=0', lines[0])
            # Decoded x: vertex 0 (on Arm) -1, vertex 1 (half) 3.5, vertex 2
            # (Origin) -0.5. Arm moves +1 per frame in x: at frame 2 vertex 0
            # moves 2 (to 1) and vertex 1 moves 1; y and z are unchanged.
            self.assertIn(' posed_min=-1.0000,4.0000,-3.0000 posed_max=3.5000,10.5000,2.5000 ', lines[0])
            self.assertIn(' posed_min=-0.5000,4.0000,-3.0000 posed_max=4.5000,10.5000,2.5000 ', lines[2])
            # Motions loop: frame 4 of 4 is frame 0 again.
            self.assertEqual(lines[4].replace('start_frame=4', 'start_frame=0'), lines[0])
            first = self.capture(self.standalone, root, '--motion-section', 1, '--motion', 1)
            again = self.capture(self.standalone, root, '--motion-section', 1, '--motion', 1, '--motion-frame', 0)
            later = self.capture(self.standalone, root, '--motion-section', 1, '--motion', 1, '--motion-frame', 2)
            looped = self.capture(self.standalone, root, '--motion-section', 1, '--motion', 1, '--motion-frame', 4)
            self.assertEqual(first, again)
            self.assertEqual(first, looped)
            self.assertNotEqual(first, later)
            hold = self.motion_line(self.standalone(root, '--motion-section', 1, '--motion', 3, '--inspect').stdout)
            self.assertIn(' posed_min=-1.0000,4.0000,-3.0000 posed_max=3.5000,10.5000,2.5000 ', hold)

    def test_walk_cycle(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root)
            motion = ('--motion-section', 1, '--motion', 3, '--walk-motion', 1)
            inspect = self.walk(root, *motion, '--steps', 'R', '--inspect').stdout
            self.assertIn('walk_motion=1 frames=4 playback=one-cycle-per-step glide=linear-over-cycle\n', inspect)
            self.assertIn('marker=cell:21,21', inspect)
            start = self.capture(self.walk, root, *motion, '--steps', 'R', '--motion-frame', 0)
            middle = self.capture(self.walk, root, *motion, '--steps', 'R', '--motion-frame', 2)
            done = self.capture(self.walk, root, *motion, '--steps', 'R', '--motion-frame', 4)
            self.assertEqual(len({start, middle, done}), 3)
            # A blocked last step plays no cycle: --motion-frame then has no effect.
            blocked = [self.capture(self.walk, root, *motion, '--steps', 'RDRU', '--motion-frame', f) for f in (0, 2)]
            self.assertEqual(blocked[0], blocked[1])
            result = self.capture(self.walk, root, *motion, '--self-test-input')
            self.assertTrue(result)

    def test_failures_and_contract(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root)
            for arguments in (('--motion', 1), ('--motion-section', 1), ('--motion-section', 1, '--motion', 0),
                              ('--motion-section', 1, '--motion', 1, '--motion', 1),
                              ('--motion-section', 1, '--motion', 1, '--walk-motion', 1),
                              ('--motion-section', 1, '--motion-frame', 1),
                              ('--motion-section', 256, '--motion', 1)):
                with self.subTest(arguments=arguments):
                    self.standalone(root, *arguments, '--inspect', success=False)
            # --motion needs a skinned model.
            self.run_viewer(root, '--archive', 'player.garc', '--entry', 0, '--section', 0, '--triangle-list',
                            '--motion-section', 1, '--motion', 1, '--inspect', success=False)
            empty = self.standalone(root, '--motion-section', 1, '--motion', 2, '--inspect', success=False)
            self.assertIn('slot 2 is absent or empty', empty.stderr)
            beyond = self.standalone(root, '--motion-section', 1, '--motion', 1, '--motion-frame', 5, '--inspect',
                                     success=False)
            self.assertIn('exceeds', beyond.stderr)
            self.walk(root, '--motion-section', 1, '--motion', 3, '--walk-motion', 2, '--inspect', success=False)
            self.write(root, motions=False)
            destination = root / 'never.bmp'
            bad = self.standalone(root, '--motion-section', 1, '--motion', 1, '--software', '--frames', 1,
                                  '--capture-frame', destination, success=False)
            self.assertIn('GF1 motion pack', bad.stderr)
            self.assertNotIn('Video=', bad.stderr)
            self.assertFalse(destination.exists())
            help_text = subprocess.run([str(VIEWER), '--help'], text=True, capture_output=True,
                                       check=True, timeout=15).stdout
            for option in ('--motion-section N', '--motion N', '--motion-frame N', '--walk-motion N'):
                self.assertIn(option, help_text)


if __name__ == '__main__':
    unittest.main()
