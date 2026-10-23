#!/usr/bin/env python3
"""Compare the native owned geometry with the independent selected-input oracle.

Only metadata is written to the report. The frame capture stays local in a new
output directory. Neither a triangle interpretation nor capture proves game
topology, materials, visibility, camera, or running-game rendering equivalence.
"""
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import struct
import subprocess


def digest(data):
    return hashlib.sha256(data).hexdigest()


def packed(rows, code):
    return b''.join(struct.pack('<' + code * len(row), *row) for row in rows)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--file', type=Path, required=True)
    parser.add_argument('--bridge', type=Path, required=True)
    parser.add_argument('--viewer', type=Path, required=True)
    parser.add_argument('--romfs', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    evidence = Path(__file__).resolve().parents[1]
    reference_path = evidence / 'bch-model/verify_geometry_reference.py'
    prior = json.loads((evidence / 'bch-model/reference-comparison.json').read_text())
    assert digest(reference_path.read_bytes()) == prior['checker_sha256'], 'reference source changed'
    original = args.file.read_bytes()
    assert digest(original) == prior['input_sha256'] and len(original) == prior['input_size'], 'wrong selected BCH'
    spec = importlib.util.spec_from_file_location('independent_geometry_reference', reference_path)
    oracle = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(oracle)
    expected = oracle._decode_reference(original)
    result = subprocess.run([str(args.bridge.resolve()), str(args.file)],
                            capture_output=True, text=True, check=True, timeout=30)
    actual = json.loads(result.stdout)
    assert actual['vertex_count'] == expected['vertex_count'] == 2356
    assert actual['index_count'] == expected['index_count'] == 4992
    assert bytes.fromhex(actual['name_hex']).decode() == expected['model_name']
    assert len(actual['meshes']) == len(expected['meshes']) == 12
    records = []
    first_vertex = first_index = first_submesh = 0
    for native, reference in zip(actual['meshes'], expected['meshes']):
        vertex_count, index_count = reference['vertex_count'], reference['index_count']
        assert native['first_vertex'] == first_vertex and native['first_index'] == first_index
        assert native['vertex_count'] == vertex_count and native['index_count'] == index_count
        assert native['first_submesh'] == first_submesh
        assert native['submesh_count'] == len(reference['submeshes'])
        assert native['material_index'] == reference['material_index']
        assert bytes.fromhex(native['name_hex']).decode() == reference['name']
        positions = packed(actual['positions'][first_vertex:first_vertex + vertex_count], 'f')
        uvs = packed(actual['uv0'][first_vertex:first_vertex + vertex_count], 'f')
        indices = actual['indices'][first_index:first_index + index_count]
        local_indices = [value - first_vertex for value in indices]
        assert positions == packed(reference['positions'], 'f'), 'position mismatch'
        assert uvs == packed(reference['uv0'], 'f'), 'UV0 mismatch'
        assert local_indices == [value for sub in reference['submeshes'] for value in sub['indices']], 'index mismatch'
        for sub in reference['submeshes']:
            native_sub = actual['submeshes'][first_submesh]
            assert native_sub['first_index'] == first_index
            assert native_sub['index_count'] == sub['index_count']
            assert native_sub['index_width'] == sub['index_width']
            assert native_sub['draw_count'] == 1
            assert native_sub['primitive_known_mask'] & 0x300 == 0
            first_index += sub['index_count']
            first_submesh += 1
        records.append({'mesh_index': reference['mesh_index'], 'vertex_count': vertex_count,
                        'index_count': index_count, 'material_index': native['material_index'],
                        'positions_f32_sha256': digest(positions), 'uv0_f32_sha256': digest(uvs),
                        'local_indices_u32_sha256': digest(packed([local_indices], 'I')),
                        'positions_equal': True, 'uv0_equal': True, 'rebased_indices_equal': True})
        first_vertex += vertex_count
    assert first_submesh == len(actual['submeshes'])
    assert first_vertex == len(actual['positions']) == len(actual['uv0'])
    assert first_index == len(actual['indices'])
    recorded = json.loads((evidence / 'bch-model/field165-model.json').read_text())
    assert len(actual['materials']) == len(recorded['materials']) == 12
    for native, old in zip(actual['materials'], recorded['materials']):
        assert native['name_hex'] == old['name_hex']
        assert native['texture_unit_config'] == old['texture_unit_config']
        assert native['texture_names'] == [unit['texture']['name_hex'] if unit['texture'] else None
                                          for unit in old['textures']]
    args.output_dir.mkdir(exist_ok=False)
    command = [str(args.viewer.resolve()), '--romfs', str(args.romfs), '--archive', 'a/0/4/1',
               '--entry', '165', '--section', '1', '--triangle-list']
    result = subprocess.run(command + ['--inspect'], capture_output=True, text=True, check=True, timeout=30)
    assert 'vertices=2356 triangles=1664 shapes=12' in result.stdout
    assert 'topology=assumed-triangle-list' in result.stdout and 'Video=' not in result.stderr
    capture = args.output_dir / 'field165-wireframe.bmp'
    result = subprocess.run(command + ['--software', '--frames', '3', '--self-test-input',
                                      '--capture-frame', str(capture)], capture_output=True,
                            text=True, check=True, timeout=30,
                            env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'})
    assert 'Input self-test passed' in result.stderr
    bmp = capture.read_bytes()
    assert bmp[:2] == b'BM' and struct.unpack_from('<H', bmp, 28)[0] == 32
    at = struct.unpack_from('<I', bmp, 10)[0]
    width, height = struct.unpack_from('<ii', bmp, 18)
    assert 0 < width <= 4096 and 0 < height <= 4096
    assert len(bmp) == at + width * height * 4
    wire_pixels = sum(bmp[pos:pos + 3] == bytes((230, 211, 126))
                      for pos in range(at, len(bmp), 4))
    assert wire_pixels > 1000, 'capture has insufficient wireframe content'
    assert digest(args.file.read_bytes()) == digest(original), 'input changed'
    report = {
        'result': 'passed', 'input_sha256': digest(original), 'input_bytes': len(original),
        'mesh_count': 12, 'vertex_count': 2356, 'index_count': 4992,
        'comparison': 'Every position and UV0 compared as IEEE binary32; every index compared after per-mesh global rebasing. Independent reference uses binary64 arithmetic, then rounds once to binary32 to match the native contract.',
        'reference_checker_sha256': digest(reference_path.read_bytes()),
        'verifier_sha256': digest(Path(__file__).read_bytes()),
        'bridge_source_sha256': digest((Path(__file__).parent / 'dump_model.c').read_bytes()),
        'native_decoder_sha256': digest((evidence.parents[0] / 'assets/bch_model.c').read_bytes()),
        'material_metadata': 'All 12 names/configurations and 16 nonnull external texture names match prior evidence.',
        'draw_state': 'All 12 submeshes have one indexed draw and unknown primitive mode bits 8..9 at that draw.',
        'runtime': {'archive': 'a/0/4/1', 'entry': 165, 'section': 1,
                    'topology': 'explicit triangle-list interpretation', 'interpreted_triangles': 1664,
                    'backend': 'SDL dummy / software', 'frames': 3, 'input_self_test': 'passed',
                    'capture_width': width, 'capture_height': height, 'wire_pixels': wire_pixels,
                    'capture_sha256': digest(bmp)},
        'meshes': records, 'payload_arrays_included': False,
        'limitations': ['Original draw topology remains unverified', 'Wireframe includes every mesh regardless of visibility',
                       'No material, projected texture, lighting, camera or running-game render equivalence']}
    (args.output_dir / 'reference-comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print('PASS: 12 meshes / 2356 vertices / 4992 rebased indices; native wireframe capture and input self-test')


if __name__ == '__main__':
    main()
