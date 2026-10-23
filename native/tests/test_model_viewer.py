"""Black-box checks of the independent orthographic wireframe inspector."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_viewer import archive, bmp_pixel, literals


VIEWER = Path(os.environ.get(
    'PX_MODEL_VIEWER', Path(__file__).resolve().parents[1] / 'build/pokemon_model_viewer')).resolve()


def cgfx_model():
    """One independent quad fixture: float positions and u16 triangle indices.

    Relative tables are deliberately separated; image data lives only in IMAG.
    The fixed source positions scale by one half and translate by (10,20,30).
    """
    data = bytearray(2160)
    def word(offset, value):
        struct.pack_into('<I', data, offset, value & 0xffffffff)
    def pointer(field, target):
        word(field, target - field)
    def floats(offset, *values):
        struct.pack_into('<' + 'f' * len(values), data, offset, *values)
    data[:20] = b'CGFX' + struct.pack('<HHIII', 0xfeff, 20, 0x05000000, len(data), 2)
    data[20:28] = b'DATA' + struct.pack('<I', 2048 - 20)
    data[2048:2056] = b'IMAG' + struct.pack('<I', len(data) - 2048)
    word(28, 1); pointer(32, 156)
    data[156:168] = b'DICT' + struct.pack('<II', 44, 1)
    pointer(192, 224); pointer(196, 256)
    name = b'quad\n"\0'
    data[224:224 + len(name)] = name
    word(256, 0x40000012); data[260:264] = b'CMDL'
    word(264, 0x09000000); pointer(268, 224)
    floats(256 + 0x30, 1, 1, 1)
    for matrix in (256 + 0x54, 256 + 0x84):
        floats(matrix, 1,0,0,0, 0,1,0,0, 0,0,1,0)
    word(256 + 0xb4, 1); pointer(256 + 0xb8, 516)
    word(256 + 0xbc, 1)
    word(256 + 0xc4, 1); pointer(256 + 0xc8, 512)
    pointer(512, 544); pointer(516, 640)
    # Object selects shape0 and points backward to its owning model.
    word(640, 0x01000000); data[644:648] = b'SOBJ'
    pointer(672, 256); data[676] = 1
    word(544, 0x10000001); data[548:552] = b'SOBJ'
    floats(544 + 32, 10, 20, 30)
    word(544 + 44, 1); pointer(544 + 48, 944)
    word(544 + 56, 1); pointer(544 + 60, 736)
    pointer(736, 768)
    # Interleaved buffer, one position attribute, stride12.
    word(768, 0x40000002); word(772, 21); word(776, 2)
    word(768 + 20, 48); pointer(768 + 24, 2056)
    word(768 + 36, 12); word(768 + 40, 1); pointer(768 + 44, 832)
    pointer(832, 864); word(864, 0x40000001)
    word(864 + 36, 0x1406); word(864 + 40, 3); floats(864 + 44, 0.5)
    # Submesh -> face -> triangle-list descriptor -> u16 index bytes.
    pointer(944, 960); word(960 + 12, 1); pointer(960 + 16, 1000)
    pointer(1000, 1024); word(1024, 1); pointer(1028, 1088)
    pointer(1088, 1120); word(1120, 0x1403); data[1125] = 1
    word(1128, 12); pointer(1132, 2120)
    floats(2056, -2,-1,0, 2,-1,0, 2,3,1, -2,3,1)
    struct.pack_into('<6H', data, 2120, 0,1,2,0,2,3)
    return data


class ModelViewerTests(unittest.TestCase):
    def run_viewer(self, *arguments, success=True):
        result = subprocess.run([str(VIEWER), *map(str, arguments)], text=True,
                                capture_output=True, timeout=15,
                                env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'})
        if success:
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        else:
            self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        return result

    def test_demo_inspection_and_help(self):
        result = self.run_viewer('--demo', '--inspect')
        self.assertIn('name="synthetic tetrahedron"', result.stdout)
        self.assertIn('vertices=4 triangles=4 shapes=1', result.stdout)
        self.assertIn('bounds_min=(-1,-1,-1) bounds_max=(1,1,1)', result.stdout)
        self.assertNotIn('Video=', result.stderr)
        help_text = self.run_viewer('--help').stdout
        self.assertIn('--model', help_text)
        self.assertIn('Orthographic wireframe', help_text)

    def test_demo_frame_capture_and_input(self):
        with tempfile.TemporaryDirectory() as directory:
            destination = Path(directory) / 'frame.bmp'
            result = self.run_viewer('--demo', '--software', '--frames', 2,
                                     '--self-test-input', '--capture-frame', destination)
            self.assertIn('Input self-test passed', result.stderr)
            self.assertIn('Exit after 2 frames.', result.stderr)
            self.assertIn('Video=dummy', result.stderr)
            data = destination.read_bytes()
            self.assertEqual(data[:2], b'BM')
            self.assertEqual(struct.unpack_from('<ii', data, 18), (960, 640))
            self.assertEqual(bmp_pixel(data, 0, 0), (22, 27, 36))
            offset = struct.unpack_from('<I', data, 10)[0]
            bits = struct.unpack_from('<H', data, 28)[0]
            self.assertIn(bits, (24, 32))
            pixel_size = bits // 8
            stride = ((960 * bits + 31) // 32) * 4
            wire = 0
            for row in range(640):
                for column in range(960):
                    start = offset + row * stride + column * pixel_size
                    wire += data[start:start + 3] == bytes((230, 211, 126))
            # Check that actual mesh lines reached the captured software frame.
            self.assertGreater(wire, 100)
            self.assertLess(wire, 960 * 640 // 4)
        default_frames = self.run_viewer('--demo', '--software', '--self-test-input')
        self.assertIn('Exit after 3 frames.', default_frames.stderr)

    def test_capture_preserves_existing_file_and_symlink_target(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            original = root / 'keep.txt'
            original.write_bytes(b'original bytes must survive')
            paths = [original]
            link = root / 'alias.bmp'
            link.symlink_to(original)
            paths.append(link)
            for destination in paths:
                self.run_viewer('--demo', '--software', '--frames', 1,
                                '--capture-frame', destination, success=False)
                self.assertEqual(original.read_bytes(), b'original bytes must survive')
            self.assertTrue(link.is_symlink())

    def test_invalid_options_are_rejected_before_loading(self):
        invalid = [[], ['--unknown'], ['--demo', '--model', '0'],
                   ['--demo', '--entry', '0'], ['--demo', '--subentry', '0'],
                   ['--demo', '--romfs', '/tmp'], ['--demo', '--archive', 'a'],
                   ['--demo', '--frames', '0'], ['--demo', '--frames', '-1'],
                   ['--demo', '--frames', '4294967296'], ['--demo', '--frames', '1x'],
                   ['--demo', '--frames'], ['--demo', '--capture-frame', ''],
                   ['--demo', '--inspect', '--frames', '1'],
                   ['--demo', '--inspect', '--capture-frame', 'a.bmp'],
                   ['--demo', '--inspect', '--self-test-input']]
        base = ['--romfs', '/tmp', '--entry', '0']
        for path in ('../escape', '/absolute', 'a//b', 'a/./b', 'a/../b',
                     'a/', 'a\\b', 'C:/path'):
            invalid.append([*base, '--archive', path])
        for option, value in (('--subentry', '32'), ('--model', '-1'),
                              ('--entry', '4294967296')):
            invalid.append([*base, '--archive', 'fixture.garc', option, value])
        for arguments in invalid:
            with self.subTest(arguments=arguments):
                result = self.run_viewer(*arguments, success=False)
                self.assertIn('Usage:', result.stderr)

    def test_archive_and_compression_errors_create_no_capture(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            selected = root / 'fixture.garc'
            destination = root / 'never.bmp'
            arguments = ['--romfs', root, '--archive', 'fixture.garc', '--entry', 0,
                         '--software', '--frames', 1, '--capture-frame', destination]
            self.run_viewer(*arguments, success=False)
            for member in (b'not CGFX', b'\x11\x10\x00\x00\x80', b'CGFX'):
                selected.write_bytes(archive(member))
                original = selected.read_bytes()
                self.run_viewer(*arguments, success=False)
                self.assertFalse(destination.exists())
                self.assertEqual(selected.read_bytes(), original)
            selected.write_bytes(archive(b'not CGFX')[:-1])
            self.run_viewer(*arguments, success=False)
            self.assertFalse(destination.exists())

    def test_raw_and_lz_model_pipeline(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            selected = root / 'fixture.garc'
            for compressed in (False, True):
                with self.subTest(compressed=compressed):
                    member = cgfx_model()
                    selected.write_bytes(archive(literals(member) if compressed else member))
                    original = selected.read_bytes()
                    base = ['--romfs', root, '--archive', selected.name, '--entry', 0]
                    result = self.run_viewer(*base, '--inspect')
                    self.assertIn('compression=' + ('LZ11' if compressed else 'none'), result.stdout)
                    self.assertIn('name="quad\\x0a\\x22" vertices=4 triangles=2 shapes=1', result.stdout)
                    self.assertIn('bounds_min=(9,19.5,30) bounds_max=(11,21.5,30.5)', result.stdout)
                    destination = root / ('compressed.bmp' if compressed else 'raw.bmp')
                    self.run_viewer(*base, '--software', '--frames', 1,
                                    '--capture-frame', destination)
                    self.assertEqual(destination.read_bytes()[:2], b'BM')
                    self.assertEqual(selected.read_bytes(), original)
                    self.run_viewer(*base, '--software', '--frames', 1,
                                    '--capture-frame', selected, success=False)
                    self.assertEqual(selected.read_bytes(), original)
                    self.run_viewer(*base, '--inspect', '--model', 1, success=False)
                    self.run_viewer(*base, '--inspect', '--subentry', 1, success=False)
            # Unsupported skeletons and out-of-range indices fail before SDL/export.
            for offset, value in ((256, 0x40000092), (2120, 4)):
                member = cgfx_model()
                struct.pack_into('<I' if offset == 256 else '<H', member, offset, value)
                selected.write_bytes(archive(member))
                destination = root / 'invalid.bmp'
                self.run_viewer('--romfs', root, '--archive', selected.name, '--entry', 0,
                                '--software', '--frames', 1, '--capture-frame', destination,
                                success=False)
                self.assertFalse(destination.exists())


if __name__ == '__main__':
    unittest.main()
