#!/usr/bin/env python3
"""Reproduce metadata for the selected field165 BCH baseline only.

Required input is a standalone BCH with the exact recorded hash, including its
GR section's alignment tail. This diagnostic is not a general BCH validator.
References read: CTRMap H3DModel/PICACommandReader and SPICA H3DMesh,
H3DSubMesh, H3DRelocator, PICACommandReader and VerticesConverter at the
revisions recorded in the report. No project parser is imported.

Only the selected v7, buffer-0, unskinned position/UV0 path is calculated.
Relocation uses an owned memory copy; original input bytes remain unchanged.
The report contains hashes, counts and bounds, never geometry payload arrays.
--output must name a new file. No rendering or runtime equivalence is claimed.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys


BASELINE_SHA256 = 'cb131a9590850d604b30aa558b7c55fba6fff654b2f6dd05c823af5ecbc16ccd'
BASELINE_SIZE = 127232


SEMANTICS = ('position', 'normal', 'tangent', 'color', 'uv0', 'uv1', 'uv2',
             'bone_index', 'bone_weight')
FORMATS = (('int8', 'b', 1), ('uint8', 'B', 1),
           ('int16', 'h', 2), ('float32', 'f', 4))


def digest(raw):
    return hashlib.sha256(raw).hexdigest()


def _decode_reference(data):
    def read(fmt, at):
        return struct.unpack_from('<' + fmt, data, at)

    def word(at):
        return read('I', at)[0]

    assert data[:6] == b'BCH\0\x07\x07'
    contents, strings, commands, raw, reloc = read('5I', 8)
    sizes = read('5I', 28)
    assert all(start + length <= len(data) for start, length in
               zip((contents, strings, commands, raw, reloc), sizes))

    # Apply the reference loader's relocations to an owned analysis copy. The
    # uint16 index flag is supplied by relocation target 7, not the raw word.
    original = data
    relocated = bytearray(original)
    section_bases = {0: contents, 1: strings, 2: commands, 3: commands,
                     4: raw, 5: raw, 6: raw, 7: raw | 0x80000000, 8: raw}
    for location in range(reloc, reloc + sizes[4], 4):
        entry = struct.unpack_from('<I', original, location)[0]
        source, target = entry >> 29, (entry >> 25) & 15
        displacement = entry & 0x1ffffff
        field = section_bases[source] + displacement * (1 if target == 1 else 4)
        value = struct.unpack_from('<I', original, field)[0] + section_bases[target]
        assert value <= 0xffffffff
        struct.pack_into('<I', relocated, field, value)
    data = bytes(relocated)

    def string(offset):
        at = offset
        end = data.index(b'\0', at, strings + sizes[1])
        return data[at:end].decode('utf-8')

    def command_state(relative, count):
        assert commands <= relative <= commands + sizes[2] - count * 4
        words = read('I' * count, relative)
        cursor = 0
        writes = []
        registers = {}
        uniform_values = {}
        uniform_cursor = None
        draw_issued = False
        while cursor < len(words):
            packet_start = cursor
            parameter, header = words[cursor:cursor + 2]
            cursor += 2
            extra = (header >> 20) & 2047
            parameters = (parameter,) + words[cursor:cursor + extra]
            assert len(parameters) == extra + 1
            cursor += extra
            cursor += cursor % 2
            register = header & 65535
            mask = (header >> 16) & 15
            byte_mask = sum(255 << (8 * byte) for byte in range(4)
                            if mask & (1 << byte))
            for parameter_index, value in enumerate(parameters):
                target = register + (parameter_index if header & 0x80000000 else 0)
                previous = registers.get(target, 0)
                registers[target] = (previous & ~byte_mask) | (value & byte_mask)
                writes.append({'register': target, 'mask': mask,
                               'parameter': value, 'packet_word': packet_start,
                               'after_draw': draw_issued})
                if target == 0x22f:
                    draw_issued = True
                if target == 0x2c0:
                    assert mask == 15 and value & 0x80000000
                    uniform_cursor = (value & 255) * 4
                elif 0x2c1 <= target <= 0x2c8:
                    assert mask == 15 and uniform_cursor is not None
                    index, component = divmod(uniform_cursor, 4)
                    assert index < 96
                    uniform_values.setdefault(index, [None] * 4)[3 - component] = \
                        struct.unpack('<f', struct.pack('<I', value))[0]
                    uniform_cursor += 1
        assert cursor == len(words)
        return registers, uniform_values, writes

    model_table, model_count, _ = read('3I', contents)
    assert model_count == 1
    model = word(model_table)
    model_name = string(word(model + 132))
    mesh_base, mesh_count = read('2I', model + 64)
    node_count, node_tree = read('2I', model + 136)
    names = [string(word(node_tree + 12 * (i + 1) + 8))
             for i in range(node_count)]
    output_meshes = []
    overall_vertices = bytearray()
    overall_indices = bytearray()
    combined = bytearray()
    for mesh_index in range(mesh_count):
        mesh_at = mesh_base + mesh_index * 56
        material, flags, node, key = read('4H', mesh_at)
        enable_offset, enable_count, submesh_offset, submesh_count = read('4I', mesh_at + 8)
        registers, uniforms, _ = command_state(enable_offset, enable_count)
        assert submesh_count > 0
        assert all(value is not None for index in (6, 7, 8) for value in uniforms[index])
        stride = (registers[0x205] >> 16) & 255
        attribute_count = registers[0x205] >> 28
        layout = registers[0x204] | ((registers[0x205] & 65535) << 32)
        permutation = registers[0x2bb] | (registers[0x2bc] << 32)
        formats = registers[0x201] | (registers[0x202] << 32)
        vertex_address = registers[0x203]
        descriptors = []
        byte_cursor = 0
        for physical_index in range(attribute_count):
            slot = (layout >> (4 * physical_index)) & 15
            assert not ((formats >> (48 + slot)) & 1)
            semantic = (permutation >> (4 * slot)) & 15
            value = (formats >> (4 * slot)) & 15
            type_name, struct_type, width = FORMATS[value & 3]
            elements = 1 + (value >> 2)
            if width > 1:
                byte_cursor += byte_cursor & 1
            scale = (uniforms[7][semantic] if semantic < 4 else
                     uniforms[8][semantic - 4] if semantic < 7 else
                     uniforms[8][3] if semantic == 8 else 1.0)
            descriptors.append({'slot': slot, 'semantic': SEMANTICS[semantic],
                                'format': type_name, 'elements': elements,
                                'offset': byte_cursor, 'scale': scale,
                                'struct_type': struct_type})
            byte_cursor += width * elements
        assert byte_cursor <= stride
        position = next(item for item in descriptors if item['semantic'] == 'position')
        uv = next(item for item in descriptors if item['semantic'] == 'uv0')
        assert position['elements'] == 3 and uv['elements'] == 2
        submeshes = []
        flattened_indices = []
        for submesh_index in range(submesh_count):
            sub_at = submesh_offset + submesh_index * 52
            skinning, bone_count = read('2H', sub_at)
            assert skinning == 0 and bone_count == 0
            draw_relative, draw_count = read('2I', sub_at + 44)
            draw_regs, _, draw_writes = command_state(draw_relative, draw_count)
            index_config = draw_regs[0x227]
            index_count = draw_regs[0x228]
            index_type = 'H' if index_config & 0x80000000 else 'B'
            index_width = 2 if index_type == 'H' else 1
            index_at = index_config & 0x7fffffff
            assert raw <= index_at <= raw + sizes[3] - index_count * index_width
            indices = list(read(index_type * index_count, index_at))
            flattened_indices.extend(indices)
            index_bytes = b''.join(struct.pack('<I', value) for value in indices)
            submeshes.append({'index': submesh_index, 'indices': indices,
                              'index_count': index_count, 'index_width': index_width,
                              'index_file_offset': index_at,
                              'indices_u32_sha256': digest(index_bytes),
                              'primitive_writes': [item for item in draw_writes
                                                   if item['register'] == 0x25e]})
        vertex_count = max(flattened_indices) + 1
        assert raw <= vertex_address <= raw + sizes[3] - vertex_count * stride
        positions = []
        uv0 = []
        for index in range(vertex_count):
            at = vertex_address + index * stride
            p = read(position['struct_type'] * position['elements'], at + position['offset'])
            t = read(uv['struct_type'] * uv['elements'], at + uv['offset'])
            positions.append([p[axis] * position['scale'] + uniforms[6][axis]
                              for axis in range(3)])
            uv0.append([value * uv['scale'] for value in t])
        vertex_bytes = b''.join(struct.pack('<5d', *p, *t) for p, t in zip(positions, uv0))
        index_bytes = b''.join(struct.pack('<I', value) for value in flattened_indices)
        overall_vertices.extend(vertex_bytes)
        overall_indices.extend(index_bytes)
        combined.extend(vertex_bytes)
        combined.extend(index_bytes)
        for descriptor in descriptors:
            del descriptor['struct_type']
        output_meshes.append({
            'mesh_index': mesh_index, 'name': names[node], 'material_index': material,
            'flags': flags, 'key': key, 'vertex_count': vertex_count,
            'index_count': len(flattened_indices), 'vertex_stride': stride,
            'vertex_file_offset': vertex_address,
            'attributes': descriptors,
            'position_offset': uniforms[6],
            'attribute_scales': {'uniform7': uniforms[7], 'uniform8': uniforms[8]},
            'position_bounds': {'min': [min(p[axis] for p in positions) for axis in range(3)],
                                'max': [max(p[axis] for p in positions) for axis in range(3)]},
            'uv0_bounds': {'min': [min(t[axis] for t in uv0) for axis in range(2)],
                          'max': [max(t[axis] for t in uv0) for axis in range(2)]},
            'positions': positions, 'uv0': uv0, 'submeshes': submeshes,
            'positions_sha256': digest(b''.join(struct.pack('<3d', *p) for p in positions)),
            'uv0_sha256': digest(b''.join(struct.pack('<2d', *t) for t in uv0)),
            'indices_sha256': digest(index_bytes),
            'positions_uv0_f64_sha256': digest(vertex_bytes),
            'indices_u32_sha256': digest(index_bytes),
            'vertices_then_indices_sha256': digest(vertex_bytes + index_bytes)})
    all_positions = [position for mesh in output_meshes for position in mesh['positions']]
    return {
        'input_sha256': digest(original), 'model_name': model_name, 'mesh_count': mesh_count,
        'vertex_count': sum(mesh['vertex_count'] for mesh in output_meshes),
        'index_count': sum(mesh['index_count'] for mesh in output_meshes),
        'position_bounds': {'min': [min(p[axis] for p in all_positions) for axis in range(3)],
                            'max': [max(p[axis] for p in all_positions) for axis in range(3)]},
        'canonical_encoding': 'Per vertex: little-endian float64 x,y,z,u,v; buffer order 0..max(raw index). '
                              'Indices: little-endian uint32, submesh order. Math uses binary64 after float32 input decoding.',
        'all_vertices_f64_sha256': digest(overall_vertices),
        'all_indices_u32_sha256': digest(overall_indices),
        'mesh_vertices_then_indices_sha256': digest(combined),
        'topology': 'undetermined: primitive register writes carry mask 8 and parameter 0 after draw; '
                    'they do not establish bits 8..9. Raw indices are preserved without triangle expansion.',
        'reference_sources': {
            'CTRMap': '4b780a7bc32a75a7f31c162829b086355caf32f2; H3DModel.java/PICACommandReader.java',
            'SPICA': 'bd29a7828595d7839cda2ac61c76bb63f9071250; H3DMesh.cs/H3DSubMesh.cs/H3DRelocator.cs/PICACommandReader.cs/VerticesConverter.cs'},
        'scope': 'Independent selected-baseline calculation with no native parser imports, rendering, '
                 'material binding, skinning, world transform application, or running-game equivalence.',
        'meshes': output_meshes}


def reference(data):
    """Return metadata for the exact selected baseline; reject other inputs."""
    if len(data) != BASELINE_SIZE or digest(data) != BASELINE_SHA256:
        raise ValueError('input differs from the selected field165 BCH baseline')
    result = _decode_reference(data)
    for mesh in result['meshes']:
        for key in ('positions', 'uv0', 'position_offset', 'attribute_scales'):
            del mesh[key]
        for submesh in mesh['submeshes']:
            del submesh['indices']
    result['checker_sha256'] = digest(Path(__file__).read_bytes())
    result['payload_arrays_included'] = False
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--file', type=Path, required=True,
                        help='standalone BCH matching the selected baseline SHA-256')
    parser.add_argument('--output', type=Path, required=True,
                        help='new metadata-only JSON output (existing files are refused)')
    args = parser.parse_args()
    try:
        with args.file.open('rb') as stream:
            data = stream.read(BASELINE_SIZE + 1)
        result = reference(data)
        encoded = json.dumps(result, indent=2, allow_nan=False) + '\n'
        with args.output.open('x', encoding='utf-8') as stream:
            stream.write(encoded)
    except (OSError, ValueError, AssertionError, IndexError, KeyError, struct.error) as error:
        print(f'error: {error or "selected-baseline invariant failed"}', file=sys.stderr)
        return 1
    print(f"Verified selected baseline: {result['mesh_count']} meshes, "
          f"{result['vertex_count']} vertices, {result['index_count']} indices.")
    return 0


if __name__ == '__main__':
    sys.exit(main())
