"""Black-box checks of wireframe and bounded textured model previews."""
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


def textured_model(*, uv=(0.25, 0.25), color=(255, 255, 255, 255),
                   blend=False, alpha_test=False, alpha_reference=0):
    """Independent quad + UV/color stream + material + asymmetric RGBA8 TXOB.

    Metadata uses explicit relative pointers. Command packets are assembled
    from PICA register numbers and byte masks, rather than copied game bytes.
    Top decoded rows are red; bottom rows are blue, both with alpha128.
    """
    data = bytearray(9000)
    data[:2048] = cgfx_model()[:2048]
    def word(offset, value):
        struct.pack_into('<I', data, offset, value & 0xffffffff)
    def pointer(field, target):
        word(field, target - field)
    def floats(offset, *values):
        struct.pack_into('<' + 'f' * len(values), data, offset, *values)
    def dictionary(offset, name, obj):
        data[offset:offset + 12] = b'DICT' + struct.pack('<II', 44, 1)
        pointer(offset + 36, name); pointer(offset + 40, obj)
    def command(offset, register, value, mask=15):
        word(offset, value); word(offset + 4, register | mask << 16)
    word(12, len(data)); word(24, 8192 - 20)
    data[8192:8200] = b'IMAG' + struct.pack('<I', len(data) - 8192)
    word(256 + 0xd4, 1)
    pointer(256 + 0xc0, 2048)
    word(36, 1); pointer(40, 1536)
    data[1856:1864] = b'pattern\0'
    data[1888:1897] = b'material\0'
    dictionary(1536, 1856, 1616)
    dictionary(2048, 1888, 2304)
    # Three distinct stream attributes, with backward table references allowed.
    word(768 + 20, 96); pointer(768 + 24, 8200)
    word(768 + 36, 24); word(768 + 40, 3)
    pointer(836, 1180); pointer(840, 1240)
    for offset, semantic, fmt, elements, scale, local in (
            (1180, 4, 0x1406, 2, 1, 12),
            (1240, 3, 0x1401, 4, 1 / 255, 20)):
        word(offset, 0x40000001); word(offset + 4, semantic)
        word(offset + 36, fmt); word(offset + 40, elements)
        floats(offset + 44, scale); word(offset + 48, local)
    for i, position in enumerate(((-2,-1,0), (2,-1,0), (2,3,1), (-2,3,1))):
        floats(8200 + i * 24, *position, *uv)
        data[8220 + i * 24:8224 + i * 24] = bytes(color)
    pointer(1132, 8320)
    struct.pack_into('<6H', data, 8320, 0,1,2,0,2,3)
    # Single-level PICA RGBA8 texture: ABGR bytes in Morton tile order.
    word(1616, 0x20000011); data[1620:1624] = b'TXOB'
    word(1624, 0x05000000); pointer(1628, 1856)
    word(1616 + 24, 8); word(1616 + 28, 8)
    word(1616 + 40, 1); pointer(1616 + 56, 1792)
    word(1792, 8); word(1796, 8); word(1800, 256)
    pointer(1804, 8400); word(1812, 32)
    for y in range(8):
        for x in range(8):
            morton = sum(((x >> bit & 1) << (2 * bit)) |
                         ((y >> bit & 1) << (2 * bit + 1)) for bit in range(3))
            data[8400 + morton * 4:8404 + morton * 4] = (
                bytes((128, 0, 0, 200)) if y < 4 else bytes((128, 200, 0, 0)))
    # MTOB v6.0.0.3, identity UV0, no culling/depth, replace or SrcAlpha blend.
    material, mapper, reference, sampler, fragment = 2304, 3072, 3168, 3216, 3328
    word(material, 0x08000000); data[material + 4:material + 8] = b'MTOB'
    word(material + 8, 0x06000003); pointer(material + 12, 1888)
    word(material + 0x108, 3); command(material + 0x110, 0x40, 0, 1)
    command(material + 0x11c, 0x107, 0x10, 1)
    command(material + 0x124, 0x126, 0, 8)
    word(material + 0x12c, int(blend))
    command(material + 0x140, 0x100, 0)
    command(material + 0x148, 0x101, 0x76760000 if blend else 0x01010000)
    command(material + 0x150, 0x102, 0)
    command(material + 0x158, 0x105, 0)
    command(material + 0x160, 0x106, 0)
    word(material + 0x168, 1); word(material + 0x174, 0xffffffff)
    floats(material + 0x17c, 1, 1)
    floats(material + 0x194, 1,0,0,0, 0,1,0,0, 0,0,1,0)
    pointer(material + 0x274, mapper); pointer(material + 0x288, fragment)
    word(mapper, 0x80000000); pointer(mapper + 8, reference)
    pointer(mapper + 12, sampler); word(mapper + 72, 56)
    # Consecutive-register packet writes border,dims,param,LOD,address.
    command(mapper + 16, 0x8e, 0, 1)
    word(mapper + 24, 0); word(mapper + 28, 0x804f0081)
    word(mapper + 32, 0); word(mapper + 36, 0); word(mapper + 40, 0)
    word(mapper + 44, 0)
    command(mapper + 48, 0x85, 0); command(mapper + 56, 0x86, 0)
    command(mapper + 64, 0x87, 0)
    word(reference, 0x20000004); data[reference + 4:reference + 8] = b'TXOB'
    word(reference + 8, 0x05000000)
    pointer(reference + 12, 1856); pointer(reference + 24, 1856)
    word(sampler, 0x80000000); pointer(sampler + 4, mapper)
    command(fragment + 0xd4, 0x104,
            int(alpha_test) | 6 << 4 | alpha_reference << 8)
    return data


def overlapping_textured_model():
    """Two coincident quads: resource order blue/red, preview order red/blue.

    Each draw replaces color and disables depth. Different constant UVs select
    distinct halves of one texture, so the final pixel proves draw ordering.
    """
    data = textured_model()
    data.extend(bytes(10000 - len(data)))
    def word(offset, value):
        struct.pack_into('<I', data, offset, value & 0xffffffff)
    def pointer(field, target):
        word(field, target - field)
    word(12, len(data)); word(8196, len(data) - 8192)
    word(256 + 0xb4, 2); pointer(256 + 0xb8, 4210)
    word(256 + 0xbc, 2); word(256 + 0xc4, 2); pointer(256 + 0xc8, 4200)
    pointer(4200, 544); pointer(4204, 4000)
    pointer(4210, 640); pointer(4214, 4100)
    data[4000:4068] = data[544:612]
    pointer(4000 + 48, 944); pointer(4000 + 60, 4250); pointer(4250, 4400)
    data[4100:4144] = data[640:684]
    word(4100 + 24, 1); word(4100 + 28, 1); pointer(4100 + 32, 256)
    data[4400:4448] = data[768:816]
    pointer(4400 + 24, 9000); pointer(4400 + 44, 832)
    data[9000:9096] = data[8200:8296]
    for i in range(4):
        struct.pack_into('<f', data, 9000 + i * 24 + 16, 0.75)
    word(2052, 60); word(2056, 2)
    data[1904:1914] = b'material2\0'
    pointer(2048 + 52, 1904); pointer(2048 + 56, 4500)
    data[4500:4500 + 0x28c] = data[2304:2304 + 0x28c]
    pointer(4500 + 12, 1904)
    pointer(4500 + 0x274, 3072); pointer(4500 + 0x288, 3328)
    word(2304 + 0x20, 1); word(4500 + 0x20, 0)
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

    def test_textured_demo_inspection_capture_and_input(self):
        result = self.run_viewer('--demo', '--textured', '--inspect')
        self.assertIn('mode=textured shading=unlit-texture-times-vertex-color', result.stdout)
        self.assertIn('draws=1 materials=1 textures=1 draw_order=translucency-priority-resource', result.stdout)
        with tempfile.TemporaryDirectory() as directory:
            destination = Path(directory) / 'demo.bmp'
            result = self.run_viewer('--demo', '--textured', '--software', '--frames', 2,
                                     '--self-test-input', '--capture-frame', destination)
            self.assertIn('Input self-test passed', result.stderr)
            image = destination.read_bytes()
            self.assertEqual(struct.unpack_from('<ii', image, 18), (960, 640))
            self.assertNotEqual(bmp_pixel(image, 480, 320), (22, 27, 36))

    def test_textured_raw_lz_uv_color_alpha_and_blend(self):
        # A constant UV/color means every interior pixel has an independently
        # known value regardless of camera angle or triangle interpolation.
        cases = [
            ('raw-blue', False, {}, (0, 0, 200)),
            ('lz-red', True, {'uv': (0.25, 0.75)}, (200, 0, 0)),
            ('modulate', False, {'color': (255, 255, 128, 255)}, (0, 0, 100)),
            ('alpha-pass', False, {'alpha_test': True, 'alpha_reference': 127}, (0, 0, 200)),
            ('alpha-equal-reject', False, {'alpha_test': True, 'alpha_reference': 128}, (22, 27, 36)),
            # RGB = texture * (128/255) + background * (127/255).
            ('source-alpha', True, {'blend': True}, (11, 13, 118)),
        ]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            selected = root / 'texture.garc'
            for name, compressed, options, expected in cases:
                with self.subTest(case=name):
                    member = textured_model(**options)
                    selected.write_bytes(archive(literals(member) if compressed else member))
                    original = selected.read_bytes()
                    base = ['--romfs', root, '--archive', selected.name, '--entry', 0, '--textured']
                    inspected = self.run_viewer(*base, '--inspect')
                    self.assertIn('draws=1 materials=1 textures=1 draw_order=translucency-priority-resource', inspected.stdout)
                    self.assertIn('draw=0 shape=0 material=0 texture=0 indices=6 visible=1', inspected.stdout)
                    destination = root / (name + '.bmp')
                    self.run_viewer(*base, '--software', '--frames', 1,
                                    '--capture-frame', destination)
                    image = destination.read_bytes()
                    for x, y in ((480, 320), (465, 305), (495, 335)):
                        self.assertEqual(bmp_pixel(image, x, y), expected)
                    self.assertEqual(bmp_pixel(image, 0, 0), (22, 27, 36))
                    self.assertEqual(selected.read_bytes(), original)
                    self.run_viewer(*base, '--software', '--frames', 1,
                                    '--capture-frame', selected, success=False)
                    self.assertEqual(selected.read_bytes(), original)

    def test_textured_unsupported_state_creates_no_capture(self):
        # Blend equation, border wrap, material UV scale, missing UV0, and
        # an out-of-image texture extent each reject before SDL/export.
        mutations = ((2304 + 0x148, 0x01010001), (3072 + 36, 0x100),
                     (2304 + 0x17c, 0x40000000), (1180 + 4, 5),
                     (1800, 1024))
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            selected = root / 'invalid.garc'
            destination = root / 'never.bmp'
            for offset, value in mutations:
                with self.subTest(offset=offset):
                    member = textured_model()
                    struct.pack_into('<I', member, offset, value)
                    selected.write_bytes(archive(member))
                    original = selected.read_bytes()
                    result = self.run_viewer('--romfs', root, '--archive', selected.name,
                                             '--entry', 0, '--textured', '--software',
                                             '--frames', 1, '--capture-frame', destination,
                                             success=False)
                    self.assertFalse(destination.exists())
                    self.assertNotIn('Video=', result.stderr)
                    self.assertEqual(selected.read_bytes(), original)

    def test_textured_preview_translucency_order(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            selected = root / 'overlap.garc'
            selected.write_bytes(archive(overlapping_textured_model()))
            destination = root / 'overlap.bmp'
            result = self.run_viewer('--romfs', root, '--archive', selected.name,
                                     '--entry', 0, '--textured', '--software',
                                     '--frames', 1, '--capture-frame', destination)
            self.assertIn('draws=2 materials=2 textures=1', result.stdout)
            self.assertEqual(bmp_pixel(destination.read_bytes(), 480, 320), (0, 0, 200))


if __name__ == '__main__':
    unittest.main()
