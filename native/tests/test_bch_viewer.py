"""Synthetic GARC/LZ/AD/GR/BCH pixel and failure-path tests; no game files."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_viewer import VIEWER, archive, bmp_pixel, etc_texture, literals


def texture_pack():
    # Fixed, deliberately separated sections. Two 8x8 ETC textures share no
    # payload bytes. A relocated zero names the first string/raw payload.
    data = bytearray(928)
    data[:8] = b'BCH\0\x07\x07\0\0'
    bases = (56, 512, 576, 800, 928)
    lengths = (456, 64, 224, 128, 0)
    struct.pack_into('<5I', data, 8, *bases)
    relocs = []

    def word(at, value):
        struct.pack_into('<I', data, at, value)

    def ptr(at, target, tag):
        target_base = bases[{0: 0, 1: 1, 2: 2, 0x24: 3}[tag]]
        source_base = bases[2 if tag == 0x24 else 0]
        word(at, target - target_base)
        relocs.append((tag << 25) | ((at - source_base) // (1 if tag == 1 else 4)))

    # Empty categories use a valid shared root; texture dictionary has 2 nodes.
    for category in range(15):
        ptr(56 + category * 12, 236, 0)
        ptr(64 + category * 12, 248, 0)
    ptr(92, 236, 0)  # replace duplicates from the generic category header
    relocs.pop()
    word(96, 2)
    ptr(100, 272, 0)
    relocs.pop()
    ptr(236, 320, 0)
    ptr(240, 352, 0)
    data[512:518] = b'etc\n"\0'
    data[520:526] = b'alpha\0'
    for index, obj, named, payload, fmt in [(0, 320, 512, 800, 12), (1, 352, 520, 832, 13)]:
        ptr(292 + index * 12, named, 1)
        ptr(obj + 28, named, 1)
        word(obj + 24, 0x100 | fmt)
        encoded = etc_texture(fmt == 13)[:-40]
        data[payload:payload + len(encoded)] = encoded
        for unit, (dim, address, form) in enumerate([(0x82, 0x85, 0x8e),
                                                    (0x92, 0x95, 0x96),
                                                    (0x9a, 0x9d, 0x9e)]):
            cmd = 576 + index * 96 + unit * 32
            ptr(obj + unit * 8, cmd, 2)
            word(obj + unit * 8 + 4, 8)
            struct.pack_into('<8I', data, cmd, 8 << 16 | 8, 0xf0000 | dim,
                             0, 0xf0000 | address, fmt, 0xf0000 | form, 1, 0xf023d)
            ptr(cmd + 8, payload, 0x24)
    data.extend(struct.pack('<' + 'I' * len(relocs), *relocs))
    struct.pack_into('<5I', data, 28, *lengths[:4], len(relocs) * 4)
    return data


def minipack(member, magic=b'AD'):
    # Includes unselected sections on each side so malformed tables matter.
    return magic + struct.pack('<H4I', 3, 20, 24, 24 + len(member), 28 + len(member)) + b'abcd' + member + b'efgh'


class BCHViewerTests(unittest.TestCase):
    def run_viewer(self, root, *args, success=True):
        result = subprocess.run([str(VIEWER), '--romfs', str(root), '--archive', 'fixture.garc',
                                 '--entry', '0', *map(str, args)], text=True, capture_output=True,
                                env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=15)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def test_standalone_listing_selection_and_pixels(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for compressed in (False, True):
                data = texture_pack()
                (root / 'fixture.garc').write_bytes(archive(literals(data) if compressed else data))
                listing = self.run_viewer(root, '--list-textures').stdout
                self.assertIn('container=BCH textures=2', listing)
                self.assertIn('name="etc\\x0a\\x22"', listing)
                self.assertIn('texture=1 name="alpha"', listing)
                for index in (0, 1):
                    path = root / f'tex-{compressed}-{index}.bmp'
                    result = self.run_viewer(root, '--texture', index, '--inspect', '--screenshot', path)
                    self.assertIn('compression=' + ('LZ11' if compressed else 'none'), result.stdout)
                    data = path.read_bytes()
                    for x, y, rgb, a in [(0, 0, (138, 70, 36), 0), (1, 0, (138, 70, 36), 68),
                                          (0, 1, (138, 70, 36), 17), (5, 4, (70, 104, 138), 68)]:
                        self.assertEqual(bmp_pixel(data, x, y, True), (*rgb, a if index else 255))
                self.assertIn('texture=0', self.run_viewer(root, '--inspect').stdout)
                self.run_viewer(root, '--texture', 2, '--inspect', success=False)

    def test_minipack_outer_and_inner_compression(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for magic in (b'AD', b'GR'):
                for outer in (False, True):
                    for inner in (False, True):
                        with self.subTest(magic=magic, outer=outer, inner=inner):
                            bch = texture_pack()
                            pack = minipack(literals(bch) if inner else bch, magic)
                            (root / 'fixture.garc').write_bytes(archive(literals(pack) if outer else pack))
                            dest = root / 'image.bmp'
                            output = self.run_viewer(root, '--section', 1, '--texture', 1,
                                                     '--inspect', '--screenshot', dest).stdout
                            self.assertIn('section=1 section_compression=' + ('LZ11' if inner else 'none'), output)
                            self.assertEqual(bmp_pixel(dest.read_bytes(), 1, 0, True), (138, 70, 36, 68))
                            dest.unlink()
                            self.run_viewer(root, '--inspect', success=False)

    def test_invalid_selection_and_whole_minipack_table(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            pack = bytearray(minipack(texture_pack()))
            for where, value in [(4, 4), (8, 2), (12, 0xffffffff), (16, len(pack) + 1), (16, len(pack) - 5)]:
                bad = bytearray(pack)
                struct.pack_into('<I', bad, where, value)
                (root / 'fixture.garc').write_bytes(archive(bad))
                self.run_viewer(root, '--section', 1, '--inspect', success=False)
            (root / 'fixture.garc').write_bytes(archive(pack))
            for section in ('-1', '3', '256', '4294967296'):
                self.run_viewer(root, '--section', section, '--inspect', success=False)
            (root / 'fixture.garc').write_bytes(archive(texture_pack()))
            self.run_viewer(root, '--section', 0, '--inspect', success=False)
            bad = minipack(b'\x11\xff\xff\xff\x80\0\0')
            (root / 'fixture.garc').write_bytes(archive(bad))
            self.run_viewer(root, '--section', 1, '--inspect', success=False)
            result = subprocess.run([str(VIEWER), '--demo', '--section', '0'], capture_output=True)
            self.assertNotEqual(result.returncode, 0)

    def test_raw_section_pixels_are_not_mistaken_for_lz_headers(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for compressed in (False, True):
                texture = etc_texture(True)
                self.assertEqual(texture[0], 0x10)
                pack = minipack(literals(texture) if compressed else texture)
                (root / 'fixture.garc').write_bytes(archive(pack))
                dest = root / f'alpha-{compressed}.bmp'
                self.run_viewer(root, '--section', 1, '--inspect', '--screenshot', dest)
                self.assertEqual(bmp_pixel(dest.read_bytes(), 1, 0, True), (138, 70, 36, 68))

    def test_bad_bch_and_unsupported_descriptor(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            original = texture_pack()
            # Header bounds, string offset, raw extent, command header and width.
            for where, value in [(8, 0), (12, 56), (28, 0xffffffff), (320 + 28, 64),
                                 (584, 128), (580, 0xfff0082), (320 + 4, 4098), (96, 65536)]:
                bad = bytearray(original)
                struct.pack_into('<I', bad, where, value)
                (root / 'fixture.garc').write_bytes(archive(bad))
                self.run_viewer(root, '--inspect', '--screenshot', root / 'no.bmp', success=False)
                self.assertFalse((root / 'no.bmp').exists())
            unsupported = bytearray(original)
            struct.pack_into('<I', unsupported, 320 + 24, 0x20c)
            (root / 'fixture.garc').write_bytes(archive(unsupported))
            self.assertIn('unsupported', self.run_viewer(root, '--list-textures').stdout)
            self.run_viewer(root, '--inspect', success=False)

    def test_runtime_input_capture_and_exclusive_outputs(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            data = archive(minipack(texture_pack()))
            source = root / 'fixture.garc'
            source.write_bytes(data)
            capture = root / 'frame.bmp'
            self.run_viewer(root, '--section', 1, '--texture', 1, '--software', '--self-test-input',
                            '--capture-frame', capture)
            self.assertGreater(capture.stat().st_size, 1000)
            for destination in (source, capture):
                before = destination.read_bytes()
                self.run_viewer(root, '--section', 1, '--inspect', '--screenshot', destination, success=False)
                self.assertEqual(destination.read_bytes(), before)
            link = root / 'input-link.bmp'
            link.symlink_to(source)
            self.run_viewer(root, '--section', 1, '--inspect', '--screenshot', link, success=False)
            self.assertEqual(source.read_bytes(), data)


if __name__ == '__main__':
    unittest.main()
