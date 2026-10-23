"""Hand-authored BCH triangle fixtures; no extracted game data or snapshots."""
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch


TOOLS = Path(__file__).resolve().parents[1] / 'tools'
sys.path.insert(0, str(TOOLS))
spec = importlib.util.spec_from_file_location('inspect_bch_model', TOOLS / 'inspect_bch_model.py')
bch_model = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bch_model)


def packet(register, *parameters, mask=15, consecutive=False):
    words = [parameters[0], register | mask << 16 | (len(parameters) - 1) << 20
             | int(consecutive) << 31, *parameters[1:]]
    if len(words) % 2:
        words.append(0)
    return struct.pack('<' + 'I' * len(words), *words)


def float_words(*numbers):
    return struct.unpack('<' + 'I' * len(numbers), struct.pack('<' + 'f' * len(numbers), *numbers))


class Fixture:
    """A small section allocator with explicitly authored pointer relocations."""

    bases = {'contents': 56, 'strings': 2104, 'commands': 2360, 'raw_data': 3384, 'relocation': 3640}
    sizes = {'contents': 2048, 'strings': 256, 'commands': 1024, 'raw_data': 256}

    def __init__(self):
        self.data = bytearray(3640)
        self.data[:8] = b'BCH\0\x07\x07\0\0'
        self.next = {name: 0 for name in self.sizes}
        self.next['contents'] = 180
        self.relocations = {}
        self.labels = {}
        self.empty = self.alloc('contents', bytes(12))
        for category in range(15):
            self.pointer(56 + category * 12, self.empty, 0)
            self.pointer(56 + category * 12 + 8, self.empty, 0)

    def alloc(self, section, content, alignment=4):
        relative = (self.next[section] + alignment - 1) // alignment * alignment
        if isinstance(content, int):
            content = bytes(content)
        at = self.bases[section] + relative
        assert relative + len(content) <= self.sizes[section]
        self.data[at:at + len(content)] = content
        self.next[section] = relative + len(content)
        return at

    def word(self, at, value):
        struct.pack_into('<I', self.data, at, value)

    def pointer(self, at, target, tag):
        section = 'strings' if tag == 1 else ('commands' if tag in (2, 3) else
                  'raw_data' if tag >= 0x24 else 'contents')
        self.word(at, target - self.bases[section])
        self.relocations[at] = tag

    def text(self, name):
        return self.alloc('strings', name + b'\0', alignment=1)

    def tree(self, names):
        at = self.alloc('contents', 12 * (len(names) + 1))
        for index, name in enumerate(names):
            self.pointer(at + (index + 1) * 12 + 8, name, 1)
        return at

    def dictionary(self, category, objects, names):
        table = self.alloc('contents', len(objects) * 4)
        for index, target in enumerate(objects):
            self.pointer(table + index * 4, target, 0)
        root = 56 + category * 12
        self.pointer(root, table, 0)
        self.word(root + 4, len(objects))
        self.pointer(root + 8, self.tree(names), 0)

    def stream(self, field, data):
        at = self.alloc('commands', data, alignment=8)
        self.pointer(field, at, 2)
        self.word(field + 4, len(data) // 4)
        return at

    def finish(self):
        data = bytearray(self.data)
        self.relocation_offsets = {}
        for field, tag in self.relocations.items():
            source = 'commands' if tag >= 0x24 else 'contents'
            relative = field - self.bases[source]
            assert tag == 1 or relative % 4 == 0
            self.relocation_offsets[field] = len(data)
            data.extend(struct.pack('<I', tag << 25 | relative // (1 if tag == 1 else 4)))
        sizes = {**self.sizes, 'relocation': len(data) - self.bases['relocation']}
        for index, section in enumerate(self.bases):
            struct.pack_into('<I', data, 8 + index * 4, self.bases[section])
            struct.pack_into('<I', data, 28 + index * 4, sizes[section])
        return data


def triangle_fixture(width=1, extra_enable=b'', extra_face=b''):
    f = Fixture()
    # Relative string offset zero is a real texture name, not a null pointer.
    texture = f.text(b'pattern')
    name, material_name, node_name = (f.text(x) for x in
                                     (b'triangle', b'surface', b'node\nmtllib injected'))
    model = f.alloc('contents', 0x98)
    material = f.alloc('contents', 0x58)
    mesh = f.alloc('contents', 56)
    sub = f.alloc('contents', 52)
    params = f.alloc('contents', 4)
    visibility = f.alloc('contents', b'\x01')
    f.dictionary(0, [model], [name])
    f.dictionary(1, [params], [material_name])
    struct.pack_into('<12f', f.data, model + 4, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0)
    for field, target in [(0x34, material), (0x3C, f.tree([material_name])),
                          (0x40, mesh), (0x7C, visibility), (0x8C, f.tree([node_name]))]:
        f.pointer(model + field, target, 0)
    for offset in (0x38, 0x44, 0x80, 0x88):
        f.word(model + offset, 1)
    f.pointer(model + 0x84, name, 1)
    f.pointer(material, params, 0)
    f.pointer(material + 0x48, texture, 1)
    f.pointer(material + 0x54, material_name, 1)
    material_commands = f.stream(material + 0x10, packet(0x80, 1))
    f.pointer(mesh + 16, sub, 0)
    f.word(mesh + 20, 1)
    f.stream(mesh + 24, packet(0x203, 0))

    vertices = f.alloc('raw_data', struct.pack('<15f',
                         -2, 3, 5, .5, -1,
                          7, -4, 2, 2, 3,
                         -1, 9, -6, -2, 4))
    indices = f.alloc('raw_data', struct.pack('<' + ('3H' if width == 2 else '3B'), 2, 0, 1))
    enable = (packet(0x200, 0x03000000, 0x7B, 0x10000000, 0, 0x10, 0x20140000,
                     consecutive=True)
              + packet(0x2BB, 0x40, 0, consecutive=True)
              + packet(0x242, 1, mask=1)
              + packet(0x2C0, 0x80000006, *float_words(0, 30, -20, 10), consecutive=True)
              + packet(0x2C0, 0x80000007)
              + packet(0x2C1, *float_words(1, 1, 1, 2, 1, 1, 1, .25))
              + extra_enable)
    enabled = f.stream(mesh + 8, enable)
    f.pointer(enabled + 16, vertices, 0x26)
    face = f.stream(sub + 44, packet(0x227, 0) + packet(0x228, 3)
                    + packet(0x25E, 0, mask=8) + extra_face)
    f.pointer(face, indices, 0x27 if width == 2 else 0x28)
    f.labels = dict(model=model, material=material, mesh=mesh, sub=sub,
                    vertices=vertices, indices=indices, enable=enabled, face=face,
                    material_commands=material_commands)
    return f


def texture_fixture(names=(b'pattern',)):
    f = Fixture()
    f.dictionary(3, [f.alloc('contents', 4) for _ in names], [f.text(name) for name in names])
    return f.finish()


class BCHModelTests(unittest.TestCase):
    def test_asymmetric_positions_uvs_scales_and_both_index_widths(self):
        positions = [(6., -14., 40.), (24., -28., 34.), (8., -2., 18.)]
        uv0 = [(.125, -.25), (.5, .75), (-.5, 1.)]
        for width in (1, 2):
            with self.subTest(width=width):
                data = triangle_fixture(width).finish()
                before = bytes(data)
                result = bch_model.decode_model(data)
                mesh = result['meshes'][0]
                self.assertEqual(mesh['positions'], positions)
                self.assertEqual(mesh['uv0'], uv0)
                self.assertEqual(mesh['indices'], [2, 0, 1])
                self.assertEqual(mesh['bounds'], {'min': [6., -28., 18.], 'max': [24., -2., 40.]})
                self.assertEqual(mesh['submeshes'][0]['index_width'], width)
                self.assertEqual((result['vertex_count'], result['index_count'], result['triangle_list_count']),
                                 (3, 3, 1))
                self.assertTrue(mesh['visible'])
                self.assertEqual(mesh['positions_sha256'], hashlib.sha256(
                    b''.join(struct.pack('<3d', *row) for row in positions)).hexdigest())
                self.assertEqual(bytes(data), before)

    def test_external_names_and_nullable_units(self):
        result = bch_model.decode_model(triangle_fixture().finish(), texture_data=texture_fixture())
        textures = result['materials'][0]['textures']
        self.assertEqual(textures[0]['texture']['name'], 'pattern')
        self.assertEqual(textures[0]['external_index'], 0)
        self.assertTrue(textures[0]['enabled'])
        self.assertIsNone(textures[1]['texture'])
        self.assertFalse(textures[1]['enabled'])
        self.assertIsNone(textures[2]['texture'])
        for names in [(b'Pattern',), (b'pattern', b'pattern'), ()]:
            with self.subTest(names=names), self.assertRaises(ValueError):
                bch_model.decode_model(triangle_fixture().finish(), texture_data=texture_fixture(names))

    def test_signed_integer_attributes_preserve_sign_and_scale(self):
        # Three signed position components, then two signed UV components.
        # Both packed formats deliberately contain negative non-palindromic data.
        raw = (-2, 3, 5, 2, -4, 7, -4, 2, 8, 12, -1, 9, -6, -8, 16)
        for fmt, nibble_pair, stride in [('b', 0x48, 5), ('h', 0x6A, 10)]:
            with self.subTest(format=fmt):
                f = triangle_fixture()
                f.word(f.labels['enable'] + 8, nibble_pair)
                f.word(f.labels['enable'] + 24, 0x20000000 | stride << 16)
                struct.pack_into('<15' + fmt, f.data, f.labels['vertices'], *raw)
                mesh = bch_model.decode_model(f.finish())['meshes'][0]
                self.assertEqual(mesh['positions'], [(6., -14., 40.), (24., -28., 34.), (8., -2., 18.)])
                self.assertEqual(mesh['uv0'], [(.5, -1.), (2., 3.), (-2., 4.)])

    def test_model_flags_transform_skinning_and_counts(self):
        cases = [('model', 0, 1), ('model', 4, 0x40000000), ('model', 16, 0x3F800000),
                 ('model', 0x6C, 1), ('model', 0x74, 1),
                 ('model', 0x38, 1025), ('model', 0x44, 1025),
                 ('model', 0x80, 2), ('model', 0x88, 1025),
                 ('mesh', 0, 1), ('mesh', 20, 0), ('mesh', 20, 1025),
                 ('sub', 0, 1), ('sub', 0, 0x10000)]
        for label, displacement, value in cases:
            with self.subTest(label=label, displacement=displacement, value=value):
                f = triangle_fixture()
                f.word(f.labels[label] + displacement, value)
                with self.assertRaises(ValueError):
                    bch_model.decode_model(f.finish())

    def test_required_pointer_relocations_and_zero_relative_address(self):
        f = triangle_fixture()
        self.assertEqual(struct.unpack_from('<I', f.data, f.labels['enable'] + 16)[0], 0)
        self.assertEqual(bch_model.decode_model(f.finish())['meshes'][0]['vertex_offset'],
                         f.bases['raw_data'])
        for label, displacement in [('model', 0x34), ('model', 0x40), ('model', 0x84),
                                     ('material', 0x10), ('mesh', 8), ('mesh', 16),
                                     ('enable', 16), ('face', 0)]:
            with self.subTest(label=label, displacement=displacement):
                f = triangle_fixture()
                del f.relocations[f.labels[label] + displacement]
                with self.assertRaises(ValueError):
                    bch_model.decode_model(f.finish())
        for root_entry in (True, False):
            f = triangle_fixture()
            field = 56 if root_entry else 56 + struct.unpack_from('<I', f.data, 56)[0]
            del f.relocations[field]
            with self.subTest(root_entry=root_entry), self.assertRaises(ValueError):
                bch_model.decode_model(f.finish())
        f = triangle_fixture()
        f.word(f.labels['material'] + 0x4C, 1)
        with self.assertRaises(ValueError):
            bch_model.decode_model(f.finish())

    def test_invalid_relocation_tags_duplicates_overlaps_and_extents(self):
        f = triangle_fixture()
        valid = f.finish()
        locations = list(f.relocation_offsets.values())
        cases = [(locations[0], 0x7F << 25),
                 (locations[0], 0x1FFFFFF),
                 (locations[1], struct.unpack_from('<I', valid, locations[0])[0]),
                 (f.relocation_offsets[f.labels['enable'] + 16],
                  0x24 << 25 | (f.labels['enable'] + 16 - f.bases['commands']) // 4),
                 (f.labels['model'] + 0x40, f.sizes['contents']),
                 (f.labels['enable'] + 16, f.sizes['raw_data']),
                 (f.labels['face'], f.sizes['raw_data'] - 1)]
        # A byte-addressed string relocation can overlap an existing pointer.
        overlap = 1 << 25 | (56 + 1 - f.bases['contents'])
        cases.append((locations[1], overlap))
        for at, value in cases:
            with self.subTest(at=at, value=value):
                data = bytearray(valid)
                struct.pack_into('<I', data, at, value)
                with self.assertRaises(ValueError):
                    bch_model.decode_model(data)

    def test_nonfinite_positions_scales_and_over_budget_counts(self):
        f = triangle_fixture()
        data = f.finish()
        for number in (float('nan'), float('inf'), 1e20):
            broken = bytearray(data)
            struct.pack_into('<f', broken, f.labels['vertices'], number)
            with self.subTest(number=number), self.assertRaises(ValueError):
                bch_model.decode_model(broken)
        for setting in ('MAX_VERTICES', 'MAX_INDICES', 'MAX_COMMAND_WORDS'):
            with self.subTest(setting=setting), patch.object(bch_model, setting, 2):
                with self.assertRaises(ValueError):
                    bch_model.decode_model(data)
        f.word(f.labels['face'] + 8, 0xFFFFFFFF)
        with self.assertRaises(ValueError):
            bch_model.decode_model(f.finish())

    def test_unproven_pointer_bytes_bad_topology_and_extra_vertex_buffers(self):
        cases = [dict(extra_enable=packet(0x203, 0, mask=1)),
                 dict(extra_enable=packet(0x224, 4)),
                 dict(extra_enable=packet(0x2C0, 0x80000006)
                      + packet(0x2C1, *float_words(float('nan'), 0, 0, 0))),
                 dict(extra_face=packet(0x25E, 1 << 8, mask=8)),
                 dict(extra_face=packet(0x228, 2))]
        for options in cases:
            with self.subTest(options=options), self.assertRaises(ValueError):
                bch_model.decode_model(triangle_fixture(**options).finish())

    def test_truncated_input_and_model_selection(self):
        data = triangle_fixture().finish()
        for length in (0, 55, 56, 100, len(data) - 1):
            with self.subTest(length=length), self.assertRaises(ValueError):
                bch_model.decode_model(data[:length])
        for model_index in (-1, 1):
            with self.subTest(model_index=model_index), self.assertRaises(ValueError):
                bch_model.decode_model(data, model_index)

    def test_obj_geometry_and_exclusive_creation(self):
        result = bch_model.decode_model(triangle_fixture().finish())
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            destination = root / 'triangle.obj'
            bch_model.write_obj(result, destination)
            lines = destination.read_text().splitlines()
            self.assertIn('v 6 -14 40', lines)
            self.assertIn('vt 0.125 -0.25', lines)
            self.assertIn('f 3/3 1/1 2/2', lines)
            self.assertFalse(any(line.startswith('mtllib ') for line in lines))
            original = destination.read_bytes()
            alias = root / 'alias.obj'
            alias.symlink_to(destination)
            dangling = root / 'dangling.obj'
            dangling.symlink_to(root / 'absent')
            for path in (destination, alias, dangling):
                with self.subTest(path=path), self.assertRaises(FileExistsError):
                    bch_model.write_obj(result, path)
                self.assertEqual(destination.read_bytes(), original)
            self.assertTrue(alias.is_symlink())
            self.assertTrue(dangling.is_symlink())
            self.assertFalse((root / 'absent').exists())

    def test_cli_summary_gr_selection_and_failed_export(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            data = triangle_fixture().finish()
            source = root / 'triangle.bch'
            source.write_bytes(data)
            gr = root / 'triangle.gr'
            gr.write_bytes(b'GR' + struct.pack('<HIII', 2, 16, 20, 20 + len(data)) + b'head' + data)
            command = [sys.executable, str(TOOLS / 'inspect_bch_model.py')]
            for args in (['--file', source], ['--file', gr, '--gr-section', '1']):
                result = subprocess.run(command + list(map(str, args)), text=True,
                                        capture_output=True, timeout=10)
                self.assertEqual(result.returncode, 0, result.stderr)
                output = json.loads(result.stdout)
                self.assertEqual(output['triangle_list_count'], 1)
                self.assertNotIn('positions', output['meshes'][0])
            result = subprocess.run(command + ['--file', str(source), '--triangle-list-obj', str(source)],
                                    text=True, capture_output=True, timeout=10)
            self.assertEqual(result.returncode, 1)
            self.assertEqual(source.read_bytes(), data)
            self.assertIn('error:', result.stderr)
            invalid = root / 'bad.bch'
            invalid.write_bytes(b'BCH')
            destination = root / 'never.obj'
            result = subprocess.run(command + ['--file', str(invalid), '--triangle-list-obj', str(destination)],
                                    text=True, capture_output=True, timeout=10)
            self.assertEqual(result.returncode, 1)
            self.assertFalse(destination.exists())


if __name__ == '__main__':
    unittest.main()
