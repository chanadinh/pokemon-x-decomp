"""Black-box checks of directional fragment lighting in the BCH combiner
preview, using hand-authored model and environment fixtures only."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_combiner_viewer import PASS, combiner_fixture
from test_bch_model import Fixture, packet
from test_bch_textured_viewer import BACKGROUND
from test_bch_viewer import minipack, texture_pack
from test_model_viewer import VIEWER
from test_viewer import archive, bmp_pixel

# Stage 0 replaces with fragment primary (source 1) for RGB and alpha, so each
# covered pixel shows the lit fragment primary color.
LIT = [(0x00010001, 0, 0, 0xff000000, 0)] + [PASS] * 5
# float24 (x, y, z, w) = (0, 1, 0, 0), packed w-first in three words.
UP = (0, 0x3f00, 0)
# Expected bytes, computed separately in numpy float32 (see the reproduction
# record): register products with material diffuse (255, 128, 255), light
# vector -direction, N = (0, 1, 0), plus material emission (5, 0, 0). The
# lamp diffuse G of 4 becomes register ((2*4+1)*(2*128+1))>>10 = 2.
DAY = (75, 80, 180)
NIGHT = (75, 28, 47)


def lit_fixture(flags=1, light_set=2, normal=True, emission=(5, 0, 0, 0)):
    f = combiner_fixture(LIT)
    params = f.labels['params']
    f.word(params + 4, flags)
    struct.pack_into('<HH', f.data, params + 84, light_set, 0)
    colors = bytes(emission) + bytes((255, 255, 255, 255, 255, 128, 255, 255)) + bytes(8)
    f.data[params + 88:params + 88 + len(colors)] = colors
    if normal:
        # Override the loader: three attributes, fixed slot 2 = semantic 1.
        extra = (packet(0x201, 0x7B, 0x20040000, consecutive=True)
                 + packet(0x2BB, 0x140, 0, consecutive=True) + packet(0x242, 2, mask=1)
                 + packet(0x232, 2) + packet(0x233, *UP, consecutive=True))
        enable = f.labels['enable']
        start = enable
        words = struct.unpack_from('<I', f.data, f.labels['mesh'] + 12)[0]
        body = bytes(f.data[start:start + words * 4]) + extra
        field = f.labels['mesh'] + 8
        vertex_field = enable + 16
        relocation = f.relocations.pop(vertex_field)
        target = struct.unpack_from('<I', f.data, vertex_field)[0] + f.bases['raw_data']
        moved = f.stream(field, body)
        f.pointer(moved + 16, target, relocation)
    return f


def light(f, name, kind, ambient, diffuse, direction, flags=0, enabled=1):
    at = f.alloc('contents', 0x6c)
    f.pointer(at, name, 1)
    struct.pack_into('<9f', f.data, at + 4, 1, 1, 1, 0, 0, 0, 0, 0, 0)
    f.data[at + 40:at + 44] = bytes((kind, enabled, flags, 0))
    f.data[at + 48:at + 56] = bytes(ambient) + bytes(diffuse)
    struct.pack_into('<3f', f.data, at + 64, *direction)
    struct.pack_into('<2f', f.data, at + 84, 0, 1)
    return at


def environment(sets=((2, (b'light_sun', b'light_night', b'lamp')),), extra=()):
    f = Fixture()
    names = {}
    for name in (b'scene', b'light_sun', b'light_night', b'lamp') + tuple(e[0] for e in extra):
        names[name] = f.text(name)
    lights = [light(f, names[b'light_sun'], 9, (10, 20, 30, 255), (100, 200, 250, 255), (0, -0.6, -0.8)),
              light(f, names[b'light_night'], 9, (40, 10, 5, 255), (50, 60, 70, 255), (0.8, -0.6, 0)),
              light(f, names[b'lamp'], 10, (0, 0, 0, 255), (0, 4, 0, 255), (0, -1, 0), flags=2)]
    order = [b'light_sun', b'light_night', b'lamp']
    for name, kind in extra:
        lights.append(light(f, names[name], kind, (1, 1, 1, 255), (1, 1, 1, 255), (0, -1, 0)))
        order.append(name)
    f.dictionary(5, lights, [names[n] for n in order])
    scene = f.alloc('contents', 0x1c)
    f.pointer(scene, names[b'scene'], 1)
    table = f.alloc('contents', 12 * len(sets))
    for index, (number, members) in enumerate(sets):
        array = f.alloc('contents', 4 * len(members))
        for k, member in enumerate(members):
            f.pointer(array + 4 * k, names[member], 1)
        f.word(table + 12 * index, number)
        f.pointer(table + 12 * index + 4, array, 0)
        f.word(table + 12 * index + 8, len(members))
    f.pointer(scene + 0xc, table, 0)
    f.word(scene + 0x10, len(sets))
    f.dictionary(14, [scene], [names[b'scene']])
    return f.finish()


class BCHLightingViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True, environment_args=True):
        extra = (['--environment-archive', 'env.garc', '--environment-entry', '0',
                  '--environment-section', '1'] if environment_args else [])
        result = subprocess.run(
            [str(VIEWER), '--romfs', str(root), '--archive', 'model.garc', '--entry', '0',
             '--triangle-list', '--textured', '--combiners', '--texture-archive', 'textures.garc',
             '--texture-entry', '0', *extra, *map(str, arguments)], text=True, capture_output=True,
            env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=15)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def write(self, root, model, env=None):
        (root / 'model.garc').write_bytes(archive(model.finish()))
        (root / 'textures.garc').write_bytes(archive(texture_pack()))
        (root / 'env.garc').write_bytes(archive(minipack(environment() if env is None else env, b'BM')))

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

    def test_day_and_night_reach_the_frame(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root, lit_fixture())
            inspect = self.run_viewer(root, '--inspect').stdout
            self.assertIn('environment_archive=env.garc environment_entry=0 environment_subentry=0 '
                          'environment_compression=none environment_section=1', inspect)
            self.assertIn('fragment_primary=directional-lit fragment_secondary=black lighting=day '
                          'point_spot=bounded normals=interpolated-renormalized', inspect)
            self.assertIn('environment scene="scene" lights=3 light_sets=1', inspect)
            self.assertIn('mesh_normal mesh=0 source=fixed', inspect)
            self.assertIn('light_setup material=0 set=2 global_ambient=5,0,0 point_bound=0,2,0 '
                          'light="light_sun":ambient=10,20,30:diffuse=100,100,250:'
                          'vector=0,0.600000024,0.800000012\n', inspect)
            night = self.run_viewer(root, '--inspect', '--lighting', 'night').stdout
            self.assertIn('lighting=night', night)
            self.assertIn('light="light_night":ambient=40,10,5:diffuse=50,30,70:'
                          'vector=-0.800000012,0.600000024,0\n', night)
            result, seen = self.colors(root)
            self.assertIn('directional fragment lighting', result.stderr)
            self.assertGreater(sum(seen.values()), 1000)
            self.assertEqual(set(seen), {DAY})
            self.assertRegex(result.stdout, r'combiner_sensitivity material=0 frame=1 fragments=[1-9]\d* '
                                            r'projected_substitute_sensitive=0 secondary_sensitive=0 '
                                            r'point_bound_sensitive=[1-9]\d* point_bound_max_delta=3 '
                                            r'degenerate_normals=0')
            _, seen = self.colors(root, '--lighting', 'night')
            self.assertEqual(set(seen), {NIGHT})

    def test_failures_before_video_and_option_contract(self):
        cases = [
            ('lighting disabled', lit_fixture(flags=0), None),
            ('absent light set', lit_fixture(light_set=7), None),
            ('no normal', lit_fixture(normal=False), None),
            ('ambient light in set', lit_fixture(),
             environment(sets=((2, (b'light_sun', b'glow')),), extra=((b'glow', 2),))),
            ('hemisphere light', lit_fixture(), environment(extra=((b'sky', 3),))),
        ]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            destination = root / 'never.bmp'
            for label, model, env in cases:
                with self.subTest(case=label):
                    self.write(root, model, env)
                    result = self.run_viewer(root, '--software', '--frames', 1,
                                             '--capture-frame', destination, success=False)
                    self.assertNotIn('Video=', result.stderr)
                    self.assertFalse(destination.exists())
            self.write(root, lit_fixture())
            self.run_viewer(root, '--inspect', '--lighting', 'dusk', success=False)
            self.run_viewer(root, '--inspect', '--lighting', 'day', '--lighting', 'night', success=False)
            self.run_viewer(root, '--inspect', '--lighting', 'day', environment_args=False, success=False)
            self.run_viewer(root, '--inspect', '--environment-section', '0', success=False)  # not a pack
            base = [str(VIEWER), '--romfs', str(root), '--archive', 'model.garc', '--entry', '0',
                    '--triangle-list', '--textured', '--texture-archive', 'textures.garc',
                    '--texture-entry', '0', '--environment-archive', 'env.garc',
                    '--environment-entry', '0', '--inspect']
            no_combiners = subprocess.run(base, text=True, capture_output=True, timeout=15)
            self.assertNotEqual(no_combiners.returncode, 0)
            help_text = subprocess.run([str(VIEWER), '--help'], text=True, capture_output=True,
                                       check=True, timeout=15).stdout
            self.assertIn('--lighting day|night', help_text)
            self.assertIn('--environment-archive', help_text)
            # Without an environment, the declared unlit policy is unchanged.
            unlit = self.run_viewer(root, '--inspect', environment_args=False).stdout
            self.assertIn('fragment_primary=white', unlit)


if __name__ == '__main__':
    unittest.main()
