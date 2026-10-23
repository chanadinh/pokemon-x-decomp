"""Native BCH geometry preview checks using only hand-authored fixtures."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_model import packet, triangle_fixture
from test_bch_viewer import minipack
from test_model_viewer import VIEWER, cgfx_model
from test_viewer import archive, bmp_pixel, literals


def compressed(data, kind):
    # Literal-only packets have the same layout in LZ10 and LZ11.
    return bytes((kind,)) + literals(data)[1:] if kind else data


class BCHModelViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run(
            [str(VIEWER), '--romfs', str(root), '--archive', 'fixture.garc',
             '--entry', '0', *map(str, arguments)], text=True, capture_output=True,
            env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=15)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def assert_triangle(self, result):
        self.assertIn('topology=assumed-triangle-list', result.stdout)
        self.assertIn('name="triangle" vertices=3 triangles=1 shapes=1', result.stdout)
        self.assertIn('bounds_min=(6,-28,18) bounds_max=(24,-2,40)', result.stdout)
        self.assertNotIn('Video=', result.stderr)

    def test_raw_lz10_lz11_and_both_index_widths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'fixture.garc'
            for width in (1, 2):
                for compression in (0, 0x10, 0x11):
                    with self.subTest(width=width, compression=compression):
                        before = archive(compressed(triangle_fixture(width).finish(), compression))
                        source.write_bytes(before)
                        result = self.run_viewer(root, '--triangle-list', '--inspect', '--model', 0)
                        self.assert_triangle(result)
                        self.assertIn('compression=' + ('none' if not compression else f'LZ{compression:02x}'),
                                      result.stdout)
                        self.assertEqual(source.read_bytes(), before)

    def test_ad_gr_sections_with_independent_inner_outer_compression(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'fixture.garc'
            bch = triangle_fixture().finish()
            for magic in (b'AD', b'GR'):
                for outer in (0, 0x10, 0x11):
                    for inner in (0, 0x10, 0x11):
                        with self.subTest(magic=magic, outer=outer, inner=inner):
                            pack = minipack(compressed(bch, inner), magic)
                            before = archive(compressed(pack, outer))
                            source.write_bytes(before)
                            result = self.run_viewer(root, '--section', 1,
                                                     '--triangle-list', '--inspect')
                            self.assert_triangle(result)
                            self.assertIn('section=1 section_compression=' +
                                          ('none' if not inner else f'LZ{inner:02x}'), result.stdout)
                            self.assertEqual(source.read_bytes(), before)
            self.run_viewer(root, '--triangle-list', '--inspect', success=False)

    def test_signed_packed_attributes_preserve_scale_and_translation(self):
        raw = (-2, 3, 5, 2, -4, 7, -4, 2, 8, 12, -1, 9, -6, -8, 16)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for fmt, nibble_pair, stride in [('b', 0x48, 5), ('h', 0x6a, 10)]:
                with self.subTest(format=fmt):
                    f = triangle_fixture()
                    f.word(f.labels['enable'] + 8, nibble_pair)
                    f.word(f.labels['enable'] + 24, 0x20000000 | stride << 16)
                    struct.pack_into('<15' + fmt, f.data, f.labels['vertices'], *raw)
                    (root / 'fixture.garc').write_bytes(archive(f.finish()))
                    self.assert_triangle(self.run_viewer(root, '--triangle-list', '--inspect'))

    def test_multiple_meshes_rebase_indices_and_keep_material_references(self):
        f = triangle_fixture()
        old_mesh = f.labels['mesh']
        record = f.data[old_mesh:old_mesh + 56]
        meshes = f.alloc('contents', record + record)
        disable = f.bases['commands'] + struct.unpack_from('<I', f.data, old_mesh + 24)[0]
        for mesh in (meshes, meshes + 56):
            f.pointer(mesh + 8, f.labels['enable'], 2)
            f.pointer(mesh + 16, f.labels['sub'], 0)
            f.pointer(mesh + 24, disable, 2)
        # The second mesh shares source vertices but has its own +50 X offset.
        # Distinct screen regions make a missed index rebase visible.
        word_count = struct.unpack_from('<I', f.data, old_mesh + 12)[0]
        enable = bytearray(f.data[f.labels['enable']:f.labels['enable'] + 4 * word_count])
        offset = enable.index(struct.pack('<4f', 0, 30, -20, 10))
        struct.pack_into('<f', enable, offset + 12, 60)
        enabled = f.stream(meshes + 56 + 8, enable)
        f.pointer(enabled + 16, f.labels['vertices'], 0x26)
        f.pointer(f.labels['model'] + 0x40, meshes, 0)
        f.word(f.labels['model'] + 0x44, 2)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / 'fixture.garc').write_bytes(archive(f.finish()))
            result = self.run_viewer(root, '--triangle-list', '--inspect')
            self.assertIn('vertices=6 triangles=2 shapes=2', result.stdout)
            self.assertIn('bounds_min=(6,-28,18) bounds_max=(74,-2,40)', result.stdout)
            self.assertIn('first_vertex=3 vertices=3 first_index=3 indices=3 material=0', result.stdout)
            capture = root / 'two-meshes.bmp'
            self.run_viewer(root, '--triangle-list', '--software', '--frames', 1,
                            '--capture-frame', capture)
            data = capture.read_bytes()
            for first, last in ((0, 480), (480, 960)):
                wire = sum(bmp_pixel(data, x, y) == (126, 211, 230)
                           for y in range(640) for x in range(first, last))
                self.assertGreater(wire, 100)

    def test_primitive_state_is_checked_at_draw_before_later_writes(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'fixture.garc'
            for mode_at_draw, trailing_mode in [(0, 1), (1, 0)]:
                with self.subTest(mode_at_draw=mode_at_draw, trailing_mode=trailing_mode):
                    commands = (packet(0x25e, mode_at_draw << 8, mask=2) +
                                packet(0x22f, 1) +
                                packet(0x25e, trailing_mode << 8, mask=2))
                    source.write_bytes(archive(triangle_fixture(extra_face=commands).finish()))
                    result = self.run_viewer(root, '--triangle-list', '--inspect',
                                             success=mode_at_draw == 0)
                    if mode_at_draw == 0:
                        self.assert_triangle(result)
                    self.assertNotIn('Video=', result.stderr)

    def test_explicit_topology_and_unsupported_modes_reject_before_video(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'fixture.garc'
            source.write_bytes(archive(triangle_fixture().finish()))
            destination = root / 'never.bmp'
            render = ('--software', '--frames', 1, '--capture-frame', destination)
            for arguments in [(), ('--textured',), ('--triangle-list', '--textured')]:
                with self.subTest(arguments=arguments):
                    result = self.run_viewer(root, *arguments, *render, success=False)
                    self.assertNotIn('Video=', result.stderr)
                    self.assertFalse(destination.exists())
            self.run_viewer(root, '--inspect', success=False)
            for option, value in (('--model', 1), ('--subentry', 1)):
                self.run_viewer(root, '--triangle-list', '--inspect', option, value, success=False)
            source.write_bytes(archive(cgfx_model()))
            self.run_viewer(root, '--triangle-list', '--inspect', success=False)
            for arguments in [('--demo', '--triangle-list'), ('--demo', '--section', '0')]:
                result = subprocess.run([str(VIEWER), *arguments, '--inspect'], text=True,
                                        capture_output=True, timeout=15)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('Usage:', result.stderr)
            help_text = subprocess.run([str(VIEWER), '--help'], text=True,
                                       capture_output=True, check=True, timeout=15).stdout
            self.assertIn('--triangle-list', help_text)
            self.assertIn('--section', help_text)

    def test_invalid_geometry_and_relocations_produce_no_capture(self):
        # These failures exercise the native reader, before a renderer can hide
        # bad references or attempt to index an incomplete vertex array.
        variants = []
        for label, offset, value in [('model', 0, 1), ('model', 0x6c, 1),
                                     ('model', 0x44, 1025), ('mesh', 0, 1),
                                     ('sub', 0, 1), ('face', 8, 2)]:
            f = triangle_fixture()
            f.word(f.labels[label] + offset, value)
            variants.append((f'{label}+{offset}', f.finish()))
        f = triangle_fixture()
        f.data[f.labels['indices']] = 255
        variants.append(('index-derived vertex extent exceeds raw section', f.finish()))
        for number in (float('nan'), float('inf'), 1e20):
            f = triangle_fixture()
            struct.pack_into('<f', f.data, f.labels['vertices'], number)
            variants.append((f'position {number}', f.finish()))
        f = triangle_fixture()
        del f.relocations[f.labels['enable'] + 16]
        variants.append(('unrelocated vertex pointer at raw offset zero', f.finish()))
        f = triangle_fixture()
        valid = f.finish()
        locations = list(f.relocation_offsets.values())
        struct.pack_into('<I', valid, locations[1], struct.unpack_from('<I', valid, locations[0])[0])
        variants.append(('duplicate relocation source', valid))
        variants += [('truncated header', b'BCH\0'),
                     ('truncated relocation table', triangle_fixture().finish()[:-1])]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'fixture.garc'
            destination = root / 'never.bmp'
            for label, data in variants:
                with self.subTest(case=label):
                    before = archive(data)
                    source.write_bytes(before)
                    result = self.run_viewer(root, '--triangle-list', '--software', '--frames', 1,
                                             '--capture-frame', destination, success=False)
                    self.assertNotIn('Video=', result.stderr)
                    self.assertFalse(destination.exists())
                    self.assertEqual(source.read_bytes(), before)

    def test_minipack_table_and_compression_failures_are_bounded(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'fixture.garc'
            destination = root / 'never.bmp'
            arguments = ('--section', 1, '--triangle-list', '--software', '--frames', 1,
                         '--capture-frame', destination)
            pack = bytearray(minipack(triangle_fixture().finish(), b'GR'))
            # Include both unselected sections: validate the whole offset table.
            for offset, value in [(4, 4), (8, 2), (12, 0xffffffff),
                                  (16, len(pack) + 1), (16, len(pack) - 5)]:
                with self.subTest(offset=offset, value=value):
                    data = bytearray(pack)
                    struct.pack_into('<I', data, offset, value)
                    source.write_bytes(archive(data))
                    result = self.run_viewer(root, *arguments, success=False)
                    self.assertFalse(destination.exists())
                    self.assertNotIn('Video=', result.stderr)
            for member in (b'\x11\x10\0\0\x80', minipack(b'\x10\xff\xff\xff\x80'),
                           triangle_fixture().finish()):
                source.write_bytes(archive(member))
                self.run_viewer(root, *arguments, success=False)
                self.assertFalse(destination.exists())
            source.write_bytes(archive(pack))
            for section in ('-1', '3', '256', '4294967296'):
                self.run_viewer(root, '--section', section, '--triangle-list', '--inspect', success=False)

    def test_wireframe_input_and_exclusive_frame_capture(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'fixture.garc'
            before = archive(compressed(minipack(triangle_fixture().finish(), b'GR'), 0x11))
            source.write_bytes(before)
            capture = root / 'frame.bmp'
            arguments = ('--section', 1, '--triangle-list', '--software')
            result = self.run_viewer(root, *arguments, '--self-test-input', '--capture-frame', capture)
            self.assertIn('Input self-test passed', result.stderr)
            self.assertIn('Exit after 3 frames.', result.stderr)
            data = capture.read_bytes()
            self.assertEqual(struct.unpack_from('<ii', data, 18), (960, 640))
            self.assertEqual(bmp_pixel(data, 0, 0), (22, 27, 36))
            bits = struct.unpack_from('<H', data, 28)[0]
            offset = struct.unpack_from('<I', data, 10)[0]
            stride = ((960 * bits + 31) // 32) * 4
            wire = sum(data[offset + y * stride + x * (bits // 8):
                            offset + y * stride + x * (bits // 8) + 3] == bytes((230, 211, 126))
                       for y in range(640) for x in range(960))
            self.assertGreater(wire, 100)
            self.assertLess(wire, 960 * 640 // 4)
            saved = capture.read_bytes()
            alias = root / 'alias.bmp'
            alias.symlink_to(source)
            dangling = root / 'dangling.bmp'
            absent = root / 'absent.bmp'
            dangling.symlink_to(absent)
            for path in (capture, source, alias, dangling):
                with self.subTest(path=path):
                    self.run_viewer(root, *arguments, '--frames', 1,
                                    '--capture-frame', path, success=False)
                    self.assertEqual(source.read_bytes(), before)
                    self.assertEqual(capture.read_bytes(), saved)
            self.assertTrue(alias.is_symlink())
            self.assertTrue(dangling.is_symlink())
            self.assertFalse(absent.exists())


if __name__ == '__main__':
    unittest.main()
