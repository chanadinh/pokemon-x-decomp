#!/usr/bin/env python3
"""Inspect a bounded static BCH 7/7 model; optionally export a triangle-list OBJ.

This decodes stored geometry, not a GPU draw or game scene. The OBJ option is an
explicit triangle-list interpretation: BCH primitive command parameters are
reported, but inherited GPU state, visibility, materials and shaders are not run.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import struct
import sys

from inspect_bch import MAX_BYTES, extract_gr_section, inspect
from pica_commands import decode_commands

MAX_MESHES = 1024
MAX_VERTICES = 262144
MAX_INDICES = 786432
MAX_COMMAND_WORDS = 262144
IDENTITY = (1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0.)
ATTRIBUTE_NAMES = ('position', 'normal', 'tangent', 'color', 'uv0', 'uv1', 'uv2')


class Reader:
    """Resolve only explicitly relocated pointers without modifying input bytes."""

    def __init__(self, data):
        self.data = data
        self.index = inspect(data)
        self.sections = self.index['sections']
        self.relocations = {}
        self.command_words = 0
        # Compatibility 7 relocation tags observed/reviewed in the references.
        kinds = {0: ('contents', 'contents'), 1: ('contents', 'strings'),
                 2: ('contents', 'commands'), 3: ('contents', 'commands'),
                 0x24: ('commands', 'raw_data'),
                 0x26: ('commands', 'raw_data'),
                 0x27: ('commands', 'raw_data'),
                 0x28: ('commands', 'raw_data')}
        rel = self.sections['relocation']
        if rel['size'] // 4 > 262144:
            raise ValueError('relocation count exceeds budget')
        occupied = []
        for at in range(rel['offset'], rel['offset'] + rel['size'], 4):
            word = self.u32(at)
            tag, displacement = word >> 25, word & 0x1ffffff
            if tag not in kinds:
                raise ValueError(f'unsupported v7 relocation tag 0x{tag:x}')
            source, target = kinds[tag]
            field = self.sections[source]['offset'] + displacement * (1 if tag == 1 else 4)
            self.span(field, 4, source)
            relative = self.u32(field)
            # Zero-length references may be one-past a section; actual reads
            # check their complete extent in pointer().
            self.span(self.sections[target]['offset'] + relative, 0, target)
            self.relocations[field] = (tag, target)
            occupied.append(field)
        occupied.sort()
        if any(b < a + 4 for a, b in zip(occupied, occupied[1:])):
            raise ValueError('duplicate or overlapping relocation fields')

    def span(self, at, size, section):
        s = self.sections[section]
        end = s['offset'] + s['size']
        if at < s['offset'] or size < 0 or at > end or size > end - at:
            raise ValueError(f'extent outside {section} at 0x{at:x}')

    def u32(self, at):
        return struct.unpack_from('<I', self.data, at)[0]

    def pointer(self, field, section, size, *, nullable=False, tags=None):
        relocation = self.relocations.get(field)
        if relocation is None:
            if nullable and self.u32(field) == 0:
                return None
            raise ValueError(f'missing relocation at 0x{field:x}')
        tag, target = relocation
        if target != section or (tags is not None and tag not in tags):
            raise ValueError(f'wrong relocation kind at 0x{field:x}')
        at = self.sections[section]['offset'] + self.u32(field)
        self.span(at, size, section)
        return at

    def name(self, field, *, nullable=False):
        at = self.pointer(field, 'strings', 1, nullable=nullable)
        if at is None:
            return None
        s = self.sections['strings']
        end = self.data.find(b'\0', at, min(at + 1025, s['offset'] + s['size']))
        if end < 0:
            raise ValueError('unterminated or over-budget name')
        raw = self.data[at:end]
        return {'name': raw.decode('utf-8', 'replace'), 'name_hex': raw.hex()}

    def tree(self, field, count):
        at = self.pointer(field, 'contents', 12 * (count + 1))
        names = []
        for i in range(count + 1):
            _, left, right = struct.unpack_from('<IHH', self.data, at + i * 12)
            if left > count or right > count:
                raise ValueError('tree child outside dictionary')
            if i:
                names.append(self.name(at + i * 12 + 8))
        return names

    def commands(self, field):
        count = self.u32(field + 4)
        self.command_words += count
        if not count or self.command_words > MAX_COMMAND_WORDS:
            raise ValueError('command word budget exceeded or empty stream')
        at = self.pointer(field, 'commands', count * 4)
        return decode_commands(self.data, at, count)


def value(commands, register, bits=0xffffffff):
    byte_mask = commands.register_known_masks.get(register, 0)
    known = sum(0xff << (i * 8) for i in range(4) if byte_mask & (1 << i))
    if known & bits != bits:
        raise ValueError(f'command register 0x{register:x} has unknown required bits')
    return commands.register_values[register] & bits


def raw_pointer(reader, commands, register, size, tags):
    # A relocated address must come from one complete write; masked assembly
    # of pointer bytes is outside this deliberately small accepted subset.
    source = commands.register_sources.get(register)
    if source is None:
        raise ValueError('raw buffer pointer lacks a complete command write')
    return reader.pointer(source, 'raw_data', size, tags=tags)


def finite(values):
    if any(not math.isfinite(v) or abs(v) > 1e12 for v in values):
        raise ValueError('nonfinite or over-budget coordinate')
    return values


def geometry_digest(positions, uv0, indices):
    """Canonical decoded numbers, independent of JSON spelling or source layout."""
    result = {}
    for name, rows, fmt in [('positions', positions, '<3d'), ('uv0', uv0, '<2d')]:
        digest = hashlib.sha256()
        for row in rows:
            digest.update(struct.pack(fmt, *row))
        result[name + '_sha256'] = digest.hexdigest()
    digest = hashlib.sha256()
    for index in indices:
        digest.update(struct.pack('<I', index))
    result['indices_sha256'] = digest.hexdigest()
    return result


def decode_mesh(reader, at, materials, node_names, visibility, budget):
    data = reader.data
    material, flags, node, key = struct.unpack_from('<4H', data, at)
    if flags or key & ~0x3ff or material >= len(materials) or node >= len(node_names):
        raise ValueError('unsupported mesh flags or invalid material/node/layer')
    finite(struct.unpack_from('<3f', data, at + 32))
    enable = reader.commands(at + 8)
    # Bounds/packet validation also covers disable streams, even though their
    # state is not used to decode this mesh's independent vertex buffer.
    reader.commands(at + 24)
    subcount = reader.u32(at + 20)
    if not 1 <= subcount <= 1024:
        raise ValueError('unsupported or over-budget submesh count')
    subs = reader.pointer(at + 16, 'contents', 52 * subcount)
    indices = []
    submeshes = []
    for i in range(subcount):
        sub = subs + i * 52
        skinning, bones = struct.unpack_from('<HH', data, sub)
        if skinning or bones:
            raise ValueError('skinned submeshes are unsupported')
        commands = reader.commands(sub + 44)
        count = value(commands, 0x228)
        budget['indices'] += count
        if not count or budget['indices'] > MAX_INDICES:
            raise ValueError('index count exceeds budget or is empty')
        source = commands.register_sources.get(0x227)
        tag = reader.relocations.get(source, (None,))[0]
        if tag not in (0x27, 0x28):
            raise ValueError('index width lacks a checked relocation')
        width = 2 if tag == 0x27 else 1
        start = raw_pointer(reader, commands, 0x227, count * width, (tag,))
        items = list(struct.unpack_from('<' + ('H' if width == 2 else 'B') * count, data, start))
        hints = [(w.value >> 8) & 3 for w in commands.writes if w.register == 0x25e and w.mask]
        masks = [w.mask for w in commands.writes if w.register == 0x25e and w.mask]
        if not hints or any(hints):
            raise ValueError('only encoded triangle-list primitive hints are supported')
        if count % 3:
            raise ValueError('index count is not a triangle-list multiple')
        submeshes.append({'index_count': count, 'index_offset': start,
                          'index_width': width, 'encoded_primitive_mode': 0,
                          'primitive_write_masks': masks})
        indices.extend(items)
    vertex_count = max(indices) + 1
    budget['vertices'] += vertex_count
    if budget['vertices'] > MAX_VERTICES:
        raise ValueError('vertex count exceeds budget')
    formats = value(enable, 0x201) | (value(enable, 0x202) << 32)
    permutation = value(enable, 0x2bb) | (value(enable, 0x2bc) << 32)
    config = value(enable, 0x205)
    order = value(enable, 0x204) | ((config & 0xffff) << 32)
    stride, stored_count = (config >> 16) & 255, config >> 28
    total = value(enable, 0x242, 0xff) + 1
    fixed_mask = (formats >> 48) & 0xfff
    if not 1 <= total <= 12 or total != (formats >> 60) + 1:
        raise ValueError('inconsistent vertex attribute counts')
    if not stride or not 1 <= stored_count <= total or stored_count + fixed_mask.bit_count() != total:
        raise ValueError('unsupported vertex buffer configuration')
    if fixed_mask >> total:
        raise ValueError('fixed attribute beyond declared inputs')
    for register in range(0x206, 0x227):
        if enable.register_known_masks.get(register, 0) and enable.register_values[register]:
            raise ValueError('multiple vertex buffers are unsupported')
    if set((6, 7, 8)) - enable.uniforms.keys():
        raise ValueError('missing position/attribute scale uniforms')
    position_offset = finite(enable.uniforms[6])
    scales = finite(enable.uniforms[7] + enable.uniforms[8])
    attributes = []
    used_slots, used_names = set(), set()
    byte = 0
    for i in range(stored_count):
        slot = (order >> (i * 4)) & 15
        if slot >= total or fixed_mask & (1 << slot) or slot in used_slots:
            raise ValueError('invalid or repeated attribute permutation')
        used_slots.add(slot)
        semantic = (permutation >> (slot * 4)) & 15
        if semantic >= len(ATTRIBUTE_NAMES) or semantic in used_names:
            raise ValueError('unsupported or repeated vertex attribute')
        used_names.add(semantic)
        fmt = (formats >> (slot * 4)) & 15
        kind, elements = fmt & 3, (fmt >> 2) + 1
        width = (1, 1, 2, 4)[kind]
        # Require natural alignment rather than guessing ambiguous PICA float
        # padding. The selected field's streams satisfy this stricter subset.
        if byte % width or stride % width:
            raise ValueError('unsupported attribute alignment')
        attributes.append({'semantic': ATTRIBUTE_NAMES[semantic], 'slot': slot,
                           'format': ('i8', 'u8', 'i16', 'f32')[kind],
                           'elements': elements, 'offset': byte,
                           'scale': scales[semantic], '_format': '<' + 'bBhf'[kind] * elements})
        byte += width * elements
    if byte > stride or 0 not in used_names:
        raise ValueError('missing position or attributes exceed vertex stride')
    for slot in range(total):
        if fixed_mask & (1 << slot):
            semantic = (permutation >> (slot * 4)) & 15
            if semantic not in (1, 2, 3, 5, 6) or semantic in used_names:
                raise ValueError('unsupported fixed attribute (position/UV0/skinning)')
            used_names.add(semantic)
    if 4 not in used_names:
        raise ValueError('missing stored UV0')
    start = raw_pointer(reader, enable, 0x203, vertex_count * stride, (0x26,))
    positions, uv0 = [], []
    for i in range(vertex_count):
        for attr in attributes:
            raw = finite(struct.unpack_from(attr['_format'], data, start + i * stride + attr['offset']))
            decoded = finite(tuple(v * attr['scale'] for v in raw))
            if attr['semantic'] == 'position':
                if len(decoded) != 3:
                    raise ValueError('position must have three components')
                positions.append(finite(tuple(decoded[j] + position_offset[j] for j in range(3))))
            elif attr['semantic'] == 'uv0':
                if len(decoded) != 2:
                    raise ValueError('UV0 must have two components')
                uv0.append(decoded)
    for attr in attributes:
        del attr['_format']
    bounds = {'min': [min(v[i] for v in positions) for i in range(3)],
              'max': [max(v[i] for v in positions) for i in range(3)]}
    return {**node_names[node], 'material_index': material, 'node_index': node,
            'visible': bool(visibility[node // 8] & (1 << (node % 8))),
            'layer': key >> 8, 'priority': key & 255,
            'vertex_offset': start, 'vertex_count': vertex_count, 'vertex_stride': stride,
            'index_count': len(indices), 'triangle_list_count': len(indices) // 3,
            'attributes': attributes, 'fixed_attribute_mask': fixed_mask,
            'position_offset': position_offset, 'bounds': bounds, 'submeshes': submeshes,
            **geometry_digest(positions, uv0, indices),
            'positions': positions, 'uv0': uv0, 'indices': indices}


def decode_model(data, model_index=0, texture_data=None):
    reader = Reader(data)
    models = reader.index['resources']['models']
    if not 0 <= model_index < len(models):
        raise ValueError('model selection outside resource table')
    record = models[model_index]
    base = reader.sections['contents']['offset']
    table = reader.pointer(base, 'contents', len(models) * 4)
    at = reader.pointer(table + model_index * 4, 'contents', 0x98)
    if at != record['object_offset']:
        raise ValueError('model pointer disagrees with resource index')
    reader.span(at, 0x98, 'contents')
    if reader.u32(at) or struct.unpack_from('<12f', data, at + 4) != IDENTITY:
        raise ValueError('only unskinned identity-transform models are supported')
    if reader.u32(at + 0x6c) or reader.u32(at + 0x74):
        raise ValueError('submesh culling and skeletons are unsupported')
    name = reader.name(at + 0x84)
    if name['name_hex'] != record['name_hex']:
        raise ValueError('model name disagrees with resource dictionary')
    count = reader.u32(at + 0x38)
    if not 1 <= count <= MAX_MESHES:
        raise ValueError('material count outside budget')
    material_at = reader.pointer(at + 0x34, 'contents', count * 0x58)
    names = reader.tree(at + 0x3c, count)
    materials = []
    parameter_objects = {m['object_offset'] for m in reader.index['resources']['materials']}
    for i in range(count):
        mat = material_at + i * 0x58
        own_name = reader.name(mat + 0x54)
        if own_name != names[i]:
            raise ValueError('material name disagrees with local dictionary')
        params = reader.pointer(mat, 'contents', 4)
        if params not in parameter_objects:
            raise ValueError('material parameters absent from resource index')
        commands = reader.commands(mat + 0x10)
        config = value(commands, 0x80)
        textures = []
        for unit in range(3):
            tex = reader.name(mat + 0x48 + unit * 4, nullable=True)
            textures.append({'unit': unit, 'enabled': bool(config & (1 << unit)), 'texture': tex})
        materials.append({**own_name, 'index': i, 'object_offset': mat,
                          'parameters_offset': params, 'textures': textures,
                          'texture_unit_config': config})
    mesh_count = reader.u32(at + 0x44)
    if not 1 <= mesh_count <= MAX_MESHES:
        raise ValueError('mesh count outside budget')
    meshes_at = reader.pointer(at + 0x40, 'contents', mesh_count * 56)
    node_count = reader.u32(at + 0x88)
    if not 1 <= node_count <= MAX_MESHES or reader.u32(at + 0x80) != node_count:
        raise ValueError('inconsistent or over-budget visibility/node counts')
    node_names = reader.tree(at + 0x8c, node_count)
    vis = reader.pointer(at + 0x7c, 'contents', (node_count + 7) // 8)
    visibility = data[vis:vis + (node_count + 7) // 8]
    budget = {'vertices': 0, 'indices': 0}
    meshes = [decode_mesh(reader, meshes_at + i * 56, materials, node_names, visibility, budget)
              for i in range(mesh_count)]
    texture_index = None
    if texture_data is not None:
        texture_index = inspect(texture_data)
        textures = texture_index['resources']['textures']
        by_name = {}
        for tex in textures:
            if tex['name_hex'] in by_name:
                raise ValueError('ambiguous duplicate external texture name')
            by_name[tex['name_hex']] = tex
        for material in materials:
            for unit in material['textures']:
                if unit['texture'] is not None:
                    match = by_name.get(unit['texture']['name_hex'])
                    if match is None:
                        raise ValueError('external texture name was not found')
                    unit['external_index'] = match['index']
                    unit['external_object_offset'] = match['object_offset']
    return {'sha256': reader.index['sha256'], 'size': len(data), 'model_index': model_index,
            **name, 'object_offset': at, 'mesh_count': mesh_count,
            'vertex_count': budget['vertices'], 'index_count': budget['indices'],
            'triangle_list_count': budget['indices'] // 3,
            'materials': materials, 'meshes': meshes,
            'external_texture_index_sha256': texture_index['sha256'] if texture_index else None,
            'scope': 'static positions/UV0/indices; triangle-list interpretation, not GPU execution; external matches are names only'}


def summary(model):
    return {**model, 'meshes': [{k: v for k, v in mesh.items() if k not in ('positions', 'uv0', 'indices')}
                              for mesh in model['meshes']]}


def write_obj(model, path):
    """Exclusive creation: never replace an asset, existing file or symlink."""
    with path.open('x', encoding='ascii', newline='\n') as stream:
        stream.write('# BCH inspection: explicit triangle-list interpretation; no materials or GPU execution\n')
        base = 1
        for i, mesh in enumerate(model['meshes']):
            # Stored resource strings are metadata, never executable OBJ syntax.
            stream.write(f'o mesh_{i}\n')
            for x, y, z in mesh['positions']:
                stream.write(f'v {x:.17g} {y:.17g} {z:.17g}\n')
            for u, v in mesh['uv0']:
                stream.write(f'vt {u:.17g} {v:.17g}\n')
            indices = mesh['indices']
            for j in range(0, len(indices), 3):
                stream.write('f ' + ' '.join(f'{base + k}/{base + k}' for k in indices[j:j + 3]) + '\n')
            base += mesh['vertex_count']


def read_file(path):
    with path.open('rb') as stream:
        data = stream.read(MAX_BYTES + 1)
    if len(data) > MAX_BYTES:
        raise ValueError('input exceeds 64 MiB')
    return data


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--file', type=Path, required=True)
    parser.add_argument('--gr-section', type=int)
    parser.add_argument('--model', type=int, default=0)
    parser.add_argument('--texture-bch', type=Path, help='check names against this external BCH index')
    parser.add_argument('--triangle-list-obj', type=Path, help='new OBJ using the explicit triangle-list interpretation')
    args = parser.parse_args()
    try:
        data = read_file(args.file)
        if args.gr_section is not None:
            data = extract_gr_section(data, args.gr_section)
        textures = read_file(args.texture_bch) if args.texture_bch else None
        model = decode_model(data, args.model, textures)
        if args.triangle_list_obj:
            write_obj(model, args.triangle_list_obj)
        print(json.dumps(summary(model), indent=2, allow_nan=False))
    except (OSError, ValueError, struct.error) as error:
        print(f'error: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
