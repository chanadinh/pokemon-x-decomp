"""Relocation arithmetic and copied-map integration tests, using synthetic data."""
import hashlib
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'scripts'))
from prepare_cro_analysis import PreparationError, prepare, relocated_word


class RelocationTests(unittest.TestCase):
    def test_explicit_addend_and_32_bit_negative_addend(self):
        self.assertEqual(relocated_word(2, 0x1000, 12, 0x2000, 0x77777777), 0x100c)
        self.assertEqual(relocated_word(2, 0x1000, 0xfffffff4, 0x2000, 0), 0xff4)
        self.assertEqual(relocated_word(3, 0x1000, 4, 0x2000, 0xffffffff), 0xfffff004)

    def test_unsupported_operation_is_not_silently_applied(self):
        with self.assertRaisesRegex(PreparationError, 'unsupported relocation type'):
            relocated_word(28, 0x1000, 0, 0x2000, 0)

    def test_preparation_links_both_directions_preserves_inputs_and_unknown_bss(self):
        # Parser bounds are covered independently. This fixture exercises the
        # preparation layer with already-parsed metadata and synthetic bytes.
        with tempfile.TemporaryDirectory(prefix='synthetic-cro-relocations-') as temp:
            root = Path(temp)
            modules = root / 'modules'; modules.mkdir()
            alpha_path = modules / 'Alpha.cro'; alpha_path.write_bytes(bytes([0xa5]) * 64)
            static_path = modules / 'static.crs'; static_path.write_bytes(b'SYNTHETIC CRS METADATA')
            code_path = root / 'code.bin'; code_path.write_bytes(bytes([0x5a]) * 80)
            main_map = root / 'main.json'
            main_map.write_text(json.dumps({
                'code': {'sha256': hashlib.sha256(code_path.read_bytes()).hexdigest()},
                'entry_point': 0x100000,
                'segments': [
                    {'name': '.text', 'address': 0x100000, 'size': 64, 'offset': 0, 'execute': True, 'write': False},
                    {'name': '.data', 'address': 0x200000, 'size': 16, 'offset': 64, 'execute': False, 'write': True},
                    {'name': '.bss', 'address': 0x200010, 'size': 16, 'offset': None, 'execute': False, 'write': True},
                ],
            }))

            def segment(index, offset, size, kind):
                return {'index': index, 'offset': offset, 'size': size, 'type': kind}

            def relocation(index, offset, kind, byte5, addend):
                return {'file_offset': index * 12, 'segment_index': 0, 'offset': offset,
                        'type': kind, 'byte5': byte5, 'byte6': 0, 'byte7': 0, 'addend': addend}

            def record(path, name, kind, segments):
                return {'path': str(path), 'name': name, 'kind': kind,
                        'sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
                        'segments': segments, 'header': {}, 'named_exports': [], 'indexed_exports': [],
                        'imports': {'named': [], 'modules': []},
                        'patches': {'internal': [], 'imports': [], 'static': []}}

            static = record(static_path, '|static|', 'crs', [
                segment(0, 0x100000, 64, 0), segment(1, 0, 0, 1), segment(2, 0x200000, 32, 2),
            ])
            alpha = record(alpha_path, 'Alpha', 'cro', [
                segment(0, 0, 32, 0), segment(1, 32, 16, 1),
                segment(2, 48, 16, 2), segment(3, 0, 8, 3),
            ])
            static['named_exports'] = [{'name': 'main_thumb_export', 'segment_index': 0, 'offset': 5}]
            alpha['named_exports'] = [{'name': 'alpha_export', 'segment_index': 0, 'offset': 16}]
            alpha['patches']['internal'] = [relocation(0, 0, 2, 1, 0xfffffffc), relocation(1, 4, 2, 3, 0)]
            alpha['patches']['imports'] = [relocation(2, 8, 2, 1, 3), relocation(3, 12, 3, 1, 0xfffffffc)]
            alpha['imports']['modules'] = [{'name': '|static|', 'indexed': [], 'anonymous': [
                {'segment_index': 0, 'offset': 4, 'patch_index': 0, 'patch_count': 1},
                {'segment_index': 0, 'offset': 16, 'patch_index': 1, 'patch_count': 1},
            ]}]
            static['patches']['imports'] = [relocation(0, 0, 2, 1, 4)]
            static['imports']['modules'] = [{'name': 'Alpha', 'indexed': [], 'anonymous': [
                {'segment_index': 0, 'offset': 12, 'patch_index': 0, 'patch_count': 1},
            ]}]
            records = {'Alpha.cro': alpha, 'static.crs': static}
            original = {p: p.read_bytes() for p in (alpha_path, static_path, code_path)}
            with patch('prepare_cro_analysis.parse_cro', side_effect=lambda p: records[p.name]):
                result = prepare(modules, code_path, main_map, root / 'prepared')
            self.assertEqual(result['relocation_summary']['applied'], 5)
            self.assertEqual(result['relocation_summary']['unresolved'], 0)
            main_text = next(b for b in result['blocks'] if b['name'] == 'main_text')
            alpha_text = next(b for b in result['blocks'] if b['name'] == 'Alpha_text_0')
            self.assertEqual(struct.unpack_from('<I', Path(main_text['path']).read_bytes())[0], 0x10000010)
            values = struct.unpack_from('<4I', Path(alpha_text['path']).read_bytes())
            self.assertEqual(values, (0x1000001c, 0x10300000, 0x00100007, (0x0010000c - 0x1000000c) & 0xffffffff))
            self.assertTrue(all(b['path'] is None for b in result['blocks'] if 'bss' in b['name']))
            thumb = next(s for s in result['symbols'] if s['name'] == 'main_thumb_export')
            self.assertEqual((thumb['address'], thumb['mode'], thumb['kind']), (0x100004, 'Thumb', 'function'))
            anonymous = [s for s in result['symbols'] if s['name'].startswith('anon_')]
            self.assertTrue(anonymous)
            self.assertTrue(all(s['kind'] == 'label' for s in anonymous))
            self.assertEqual({p: p.read_bytes() for p in original}, original)


if __name__ == '__main__':
    unittest.main()
