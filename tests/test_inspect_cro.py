"""Invented CRO/CRS fixtures validate metadata interpretation and failure bounds."""
import importlib.util
from pathlib import Path
import struct
import unittest


ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('inspect_cro', ROOT / 'scripts/inspect_cro.py')
cro = importlib.util.module_from_spec(spec)
spec.loader.exec_module(cro)


def set_header(data, key, value):
    struct.pack_into('<I', data, 0x80 + cro.HEADER_FIELDS.index(key) * 4, value)


def fixture():
    data = bytearray(0x400)
    for key in cro.HEADER_FIELDS:
        set_header(data, key, 0x400 if key.endswith('_offset') else 0)
    values = {
        'magic': int.from_bytes(b'CRO0', 'little'), 'file_size': len(data), 'bss_size': 0x20,
        'name_offset': 0x138, 'module_name_offset': 0x138, 'module_name_size': 10,
        'nnro_control_object': 0xffffffff, 'on_load': 4 << 4, 'on_exit': 0xffffffff, 'on_unresolved': 0xffffffff,
        'code_offset': 0x200, 'code_size': 0x40, 'data_offset': 0x250, 'data_size': 0x10,
        'segment_table_offset': 0x148, 'segment_table_count': 5,
        'named_export_table_offset': 0x280, 'named_export_table_count': 2,
        'indexed_export_table_offset': 0x290, 'indexed_export_table_count': 1,
        'export_strings_offset': 0x2a0, 'export_strings_size': 15,
        'import_module_table_offset': 0x2e0, 'import_module_table_count': 1,
        'import_patches_offset': 0x300, 'import_patches_count': 2,
        'named_import_table_offset': 0x320, 'named_import_table_count': 1,
        'indexed_import_table_offset': 0x328, 'indexed_import_table_count': 1,
        'anonymous_import_table_offset': 0x330, 'anonymous_import_table_count': 1,
        'import_strings_offset': 0x340, 'import_strings_size': 14,
        'relocation_patches_offset': 0x380, 'relocation_patches_count': 2,
    }
    for key, value in values.items():
        set_header(data, key, value)
    data[0x138:0x142] = b'synthetic\0'
    for i, entry in enumerate(((0x200, 0x40, 0), (0x240, 0x10, 1), (0x250, 0x10, 2), (0, 0x20, 3), (0x260, 8, 1))):
        struct.pack_into('<III', data, 0x148 + 12 * i, *entry)
    struct.pack_into('<II', data, 0x280, 0x2a0, 5 << 4)
    struct.pack_into('<II', data, 0x288, 0x2a6, 4)
    struct.pack_into('<I', data, 0x290, (4 << 4) | 2)
    data[0x2a0:0x2af] = b'entry\0extra_ro\0'
    struct.pack_into('<5I', data, 0x2e0, 0x340, 0x328, 1, 0x330, 1)
    struct.pack_into('<IBBBBI', data, 0x300, 2, 2, 0, 0, 0, 0)
    struct.pack_into('<IBBBBI', data, 0x30c, (4 << 4) | 2, 2, 1, 0, 0, 0)
    struct.pack_into('<II', data, 0x320, 0x344, 0x300)
    struct.pack_into('<II', data, 0x328, 7, 0x30c)
    struct.pack_into('<II', data, 0x330, (8 << 4) | 0, 0x300)
    data[0x340:0x34e] = b'dep\0named_imp\0'
    struct.pack_into('<IBBBBI', data, 0x380, 0, 2, 4, 0, 0, 0)
    struct.pack_into('<IBBBBI', data, 0x38c, (4 << 4) | 0, 2, 3, 0, 0, 4)
    return data


class InspectCroTests(unittest.TestCase):
    def test_segment_table_indices_and_thumb_pointer_are_preserved(self):
        result = cro.parse_bytes(fixture())
        self.assertEqual(result['name'], 'synthetic')
        self.assertEqual(result['named_exports'][0]['offset'], 5)
        self.assertEqual(result['named_exports'][1]['segment_index'], 4)
        self.assertEqual(result['segments'][4]['type'], 1)
        self.assertEqual(result['patches']['internal'][0]['byte5'], 4)
        self.assertEqual(result['specials']['on_load']['offset'], 4)
        self.assertIsNone(result['specials']['on_exit'])
        self.assertEqual(result['warnings'], [])

    def test_import_ownership_and_terminal_batches(self):
        result = cro.parse_bytes(fixture())
        self.assertEqual(result['imports']['named'][0], {'name': 'named_imp', 'patch_index': 0, 'patch_count': 2})
        module = result['imports']['modules'][0]
        self.assertEqual(module['name'], 'dep')
        self.assertEqual(module['indexed'][0], {'index': 7, 'patch_index': 1, 'patch_count': 1})
        self.assertEqual(module['anonymous'][0]['offset'], 8)
        self.assertEqual(module['anonymous'][0]['patch_count'], 2)

    def test_crs_external_segments_are_not_treated_as_file_offsets(self):
        data = fixture()
        for index in (0, 1, 2, 4):
            struct.pack_into('<I', data, 0x148 + 12 * index, 0x100000 * (index + 1))
        result = cro.parse_bytes(data, 'static.crs')
        self.assertEqual(result['segments'][0]['offset'], 0x100000)
        with self.assertRaisesRegex(cro.FormatError, 'segment 0.*exceeds file'):
            cro.parse_bytes(data, 'wrong.cro')

    def test_rejects_large_table_before_iteration(self):
        data = fixture()
        set_header(data, 'named_export_table_count', 0xffffffff)
        with self.assertRaisesRegex(cro.FormatError, 'named_export_table.*exceeds file'):
            cro.parse_bytes(data)

    def test_rejects_unterminated_string_at_declared_boundary(self):
        data = fixture()
        data[0x138:0x142] = b'abcdefghij'
        with self.assertRaisesRegex(cro.FormatError, 'unterminated module name'):
            cro.parse_bytes(data)

    def test_rejects_patch_site_outside_segment(self):
        data = fixture()
        struct.pack_into('<I', data, 0x380, (0x3e << 4) | 0)
        with self.assertRaisesRegex(cro.FormatError, 'patch site extends outside segment'):
            cro.parse_bytes(data)

    def test_rejects_missing_relocation_target_segment(self):
        data = fixture()
        data[0x385] = 15
        with self.assertRaisesRegex(cro.FormatError, 'target segment index 15 is absent'):
            cro.parse_bytes(data)

    def test_rejects_unterminated_import_batch(self):
        data = fixture()
        data[0x311] = 0
        with self.assertRaisesRegex(cro.FormatError, 'unterminated import patch batch'):
            cro.parse_bytes(data)

    def test_rejects_module_import_subarray_outside_table(self):
        data = fixture()
        struct.pack_into('<I', data, 0x2e0 + 12, 0x338)
        with self.assertRaisesRegex(cro.FormatError, 'anonymous import subarray is outside'):
            cro.parse_bytes(data)

    def test_preserves_outside_target_addend_as_inspection_warning(self):
        data = fixture()
        struct.pack_into('<I', data, 0x388, 9)
        result = cro.parse_bytes(data)
        self.assertEqual(result['patches']['internal'][0]['addend'], 9)
        self.assertEqual(len(result['warnings']), 1)

    def test_unknown_patch_type_is_inspected_but_explicitly_warned(self):
        data = fixture()
        data[0x384] = 99
        result = cro.parse_bytes(data)
        self.assertIn('Unsupported patch type 99', result['warnings'][0])

    def test_negative_addend_preserves_raw_and_signed_interpretations(self):
        data = fixture()
        struct.pack_into('<I', data, 0x388, 0xfffffff4)
        result = cro.parse_bytes(data)
        patch = result['patches']['internal'][0]
        self.assertEqual(patch['addend'], 0xfffffff4)
        self.assertEqual(patch['signed_addend'], -12)
        self.assertIn('signed addend -12', result['warnings'][0])

    def test_unknown_static_symbol_table_fails_closed(self):
        data = fixture()
        set_header(data, 'static_anonymous_symbol_table_count', 1)
        with self.assertRaisesRegex(cro.FormatError, 'static anonymous symbol table is not supported'):
            cro.parse_bytes(data)


if __name__ == '__main__':
    unittest.main()
