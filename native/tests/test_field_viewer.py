"""Independent ZO -> MM -> GR fixtures for the field inspection executable."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_viewer import bmp_pixel, literals


VIEWER = Path(os.environ.get(
    'PX_FIELD_VIEWER', Path(__file__).resolve().parents[1] / 'build/pokemon_field_viewer')).resolve()


def minipack(magic, parts, alignment=4):
    offset = (8 + 4 * len(parts) + alignment - 1) // alignment * alignment
    data = bytearray(offset)
    data[:4] = magic + struct.pack('<H', len(parts))
    for index, part in enumerate(parts):
        struct.pack_into('<I', data, 4 + 4 * index, len(data))
        data.extend(part)
        data.extend(bytes((-len(data)) % alignment))
    struct.pack_into('<I', data, 4 + 4 * len(parts), len(data))
    return data


def garc(members):
    """One subentry per entry, independently assembled GARC v4 allocation."""
    payload = bytearray()
    records = bytearray()
    offsets = []
    for member in members:
        offsets.append(len(records))
        start = len(payload)
        payload.extend(member)
        payload.extend(bytes((-len(payload)) % 4))
        records.extend(struct.pack('<4I', 1, start, len(payload), len(member)))
    fato = b'OTAF' + struct.pack('<IHH', 12 + 4 * len(members), len(members), 0xffff)
    fato += struct.pack('<' + 'I' * len(offsets), *offsets)
    fatb = b'BTAF' + struct.pack('<II', 12 + len(records), len(members)) + records
    fimb = b'BMIF' + struct.pack('<II', 12, len(payload))
    start = 28 + len(fato) + len(fatb) + len(fimb)
    header = b'CRAG' + struct.pack('<IHHHHIII', 28, 0xfeff, 0x0400, 4, 0,
                                   start, start + len(payload), max(map(len, members)))
    return header + fato + fatb + fimb + payload


def zone(matrix_id=0):
    header = bytearray(56)
    header[0] = 3
    struct.pack_into('<HH', header, 2, 75, matrix_id)
    # Metadata retained verbatim; deliberately not an inspection marker spawn.
    struct.pack_into('<iii', header, 44, 999, -123, 567)
    return minipack(b'ZO', [header, b'', b'', b''])


def matrix(ids=(0,), width=1, height=1):
    cells = struct.pack('<4H', 0, 0, width, height)
    cells += struct.pack('<' + 'h' * len(ids), *ids)
    # One camera bound and the observed zero-filled sixteen-record capacity.
    cameras = struct.pack('<I4fi', 1, 0, 720, 0, 720, 0) + bytes(300)
    return minipack(b'MM', [cells, cameras])


def collision(points):
    data = bytearray(800)
    data[:4] = b'coll'
    struct.pack_into('<7I', data, 4, 784 + 16 * len(points), 0x5d8, 1, 3, 2, 1, 2)
    for group in range(16):
        struct.pack_into('<II', data, 672 + 8 * group,
                         0 if group == 0 else len(points), len(points) if group == 0 else 0)
    for point in points:
        data.extend(struct.pack('<4f', *point, 1))
    data.extend(b'term' + struct.pack('<3I', 0, 0x5d8, 1))
    return data


def region(walkable=((1, 1), (2, 1), (2, 2), (3, 2)), *, width=40, height=40,
           collision_data=b''):
    words = [0x01000021] * (width * height)
    for x, z in walkable:
        words[z * width + x] = 0x20
    if width > 1 and height > 2:
        words[2 * width + 1] = 0x01000020  # Same low byte, unknown full tile word.
    tiles = struct.pack('<HH', width, height) + struct.pack('<' + 'I' * len(words), *words)
    # Models/collision remain opaque metadata for this 2D field-grid path.
    return minipack(b'GR', [tiles, b'BCH\0', collision_data, b'', b'', b''], 128)


def romfs_fixture(root, *, compressed=False, matrix_data=None, regions=None, zone_data=None):
    resources = {
        'a/0/1/2': [zone() if zone_data is None else zone_data],
        'a/0/4/2': [matrix() if matrix_data is None else matrix_data],
        'a/0/4/1': [region()] if regions is None else regions,
    }
    for relative, members in resources.items():
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(garc([literals(m) if compressed else m for m in members]))
    return {relative: (root / relative).read_bytes() for relative in resources}


class FieldViewerTests(unittest.TestCase):
    def run_viewer(self, *arguments, success=True):
        result = subprocess.run([str(VIEWER), *map(str, arguments)], text=True,
                                capture_output=True, timeout=15,
                                env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'})
        if success:
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        else:
            self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        return result

    def test_help_demo_and_input(self):
        self.assertIn('--zone', self.run_viewer('--help').stdout)
        result = self.run_viewer('--demo', '--inspect')
        self.assertNotIn('Video=', result.stderr)
        with tempfile.TemporaryDirectory() as directory:
            destination = Path(directory) / 'demo.bmp'
            result = self.run_viewer('--demo', '--software', '--frames', 2,
                                     '--self-test-input', '--capture-frame', destination)
            self.assertIn('Input self-test passed', result.stderr)
            self.assertEqual(destination.read_bytes()[:2], b'BM')

    def test_raw_lz_chain_and_exact_word_policy(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for compressed in (False, True):
                with self.subTest(compressed=compressed):
                    originals = romfs_fixture(root, compressed=compressed)
                    result = self.run_viewer('--romfs', root, '--zone', 0,
                                             '--inspect', '--steps', 'RDLU')
                    self.assertIn('marker_start=(1,1)', result.stdout)
                    self.assertIn('marker=(2,1)', result.stdout)
                    self.assertIn('accepted=3 rejected=1', result.stdout)
                    self.assertIn('standard=4', result.stdout)
                    self.assertNotIn('Video=', result.stderr)
                    for name, original in originals.items():
                        self.assertEqual((root / name).read_bytes(), original)

    def test_multi_region_placement_and_boundary_steps(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            romfs_fixture(root, matrix_data=matrix((0, 1), 2, 1),
                          regions=[region(((39, 1),)), region(((0, 1), (1, 1)))])
            result = self.run_viewer('--romfs', root, '--zone', 0,
                                     '--inspect', '--steps', 'RRD')
            self.assertIn('marker_start=(39,1)', result.stdout)
            self.assertIn('marker=(41,1)', result.stdout)
            self.assertIn('accepted=2 rejected=1', result.stdout)

    def test_capture_and_no_clobber(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            originals = romfs_fixture(root)
            destination = root / 'field.bmp'
            self.run_viewer('--romfs', root, '--zone', 0, '--software', '--frames', 1,
                            '--capture-frame', destination)
            image = destination.read_bytes()
            self.assertEqual(image[:2], b'BM')
            self.assertEqual(struct.unpack_from('<ii', image, 18), (1100, 760))
            # The fixture's focused bounds are x1..3,z1..2. The documented
            # top-down viewport puts these cell centers100 pixels apart.
            self.assertEqual(bmp_pixel(image, 284, 354), (255, 225, 125))  # marker(1,1)
            self.assertEqual(bmp_pixel(image, 384, 454), (58, 143, 116))   # standard(2,2)
            self.assertEqual(bmp_pixel(image, 284, 454), (191, 128, 65))   # unknown(1,2)
            self.assertEqual(bmp_pixel(image, 184, 354), (39, 53, 70))     # blocked(0,1)
            existing = root / 'a/0/4/1'
            alias = root / 'alias.bmp'
            alias.symlink_to(existing)
            for path in (existing, alias, destination):
                original = path.read_bytes()
                self.run_viewer('--romfs', root, '--zone', 0, '--software', '--frames', 1,
                                '--capture-frame', path, success=False)
                self.assertEqual(path.read_bytes(), original)
            for name, original in originals.items():
                self.assertEqual((root / name).read_bytes(), original)

    def test_malformed_chain_limits_and_no_capture(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            destination = root / 'never.bmp'
            bad_cases = [
                {'zone_data': b'ZO'},
                {'zone_data': zone(1)},
                {'matrix_data': matrix((1,))},
                {'matrix_data': matrix((0,) * 65, 65, 1)},
                {'regions': [region(((1, 1),), width=2, height=3)]},
            ]
            for options in bad_cases:
                with self.subTest(options=list(options)):
                    originals = romfs_fixture(root, **options)
                    result = self.run_viewer('--romfs', root, '--zone', 0, '--software',
                                             '--frames', 1, '--capture-frame', destination,
                                             success=False)
                    self.assertFalse(destination.exists())
                    self.assertNotIn('Video=', result.stderr)
                    for name, original in originals.items():
                        self.assertEqual((root / name).read_bytes(), original)

    def test_absent_region_or_standard_cells_remain_inspectable(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for options in ({'matrix_data': matrix((-1,))}, {'regions': [region(())]}):
                romfs_fixture(root, **options)
                result = self.run_viewer('--romfs', root, '--zone', 0,
                                         '--inspect', '--steps', 'RDLU')
                self.assertIn('marker_start=(-1,-1)', result.stdout)
                self.assertIn('marker=(-1,-1)', result.stdout)
                self.assertIn('accepted=0 rejected=4', result.stdout)

    def test_large_collision_edges_are_clipped_before_drawing(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            # The first edge lies at localZ=-333, hence worldZ27/cell-row1.5.
            # Its endpoints are far outside the viewport but its center crosses it.
            points = ((-1e12, 0, -333), (1e12, 0, -333), (0, 0, 1e12))
            romfs_fixture(root, regions=[region(collision_data=collision(points))])
            destination = root / 'collision.bmp'
            self.run_viewer('--romfs', root, '--zone', 0, '--software', '--frames', 1,
                            '--collision', '--capture-frame', destination)
            self.assertEqual(bmp_pixel(destination.read_bytes(), 700, 354), (144, 152, 213))

    def test_collision_budget_counts_repeated_region_instances(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            #4098 unique vertices fit the parser;64 instances require262272,
            # exceeding the app's262144 placed-vertex budget.
            points = ((0, 0, 0), (18, 0, 0), (0, 0, 18)) * 1366
            originals = romfs_fixture(root, matrix_data=matrix((0,) * 64, 8, 8),
                                      regions=[region(collision_data=collision(points))])
            destination = root / 'never.bmp'
            result = self.run_viewer('--romfs', root, '--zone', 0, '--software', '--frames', 1,
                                     '--collision', '--capture-frame', destination, success=False)
            self.assertFalse(destination.exists())
            self.assertNotIn('Video=', result.stderr)
            for name, original in originals.items():
                self.assertEqual((root / name).read_bytes(), original)

    def test_invalid_cli(self):
        for arguments in ([], ['--unknown'], ['--demo', '--zone', '0'],
                          ['--romfs', '/tmp'], ['--romfs', '/tmp', '--zone', '-1'],
                          ['--romfs', '/tmp', '--zone', '4294967296'],
                          ['--demo', '--frames', '0'], ['--demo', '--steps', 'RX'],
                          ['--demo', '--inspect', '--frames', '1'],
                          ['--demo', '--inspect', '--collision'],
                          ['--demo', '--capture-frame', ''],
                          ['--demo', '--inspect', '--self-test-input']):
            with self.subTest(arguments=arguments):
                result = self.run_viewer(*arguments, success=False)
                self.assertIn('Usage:', result.stderr)


if __name__ == '__main__':
    unittest.main()
