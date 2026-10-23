"""Black-box BCH textured-preview checks using only hand-authored fixtures."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_model import float_words, packet, triangle_fixture
from test_bch_viewer import minipack, texture_pack
from test_model_viewer import VIEWER, cgfx_model
from test_viewer import archive, bmp_pixel, literals

# The opaque ETC1 fixture texture "etc\n\"" has exactly these four block colors.
ETC_COLORS = {(138, 70, 36), (36, 138, 70), (70, 36, 138), (70, 104, 138)}
BACKGROUND = (22, 27, 36)


def textured_fixture(names=(b'etc\n"', None, None), unit_config=0x11001,
                     stage0=(0x00030003, 0, 0x00000001, 0xff000000, 0),
                     packed=(0xff000001, 0xff000001, 1), blend=0x01010000,
                     alpha=0x10, depth=0x1f51, sampler=(0x2220, 0x2220, 0x2220)):
    """A triangle whose material carries a full parameter object."""
    f = triangle_fixture()
    material = f.labels['material']
    # Replace the four-byte parameter object with a full 0xd0-byte one. The
    # resource table and material both point at it; old fields are rewritten.
    params = f.alloc('contents', 0xd0)
    f.pointer(material, params, 0)
    table = f.bases['contents'] + struct.unpack_from('<I', f.data, 56 + 12)[0]
    f.pointer(table, params, 0)
    for unit, name in enumerate(names):
        field = material + 0x48 + unit * 4
        if name is None:
            f.relocations.pop(field, None)
            f.word(field, 0)
        else:
            f.pointer(field, f.text(name), 1)
    texture_commands = (packet(0x80, unit_config) + packet(0x83, sampler[0]) +
                        packet(0x93, sampler[1]) + packet(0x9b, sampler[2]))
    f.stream(material + 0x10, texture_commands)
    for unit in range(3):
        at = params + 12 + unit * 24
        f.word(at, packed[unit])
        struct.pack_into('<5f', f.data, at + 4, 1, 1, 0, 0, 0)
    fragment = (packet(0x40, 2, mask=1) + packet(0x100, 0x100, mask=3) + packet(0x101, blend)
                + packet(0x104, alpha, mask=3) + packet(0x107, depth)
                + packet(0xC0, *stage0, consecutive=True))
    f.stream(params + 0xC8, fragment)
    f.labels['params'] = params
    return f


def pack_with_names(first, second):
    data = bytearray(texture_pack())
    assert len(first) <= 6 and len(second) <= 6
    data[512:518] = first.ljust(6, b'\0')
    data[520:526] = second.ljust(6, b'\0')
    return data


class BCHTexturedViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True, textures=True):
        extra = ['--texture-archive', 'textures.garc', '--texture-entry', '0'] if textures else []
        result = subprocess.run(
            [str(VIEWER), '--romfs', str(root), '--archive', 'model.garc', '--entry', '0',
             *extra, *map(str, arguments)], text=True, capture_output=True,
            env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=15)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def write(self, root, model, textures=None):
        (root / 'model.garc').write_bytes(archive(model))
        (root / 'textures.garc').write_bytes(archive(texture_pack() if textures is None else textures))

    def test_stage_zero_binding_is_reported_and_rendered_from_texels(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root, textured_fixture().finish())
            result = self.run_viewer(root, '--triangle-list', '--textured', '--inspect')
            self.assertIn('mode=textured visibility=all-meshes materials=tev-stage0-unlit', result.stdout)
            self.assertIn('texture_archive=textures.garc texture_entry=0 texture_subentry=0 '
                          'texture_compression=none', result.stdout)
            self.assertIn('external_textures=2 decoded_textures=1', result.stdout)
            self.assertIn('binding material=0 unit=0 texture="etc\\x0a\\x22" external_index=0 '
                          'size=8x8 wrap=2,2 filter=nearest alpha_test=0', result.stdout)
            self.assertIn('blend=replace cull_register=2 unexecuted_units=0x0', result.stdout)
            self.assertIn('culling=not-executed', result.stdout)
            self.assertNotIn('Video=', result.stderr)
            capture = root / 'textured.bmp'
            result = self.run_viewer(root, '--triangle-list', '--textured', '--software',
                                     '--self-test-input', '--capture-frame', capture)
            self.assertIn('unlit BCH TEV-stage-0 software preview', result.stderr)
            self.assertIn('Input self-test passed', result.stderr)
            data = capture.read_bytes()
            seen = {}
            for y in range(0, 640, 2):
                for x in range(0, 960, 2):
                    rgb = bmp_pixel(data, x, y)
                    if rgb != BACKGROUND:
                        seen[rgb] = seen.get(rgb, 0) + 1
            self.assertGreater(sum(seen.values()), 1000)
            # Unlit replace with nearest sampling: every covered pixel is a texel.
            self.assertLessEqual(set(seen), ETC_COLORS)
            self.assertGreaterEqual(len(seen), 3)

    def test_projected_unit_is_not_required_when_stage_zero_names_unit_one(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            model = textured_fixture(names=(b'nope', b'alpha', None), unit_config=0x11003,
                                     stage0=(0x00040014, 0, 0x00010001, 0, 0),
                                     packed=(0x00030001, 0xff000001, 1),
                                     sampler=(0x30002220, 0x3006, 0x2220),
                                     blend=0x76760000, alpha=0x0061, depth=0x0f51)
            self.write(root, model.finish())
            result = self.run_viewer(root, '--triangle-list', '--textured', '--inspect')
            self.assertIn('binding material=0 unit=1 texture="alpha" external_index=1 size=8x8 '
                          'wrap=3,0 filter=linear alpha_test=1 alpha_compare=6 alpha_reference=0 '
                          'depth_test=1 depth_compare=5 depth_write=0 color_mask=0xf '
                          'blend=source-alpha cull_register=2 unexecuted_units=0x1', result.stdout)
            capture = root / 'alpha.bmp'
            self.run_viewer(root, '--triangle-list', '--textured', '--software', '--frames', 1,
                            '--capture-frame', capture)
            data = capture.read_bytes()
            colors = {bmp_pixel(data, x, y) for y in range(0, 640, 4) for x in range(0, 960, 4)}
            # Source-alpha blending over the background produces blended colors.
            self.assertTrue(colors - ETC_COLORS - {BACKGROUND})

    def test_compressed_minipack_texture_source(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            model = textured_fixture().finish()
            (root / 'model.garc').write_bytes(archive(literals(minipack(model, b'GR'))))
            (root / 'textures.garc').write_bytes(archive(literals(minipack(literals(texture_pack()), b'AD'))))
            result = self.run_viewer(root, '--section', 1, '--texture-section', 1,
                                     '--triangle-list', '--textured', '--inspect')
            self.assertIn('compression=LZ11 section=1 section_compression=none', result.stdout)
            self.assertIn('texture_compression=LZ11 texture_section=1 '
                          'texture_section_compression=LZ11', result.stdout)
            self.assertIn('external_index=0', result.stdout)
            self.run_viewer(root, '--section', 1, '--triangle-list', '--textured', '--inspect',
                            success=False)

    def test_unsupported_or_unresolved_materials_fail_before_video(self):
        cases = [
            ('missing name', textured_fixture(names=(b'absent', None, None)), None),
            ('case differs', textured_fixture(names=(b'Alpha', None, None)), None),
            ('ambiguous name', textured_fixture(names=(b'twin', None, None)),
             pack_with_names(b'twin', b'twin')),
            ('constant source', textured_fixture(stage0=(0x0003000e, 0, 0, 0, 0)), None),
            ('add combiner', textured_fixture(stage0=(0x00030003, 0, 2, 0, 0)), None),
            ('disabled unit', textured_fixture(unit_config=0x11000), None),
            ('projection mapping', textured_fixture(packed=(0x00030001, 1, 1)), None),
            ('border wrap', textured_fixture(sampler=(0x1220, 0x2220, 0x2220)), None),
            ('projection sampler', textured_fixture(sampler=(0x30002220, 0x2220, 0x2220)), None),
            ('distinct filters', textured_fixture(sampler=(0x2222, 0x2220, 0x2220)), None),
            ('logic op', textured_fixture(), None),
            ('constant blend', textured_fixture(blend=0xaaaa0000), None),
            ('subtract', textured_fixture(blend=0x01010001), None),
        ]
        logic = cases[-3][1]
        fragment = struct.unpack_from('<I', logic.data, logic.labels['params'] + 0xC8)[0]
        at = logic.bases['commands'] + fragment + 8  # second packet: 0x100
        logic.word(at, 0)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            destination = root / 'never.bmp'
            for label, model, textures in cases:
                with self.subTest(case=label):
                    self.write(root, model.finish(), textures)
                    result = self.run_viewer(root, '--triangle-list', '--textured', '--software',
                                             '--frames', 1, '--capture-frame', destination,
                                             success=False)
                    self.assertNotIn('Video=', result.stderr)
                    self.assertFalse(destination.exists())
            # A legacy parameter object has no fragment metadata to interpret.
            self.write(root, triangle_fixture().finish())
            result = self.run_viewer(root, '--triangle-list', '--textured', '--inspect', success=False)
            self.assertIn('lacks fragment parameter metadata', result.stderr)
            self.run_viewer(root, '--triangle-list', '--inspect', textures=False)  # wireframe still valid
            # The texture source must be a BCH texture pack.
            self.write(root, textured_fixture().finish(), cgfx_model())
            self.run_viewer(root, '--triangle-list', '--textured', '--inspect', success=False)

    def test_option_contract(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write(root, textured_fixture().finish())
            for arguments, textures in [(('--triangle-list', '--inspect'), True),
                                        (('--textured', '--inspect'), True),
                                        (('--triangle-list', '--textured', '--inspect'), False),
                                        (('--triangle-list', '--textured', '--inspect',
                                          '--texture-archive', 'textures.garc'), False),
                                        (('--triangle-list', '--textured', '--inspect',
                                          '--texture-subentry', '32'), True),
                                        (('--triangle-list', '--textured', '--inspect',
                                          '--texture-section', '256'), True)]:
                with self.subTest(arguments=arguments, textures=textures):
                    result = self.run_viewer(root, *arguments, textures=textures, success=False)
                    self.assertNotIn('Video=', result.stderr)
            result = self.run_viewer(root, '--triangle-list', '--textured', '--inspect',
                                     '--texture-archive', '../textures.garc', '--texture-entry', '0',
                                     textures=False, success=False)
            self.assertIn('Usage:', result.stderr)
            (root / 'model.garc').write_bytes(archive(cgfx_model()))
            result = self.run_viewer(root, '--triangle-list', '--textured', '--inspect', success=False)
            self.assertIn('--texture-archive requires BCH model input', result.stderr)
            for extra in (('--texture-archive', 'textures.garc', '--texture-entry', '0'),):
                result = subprocess.run([str(VIEWER), '--demo', '--textured', *extra, '--inspect'],
                                        text=True, capture_output=True, timeout=15)
                self.assertNotEqual(result.returncode, 0)
            help_text = subprocess.run([str(VIEWER), '--help'], text=True, capture_output=True,
                                       check=True, timeout=15).stdout
            for option in ('--texture-archive', '--texture-entry', '--texture-section'):
                self.assertIn(option, help_text)


if __name__ == '__main__':
    unittest.main()
