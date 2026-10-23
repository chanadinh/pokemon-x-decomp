"""Synthetic BCH index bounds and identity checks; no game assets."""
import importlib.util
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('inspect_bch', Path(__file__).resolve().parents[1] / 'tools/inspect_bch.py')
bch = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bch)


def fixture(name=b'one'):
    name_size = len(name) + 1
    data = bytearray(296 + name_size)
    data[:8] = b'BCH\0\x07\x07\0\0'
    struct.pack_into('<5I', data, 8, 56, 280, 280 + name_size,
                     284 + name_size, 288 + name_size)
    struct.pack_into('<5I', data, 28, 224, name_size, 4, 4, 4)
    for i in range(15):
        struct.pack_into('<III', data, 56 + i * 12, 180, 0, 184)
    struct.pack_into('<III', data, 56, 180, 1, 184)
    struct.pack_into('<I', data, 236, 208)
    struct.pack_into('<IHHI', data, 252, 0, 0, 1, 0)
    data[280:280 + name_size] = name + b'\0'
    return data


def gr_fixture():
    sections = [b'prefix', bytes(fixture()), b'']
    offsets = [32]
    for section in sections:
        offsets.append(offsets[-1] + len(section))
    data = bytearray(offsets[-1] + 2)
    data[:4] = b'GR' + struct.pack('<H', len(sections))
    struct.pack_into('<4I', data, 4, *offsets)
    data[32:offsets[-1]] = b''.join(sections)
    data[-2:] = b'\xde\xad'
    return data


class BCHTests(unittest.TestCase):
    def test_names_and_section_relative_offsets(self):
        result = bch.inspect(fixture())
        self.assertEqual(result['resources']['models'][0]['name'], 'one')
        self.assertEqual(result['resources']['models'][0]['object_offset'], 264)
        self.assertEqual(result['resources']['textures'], [])

    def test_all_truncated_headers(self):
        for length in range(56):
            with self.assertRaises(ValueError):
                bch.inspect(fixture()[:length])

    def test_invalid_extents_and_indices(self):
        for offset, value in [(8, 0), (12, 60), (28, 0xffffffff),
                              (60, 65536), (56, 224), (64, 224),
                              (236, 224), (260, 4), (44, 3)]:
            with self.subTest(offset=offset, value=value):
                data = fixture()
                struct.pack_into('<I', data, offset, value)
                with self.assertRaises(ValueError):
                    bch.inspect(data)

    def test_tree_child_and_termination(self):
        data = fixture()
        struct.pack_into('<H', data, 256, 2)
        with self.assertRaises(ValueError):
            bch.inspect(data)
        data = fixture()
        data[283] = 65
        with self.assertRaises(ValueError):
            bch.inspect(data)

    def test_version_and_read_only(self):
        data = fixture()
        before = bytes(data)
        bch.inspect(data)
        self.assertEqual(bytes(data), before)
        data[4] = 0x21
        with self.assertRaises(ValueError):
            bch.inspect(data)


    def test_name_length_boundary(self):
        name = b'a' * 1024
        record = bch.inspect(fixture(name))['resources']['models'][0]
        self.assertEqual(record['name'], name.decode('ascii'))
        self.assertEqual(record['name_hex'], name.hex())
        with self.assertRaises(ValueError):
            bch.inspect(fixture(name + b'a'))


class GRTests(unittest.TestCase):
    def test_padded_sections_empty_section_and_read_only(self):
        data = gr_fixture()
        before = bytes(data)
        self.assertEqual(bch.extract_gr_section(data, 0), b'prefix')
        self.assertEqual(bch.extract_gr_section(data, 1), fixture())
        self.assertEqual(bch.extract_gr_section(data, 2), b'')
        self.assertEqual(bytes(data), before)

    def test_headers_counts_and_selections(self):
        for length in range(8):
            with self.subTest(length=length), self.assertRaises(ValueError):
                bch.extract_gr_section(gr_fixture()[:length], 0)
        for section in (-1, 3):
            with self.subTest(section=section), self.assertRaises(ValueError):
                bch.extract_gr_section(gr_fixture(), section)
        for header in (b'XX\x03\0', b'GR\0\0', b'GR\x01\x01'):
            data = gr_fixture()
            data[:4] = header
            with self.subTest(header=header), self.assertRaises(ValueError):
                bch.extract_gr_section(data, 1)

    def test_truncated_table_and_invalid_extents(self):
        with self.assertRaises(ValueError):
            bch.extract_gr_section(gr_fixture()[:19], 1)
        for offset, value in ((4, 19), (8, 31), (16, len(gr_fixture()) + 1)):
            data = gr_fixture()
            struct.pack_into('<I', data, offset, value)
            with self.subTest(offset=offset), self.assertRaises(ValueError):
                bch.extract_gr_section(data, 1)

    def test_input_budget(self):
        data = gr_fixture()
        with patch.object(bch, 'MAX_BYTES', len(data) - 1):
            with self.assertRaises(ValueError):
                bch.extract_gr_section(data, 1)

    def test_cli_standalone_and_gr_identity(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'input.bin'
            for data, selection in ((fixture(), []), (gr_fixture(), ['--gr-section', '1'])):
                path.write_bytes(data)
                with self.subTest(selection=selection):
                    result = subprocess.run(
                        [sys.executable, bch.__file__, '--file', str(path), *selection],
                        capture_output=True, text=True, timeout=10)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    self.assertEqual(result.stderr, '')
                    self.assertEqual(json.loads(result.stdout), bch.inspect(fixture()))

    def test_cli_invalid_inputs_report_errors(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'input.bin'
            for data, selection in ((gr_fixture(), '-1'), (gr_fixture(), '3'),
                                    (b'GR\x03\0' + bytes(8), '1'), (b'bad', '0')):
                path.write_bytes(data)
                with self.subTest(selection=selection, size=len(data)):
                    result = subprocess.run(
                        [sys.executable, bch.__file__, '--file', str(path),
                         '--gr-section', selection],
                        capture_output=True, text=True, timeout=10)
                    self.assertEqual(result.returncode, 1)
                    self.assertEqual(result.stdout, '')
                    self.assertTrue(result.stderr.startswith('error: '), result.stderr)
                    self.assertNotIn('Traceback', result.stderr)


if __name__ == '__main__':
    unittest.main()
