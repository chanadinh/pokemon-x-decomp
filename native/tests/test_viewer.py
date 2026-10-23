"""Black-box checks of the native GARC -> LZ11 -> BCLIM -> BMP path."""
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
