"""Black-box checks of GARC -> optional LZ11 -> BCLIM/CGFX -> BMP."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

VIEWER = Path(os.environ.get('PX_VIEWER', Path(__file__).resolve().parents[1] / 'build/pokemon_asset_viewer')).resolve()


def texture():
    # 8x8 opaque blue, with red at (0,0) and green at (1,0).
    pixels = bytearray(b'\xff\xff\x00\x00' * 64)
    pixels[:8] = b'\xff\x00\x00\xff\xff\x00\xff\x00'
    footer = b'CLIM' + struct.pack('<HHIIHH', 0xfeff, 20, 0x02020000, 296, 1, 0)
    footer += b'imag' + struct.pack('<IHHII', 16, 8, 8, 9, 256)
    return bytes(pixels) + footer


def etc_texture(alpha):
    # Four fixed ETC1 blocks in one 8x8 PICA tile, cropped to 6x5.
    # Equal 4-bit endpoints, table 0, selector 0: each channel is base+2.
    pixels = bytearray()
    for color in (0x8844220000000000, 0x2288440000000000,
                  0x4422880000000000, 0x4466880000000000):
        if alpha:
            # Column-major alpha nibble indices: (0,0)=0, (1,0)=4, (0,1)=1.
            pixels.extend(bytes.fromhex('1032547698badcfe'))
        pixels.extend(color.to_bytes(8, 'little'))
    footer = b'CLIM' + struct.pack('<HHIIHH', 0xfeff, 20, 0x02020000,
                                  len(pixels) + 40, 1, 0)
    footer += b'imag' + struct.pack('<IHHII', 16, 6, 5, 11 if alpha else 10, len(pixels))
    return bytes(pixels) + footer


def literals(data):
    # Deliberately simple independent LZ11 fixture: literal groups only.
    output = bytearray(b'\x11' + len(data).to_bytes(3, 'little'))
    for offset in range(0, len(data), 8):
        output.append(0)
        output.extend(data[offset:offset + 8])
    return bytes(output)


def cgfx_texture():
    # Independent two-entry CGFX: descriptors deliberately not inline.
    data = bytearray(1408)
    data[:20] = b'CGFX' + struct.pack('<HHIII', 0xfeff, 20, 0x05000000, len(data), 2)
    data[20:28] = b'DATA' + struct.pack('<I', 1004)
    data[1024:1032] = b'IMAG' + struct.pack('<I', 384)
    def word(offset, value):
        struct.pack_into('<I', data, offset, value & 0xffffffff)
    word(36, 2); word(40, 156 - 40)
    data[156:168] = b'DICT' + struct.pack('<II', 60, 2)
    for i, obj, desc, name, payload, fmt, bpp, encoded in [
            (0, 256, 320, 768, 1032, 0, 32, texture()[:-40]),
            (1, 512, 448, 800, 1288, 13, 8, etc_texture(True)[:-40])]:
        record = 184 + 16 * i
        word(record + 8, name - (record + 8)); word(record + 12, obj - (record + 12))
        data[name:name + 6] = (b'raw\n"\0' if i == 0 else b'alpha\0')
        word(obj, 0x20000011); data[obj + 4:obj + 8] = b'TXOB'
        word(obj + 8, 0x05000000); word(obj + 12, name - (obj + 12))
        word(obj + 0x18, 8); word(obj + 0x1c, 8); word(obj + 0x28, 1)
        word(obj + 0x34, fmt); word(obj + 0x38, desc - (obj + 0x38))
        word(desc, 8); word(desc + 4, 8); word(desc + 8, len(encoded))
        word(desc + 12, payload - (desc + 12)); word(desc + 20, bpp)
        data[payload:payload + len(encoded)] = encoded
    return data


def archive(member):
    padded = (len(member) + 3) & ~3
    header = b'CRAG' + struct.pack('<IHHHHIII', 28, 0xfeff, 0x0400, 4, 0, 84, 84 + padded, len(member))
    fato = b'OTAF' + struct.pack('<IHHI', 16, 1, 0xffff, 0)
    fatb = b'BTAF' + struct.pack('<IIIIII', 28, 1, 1, 0, padded, len(member))
    fimb = b'BMIF' + struct.pack('<II', 12, padded)
    return header + fato + fatb + fimb + member + bytes(padded - len(member))


def bmp_pixel(data, x, y, with_alpha=False):
    assert data[:2] == b'BM'
    offset = struct.unpack_from('<I', data, 10)[0]
    width, height = struct.unpack_from('<ii', data, 18)
    bits = struct.unpack_from('<H', data, 28)[0]
    assert bits in (24, 32)
    row = y if height < 0 else height - y - 1
    stride = ((width * bits + 31) // 32) * 4
    position = offset + row * stride + x * (bits // 8)
    # SDL_SaveBMP's BMP byte order is B,G,R,(A).
    b, g, r = data[position:position + 3]
    if with_alpha:
        assert bits == 32
        return (r, g, b, data[position + 3])
    return (r, g, b)


class ViewerTests(unittest.TestCase):
    def run_viewer(self, *arguments, success=True):
        result = subprocess.run([str(VIEWER), *map(str, arguments)],
                                text=True, capture_output=True, timeout=15,
                                env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'})
        if success:
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        else:
            self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        return result

    def test_cgfx_selection_listing_and_exported_pixels(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for compressed in (False, True):
                member = cgfx_texture()
                (root / 'fixture.garc').write_bytes(archive(literals(member) if compressed else member))
                base = ['--romfs', root, '--archive', 'fixture.garc', '--entry', 0]
                listing = self.run_viewer(*base, '--list-textures').stdout
                self.assertIn('container=CGFX textures=2', listing)
                self.assertIn('name="raw\\x0a\\x22"', listing)
                self.assertIn('texture=1 name="alpha"', listing)
                for index in (0, 1):
                    image = root / 'result.bmp'
                    result = self.run_viewer(*base, '--inspect', '--texture', index,
                                             '--screenshot', image)
                    self.assertIn('format_namespace=PICA', result.stdout)
                    self.assertIn('compression=' + ('LZ11' if compressed else 'none'), result.stdout)
                    data = image.read_bytes()
                    if index == 0:
                        self.assertEqual(bmp_pixel(data, 0, 0, True), (255, 0, 0, 255))
                        self.assertEqual(bmp_pixel(data, 1, 0, True), (0, 255, 0, 255))
                    else:
                        self.assertEqual(bmp_pixel(data, 1, 0, True), (138, 70, 36, 68))
                        self.assertEqual(bmp_pixel(data, 5, 4, True), (70, 104, 138, 68))
                    image.unlink()
                self.assertIn('texture=0', self.run_viewer(*base, '--inspect').stdout)
                self.run_viewer(*base, '--inspect', '--texture', 2, success=False)

    def test_cgfx_bad_descriptor_and_unsupported_mips(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            base = ['--romfs', root, '--archive', 'fixture.garc', '--entry', 0]
            member = cgfx_texture()
            struct.pack_into('<I', member, 256 + 0x28, 2)
            (root / 'fixture.garc').write_bytes(archive(member))
            self.assertIn('unsupported', self.run_viewer(*base, '--list-textures').stdout.lower())
            self.run_viewer(*base, '--inspect', success=False)
            struct.pack_into('<I', member, 512 + 0x38, 0)
            (root / 'fixture.garc').write_bytes(archive(member))
            destination = root / 'no.bmp'
            self.run_viewer(*base, '--inspect', '--texture', 1, '--screenshot', destination,
                            success=False)
            self.assertFalse(destination.exists())
            (root / 'fixture.garc').write_bytes(archive(texture()))
            self.run_viewer(*base, '--inspect', '--texture', 0, success=False)
            self.run_viewer(*base, '--list-textures', success=False)

    def test_raw_and_compressed_pipeline_and_exported_pixels(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for compressed in (False, True):
                member = literals(texture()) if compressed else texture()
                (root / 'fixture.garc').write_bytes(archive(member))
                image = root / 'result.bmp'
                result = self.run_viewer('--romfs', root, '--archive', 'fixture.garc',
                                         '--entry', 0, '--inspect', '--screenshot', image)
                self.assertIn('width=8 height=8 format=9', result.stdout)
                self.assertIn('compression=' + ('LZ11' if compressed else 'none'), result.stdout)
                data = image.read_bytes()
                self.assertEqual(bmp_pixel(data, 0, 0), (255, 0, 0))
                self.assertEqual(bmp_pixel(data, 1, 0), (0, 255, 0))
                self.assertEqual(bmp_pixel(data, 0, 1), (0, 0, 255))
                self.assertEqual(bmp_pixel(data, 7, 7), (0, 0, 255))
                image.unlink()

    def test_etc_archive_pipeline_crop_and_exported_alpha(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for alpha in (False, True):
                for compressed in (False, True):
                    with self.subTest(alpha=alpha, compressed=compressed):
                        member = etc_texture(alpha)
                        (root / 'fixture.garc').write_bytes(archive(literals(member) if compressed else member))
                        image = root / 'result.bmp'
                        result = self.run_viewer('--romfs', root, '--archive', 'fixture.garc',
                                                 '--entry', 0, '--inspect', '--screenshot', image)
                        self.assertIn('width=6 height=5 format=' + ('11' if alpha else '10'), result.stdout)
                        self.assertIn('compression=' + ('LZ11' if compressed else 'none'), result.stdout)
                        data = image.read_bytes()
                        self.assertEqual(struct.unpack_from('<ii', data, 18), (6, 5))
                        for x, y, rgb, a in [(0, 0, (138, 70, 36), 0),
                                              (1, 0, (138, 70, 36), 68),
                                              (0, 1, (138, 70, 36), 17),
                                              (4, 0, (36, 138, 70), 0),
                                              (0, 4, (70, 36, 138), 0),
                                              (5, 4, (70, 104, 138), 68)]:
                            self.assertEqual(bmp_pixel(data, x, y, with_alpha=True),
                                             (*rgb, a if alpha else 255))
                        image.unlink()

    def test_export_does_not_overwrite_existing_files_or_input(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            fixture = root / 'fixture.garc'
            original = archive(texture())
            fixture.write_bytes(original)
            for export, mode in [('--screenshot', ['--inspect']),
                                 ('--capture-frame', ['--software', '--frames', 1])]:
                result = self.run_viewer('--romfs', root, '--archive', 'fixture.garc',
                                         '--entry', 0, *mode, export, fixture,
                                         success=False)
                self.assertEqual(fixture.read_bytes(), original)
            destination = root / 'keep.txt'
            destination.write_bytes(b'keep this file')
            self.run_viewer('--demo', '--inspect', '--screenshot', destination, success=False)
            self.assertEqual(destination.read_bytes(), b'keep this file')

    def test_invalid_selection_and_options(self):
        for arguments in [[], ['--demo', '--frames', '0'], ['--demo', '--frames', '-1'],
                          ['--demo', '--frames', '4294967296'], ['--demo', '--romfs', '/tmp'],
                          ['--demo', '--inspect', '--frames', '1'],
                          ['--demo', '--texture', '0'], ['--demo', '--list-textures'],
                          ['--romfs', '/tmp', '--archive', 'a', '--entry', '0', '--texture', '-1'],
                          ['--romfs', '/tmp', '--archive', 'a', '--entry', '0', '--list-textures', '--frames', '1'],
                          ['--romfs', '/tmp', '--archive', '../escape', '--entry', '0'],
                          ['--romfs', '/tmp', '--archive', '/absolute', '--entry', '0'],
                          ['--romfs', '/tmp', '--archive', 'a//b', '--entry', '0'],
                          ['--romfs', '/tmp', '--archive', 'a', '--entry', '0', '--subentry', '32']]:
            with self.subTest(arguments=arguments):
                self.run_viewer(*arguments, success=False)

    def test_missing_member_truncated_archive_and_unsupported_texture(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            selection = ['--romfs', root, '--archive', 'fixture.garc', '--entry', 0, '--inspect']
            self.run_viewer(*selection, success=False)
            (root / 'fixture.garc').write_bytes(archive(texture()))
            self.run_viewer('--romfs', root, '--archive', 'fixture.garc', '--entry', 1,
                            '--inspect', success=False)
            (root / 'fixture.garc').write_bytes(archive(texture())[:-1])
            self.run_viewer(*selection, success=False)
            unsupported = bytearray(texture())
            struct.pack_into('<I', unsupported, len(unsupported) - 8, 14)
            (root / 'fixture.garc').write_bytes(archive(unsupported))
            result = self.run_viewer(*selection, success=False)
            self.assertIn('unsupported', result.stderr.lower())


if __name__ == '__main__':
    unittest.main()
