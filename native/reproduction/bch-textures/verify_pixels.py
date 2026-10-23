#!/usr/bin/env python3
"""Compare the selected area's viewer pixels with an external AOSP ETC1 oracle.

Requires local game inputs and the separately built reference library documented
in ../texture-formats/reference-comparison.json. No reference source is vendored.
The new output directory contains local BMPs plus a metadata-only report.
"""
import argparse
import ctypes as C
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess


def digest(data):
    return hashlib.sha256(data).hexdigest()


def bmp_rgba(data, width, height):
    if data[:2] != b'BM' or struct.unpack_from('<ii', data, 18) != (width, height):
        raise ValueError('unexpected viewer BMP dimensions')
    if struct.unpack_from('<H', data, 28)[0] != 32:
        raise ValueError('expected 32-bit RGBA export')
    start = struct.unpack_from('<I', data, 10)[0]
    output = bytearray()
    for y in range(height):
        for x in range(width):
            at = start + ((height - 1 - y) * width + x) * 4
            blue, green, red, alpha = data[at:at + 4]
            output.extend((red, green, blue, alpha))
    return bytes(output)


def reference_pixels(payload, width, height, alpha, library):
    byte = C.c_uint8
    step = 16 if alpha else 8
    blocks = []
    for at in range(0, len(payload), step):
        block = payload[at:at + step]
        # AOSP accepts ETC1 MSB-first; PICA serializes the color word LE.
        rgb = (byte * 48)()
        library.etc1_decode_block((byte * 8).from_buffer_copy(block[-8:][::-1]), rgb)
        blocks.append((bytes(rgb), block[:8] if alpha else None))
    output = bytearray()
    for y in range(height):
        for x in range(width):
            tile = (y // 8) * (width // 8) + x // 8
            sub = ((y % 8) // 4) * 2 + (x % 8) // 4
            rgb, a4 = blocks[tile * 4 + sub]
            rgb_at = ((y % 4) * 4 + x % 4) * 3
            nibble = (x % 4) * 4 + y % 4
            a = ((a4[nibble // 2] >> (4 * (nibble % 2))) & 15) * 17 if a4 else 255
            output.extend((*rgb[rgb_at:rgb_at + 3], a))
    return bytes(output)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--romfs', type=Path, required=True)
    parser.add_argument('--texture-bch', type=Path, required=True)
    parser.add_argument('--viewer', type=Path, required=True)
    parser.add_argument('--reference-library', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    source = args.texture_bch.read_bytes()
    evidence = Path(__file__).resolve().parents[1]
    area = json.loads((evidence / 'bch-model/area75-texture-association.json').read_text())
    baseline = area['world_bch']
    if len(source) != baseline['size'] or digest(source) != baseline['sha256']:
        raise ValueError('texture BCH does not match selected baseline')
    library = C.CDLL(str(args.reference_library.resolve()))
    library.etc1_decode_block.argtypes = [C.POINTER(C.c_uint8), C.POINTER(C.c_uint8)]
    library.etc1_decode_block.restype = None
    args.output_dir.mkdir(exist_ok=False)
    base = [str(args.viewer.resolve()), '--romfs', str(args.romfs), '--archive', 'a/0/1/3',
            '--entry', '75', '--section', '11']
    samples = []
    for item in baseline['textures']:
        at, size = item['payload_offset'], item['payload_size']
        payload = source[at:at + size]
        if digest(payload) != item['payload_sha256']:
            raise ValueError('payload hash mismatch')
        width, height = item['width'], item['height']
        expected = reference_pixels(payload, width, height, item['format_code'] == 13, library)
        destination = args.output_dir / f"texture-{item['index']:02d}.bmp"
        command = base + ['--texture', str(item['index']), '--inspect', '--screenshot', str(destination)]
        result = subprocess.run(command, text=True, capture_output=True, timeout=30)
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
        actual = bmp_rgba(destination.read_bytes(), width, height)
        if actual != expected:
            raise ValueError(f"pixel mismatch for {item['name']}")
        samples.append({'index': item['index'], 'name': item['name'], 'width': width,
                        'height': height, 'format': item['format'], 'rgba_sha256': digest(actual),
                        'payload_sha256': digest(payload), 'pixel_count': width * height,
                        'full_image_equal': True})
    capture = args.output_dir / 'texture-09-frame.bmp'
    command = base + ['--texture', '9', '--software', '--frames', '3', '--self-test-input',
                      '--capture-frame', str(capture)]
    result = subprocess.run(command, text=True, capture_output=True, timeout=30,
                            env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'})
    if result.returncode:
        raise RuntimeError(result.stdout + result.stderr)
    report = {
        'result': 'passed', 'texture_bch_sha256': digest(source), 'textures_compared': len(samples),
        'pixels_compared': sum(item['pixel_count'] for item in samples), 'mismatches': 0,
        'archive': 'a/0/1/3', 'entry': 75, 'section': 11,
        'reference': json.loads((evidence / 'texture-formats/reference-comparison.json').read_text())['reference'],
        'reference_library_sha256': digest(args.reference_library.read_bytes()),
        'verifier_sha256': digest(Path(__file__).read_bytes()),
        'method': 'AOSP ETC1 RGB block decode; independent alpha-nibble and 8x8 tile reconstruction; compare every viewer BMP RGBA byte.',
        'runtime': {'backend': 'SDL dummy', 'frames': 3, 'input_self_test': 'passed',
                    'capture_sha256': digest(capture.read_bytes())},
        'samples': samples,
        'limitations': ['No game render or material equivalence', 'AOSP RGB reference is external; alpha/tile oracle is independently authored',
                       'Reference library must be built from the pinned source using recorded instructions']}
    if digest(args.texture_bch.read_bytes()) != digest(source):
        raise ValueError('source BCH changed during validation')
    with (args.output_dir / 'reference-comparison.json').open('x') as stream:
        json.dump(report, stream, indent=2)
        stream.write('\n')
    print(f"PASS: {len(samples)} textures / {report['pixels_compared']} pixels; dummy-video capture and input self-test")


if __name__ == '__main__':
    main()
