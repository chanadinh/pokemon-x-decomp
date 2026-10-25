"""Black-box checks of the BCH six-stage combiner preview, synthetic fixtures only."""
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from test_bch_model import packet
from test_bch_textured_viewer import ETC_COLORS, BACKGROUND, textured_fixture
from test_bch_viewer import texture_pack
from test_model_viewer import VIEWER, cgfx_model
from test_viewer import archive, bmp_pixel

PASS = (0x0fff0fff, 0, 0, 0xff000000, 0)  # replace previous, RGB and alpha


def combiner_fixture(stages, buffer_color=0, buffer_update=0, **kwargs):
    """textured_fixture with a complete six-stage fragment stream."""
    f = textured_fixture(**kwargs)
    params = f.labels['params']
    stream = (packet(0x40, 2, mask=1) + packet(0x100, 0x100, mask=3)
              + packet(0x101, kwargs.get('blend', 0x01010000))
              + packet(0x104, kwargs.get('alpha', 0x10), mask=3)
              + packet(0x107, kwargs.get('depth', 0x1f51))
              + packet(0xe0, buffer_update, mask=2) + packet(0xfd, buffer_color))
    for base, stage in zip((0xc0, 0xc8, 0xd0, 0xd8, 0xf0, 0xf8), stages):
        stream += packet(base, *stage, consecutive=True)
    f.stream(params + 0xC8, stream)
    return f


def mul(a, b):
    return a * b // 255


class BCHCombinerViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True, textures=True, combiners=True):
        extra = ['--texture-archive', 'textures.garc', '--texture-entry', '0'] if textures else []
        result = subprocess.run(
            [str(VIEWER), '--romfs', str(root), '--archive', 'model.garc', '--entry', '0',
             '--triangle-list', '--textured', *(['--combiners'] if combiners else []),
             *extra, *map(str, arguments)], text=True, capture_output=True,
            env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=15)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def write(self, root, fixture, textures=None):
        (root / 'model.garc').write_bytes(archive(fixture.finish()))
        (root / 'textures.garc').write_bytes(archive(texture_pack() if textures is None else textures))

    def capture_colors(self, root, *arguments):
        capture = root / 'frame.bmp'
        if capture.exists():
            capture.unlink()
        result = self.run_viewer(root, '--software', '--frames', 1, '--capture-frame', capture, *arguments)
        data = capture.read_bytes()
        colors = {}
        for y in range(0, 640, 2):
            for x in range(0, 960, 2):
                rgb = bmp_pixel(data, x, y)
                if rgb != BACKGROUND:
                    colors[rgb] = colors.get(rgb, 0) + 1
        return result, colors

    def test_constant_buffer_and_scale_reach_the_frame(self):
        # Stage 0: texture 0. Stage 1: x constant (128, 64, 255) with 2x RGB
        # scale. Stage 0 writes the buffer; stage 3 (one-stage delay) reads it
        # back and adds the constant (16, 0, 0) from stage 3's own register.
        stages = [(0x00030003, 0, 0, 0xff000000, 0),
                  (0x0fff0fef, 0, 0x00000001, 0xffff4080, 1),
                  PASS,
                  (0x0fff0fed, 0, 0x00000002, 0x00000010, 0),
                  PASS, PASS]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root, combiner_fixture(stages, buffer_update=0x0100))
            inspect = self.run_viewer(root, '--inspect').stdout
            self.assertIn('shading=tev-six-stage primary=vertex-color fragment_primary=white '
                          'fragment_secondary=black', inspect)
            self.assertIn('mesh_color mesh=0 source=absent-white', inspect)
            self.assertIn('combiner material=0 sources=0xe009 sampled_units=0x1 projected_units=0x0 '
                          'disabled_units=0x0 buffer_update=0x0100 buffer_color=0x00000000 '
                          'unit0="etc\\x0a\\x22":0:wrap2,2:nearest', inspect)
            self.assertIn('tev material=0 stage=1 source=0x0fff0fef operand=0x00000000 '
                          'combine=0x00000001 constant=0xffff4080 scale=0x00000001', inspect)
            result, colors = self.capture_colors(root)
            self.assertIn('six-stage combiner', result.stderr)
            # Final = buffer (stage-0 texel) + (16, 0, 0); stage 1 is discarded.
            expected = {(min(255, r + 16), g, b) for r, g, b in ETC_COLORS}
            self.assertGreater(sum(colors.values()), 1000)
            self.assertLessEqual(set(colors), expected)
            self.assertGreaterEqual(len(colors), 3)
            self.assertRegex(result.stdout, r'combiner_sensitivity material=0 frame=1 fragments=[1-9]\d* '
                                            r'projected_substitute_sensitive=0 secondary_sensitive=0')

    def test_modulate_and_previous_chain(self):
        # tex0 * const(200, 100, 50), then + fragment secondary (black), x2.
        stages = [(0x00030003, 0, 0, 0xff000000, 0),
                  (0x0fff0fef, 0, 0x00000001, 0xff3264c8, 0),
                  (0x0fff0f2f, 0, 0x00000002, 0, 1),
                  PASS, PASS, PASS]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root, combiner_fixture(stages))
            result, colors = self.capture_colors(root)
            expected = {tuple(min(255, 2 * mul(c, k)) for c, k in zip(rgb, (200, 100, 50)))
                        for rgb in ETC_COLORS}
            self.assertLessEqual(set(colors), expected)
            self.assertGreaterEqual(len(colors), 3)
            sensitive = int(re.search(r'secondary_sensitive=(\d+)', result.stdout).group(1))
            self.assertGreater(sensitive, 0)  # stage 2 reads fragment secondary

    def test_projected_and_disabled_units(self):
        # Unit 0 is projection-mapped (substituted white; reported); unit 1
        # holds the image; unit 2 is disabled and therefore reads zero.
        # Stage 0: tex1 * tex0 (sensitive). Stage 1: + tex2 (zero).
        stages = [(0x00430043, 0, 0x00010001, 0xff000000, 0),
                  (0x0fff0f5f, 0, 0x00000002, 0xff000000, 0),
                  PASS, PASS, PASS, PASS]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            fixture = combiner_fixture(stages, names=(b'nope', b'etc\n"', None), unit_config=0x11003,
                                       packed=(0x00030001, 0xff000001, 1),
                                       sampler=(0x30002220, 0x2220, 0x2220))
            self.write(root, fixture)
            inspect = self.run_viewer(root, '--inspect').stdout
            self.assertIn('sampled_units=0x2 projected_units=0x1 disabled_units=0x4', inspect)
            self.assertIn('projected_units=white-substitute disabled_units=zero', inspect)
            result, colors = self.capture_colors(root)
            self.assertLessEqual(set(colors), ETC_COLORS)
            counts = re.search(r'fragments=(\d+) projected_substitute_sensitive=(\d+)', result.stdout)
            self.assertGreater(int(counts.group(1)), 0)
            self.assertEqual(counts.group(1), counts.group(2))

    def test_failures_before_video_and_option_contract(self):
        full = [(0x00030003, 0, 0, 0xff000000, 0)] + [PASS] * 5
        cases = [
            ('missing stage', combiner_fixture(full[:5])),
            ('reserved source', combiner_fixture([(0x00070007, 0, 0, 0, 0)] + [PASS] * 5)),
            ('texture 3', combiner_fixture([(0x00060006, 0, 0, 0, 0)] + [PASS] * 5)),
            ('bad operation', combiner_fixture([(0x00030003, 0, 10, 0, 0)] + [PASS] * 5)),
            ('unnamed enabled unit', combiner_fixture([(0x00040004, 0, 0, 0, 0)] + [PASS] * 5,
                                                      unit_config=0x11003)),
            ('mapping mode 4', combiner_fixture(full, packed=(0x00040001, 1, 1))),  # 1-3 are unexecuted units
            ('stage-0 only fixture', textured_fixture()),
        ]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            destination = root / 'never.bmp'
            for label, fixture in cases:
                with self.subTest(case=label):
                    self.write(root, fixture)
                    result = self.run_viewer(root, '--software', '--frames', 1,
                                             '--capture-frame', destination, success=False)
                    self.assertNotIn('Video=', result.stderr)
                    self.assertFalse(destination.exists())
            self.write(root, combiner_fixture(full))
            self.run_viewer(root, '--inspect', textures=False, success=False)
            (root / 'model.garc').write_bytes(archive(cgfx_model()))
            self.run_viewer(root, '--inspect', success=False)
            help_text = subprocess.run([str(VIEWER), '--help'], text=True, capture_output=True,
                                       check=True, timeout=15).stdout
            self.assertIn('--combiners', help_text)
            # The stage-0 preview remains the default for the same fixture.
            self.write(root, combiner_fixture(full))
            stage0 = self.run_viewer(root, '--inspect', combiners=False).stdout
            self.assertIn('shading=unlit-tev-stage0', stage0)


if __name__ == '__main__':
    unittest.main()
