"""Black-box checks of --zone: a hand-authored RomFS with zone, matrix, field,
area, environment, character, motion and NPC-registry archives at the paths
the zone loader uses. No game data."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_lighting_viewer import UP, environment, lit_fixture
from test_bch_model import float_words, packet
from test_bch_player_viewer import add_texture
from test_bch_viewer import texture_pack
from test_bch_walk_viewer import FLOOR, WALKABLE
from test_field_viewer import collision, garc, matrix, minipack
from test_model_viewer import VIEWER


def lit_skinned_bch():
    """The lit triangle of the lighting tests, skinned as in
    test_bch_motion_viewer.skinned_bch (bones Origin and Arm), with a fixed
    normal (0, 1, 0) as a fifth attribute and its own ETC1 texture."""
    f = lit_fixture(normal=False)
    add_texture(f)
    model, mesh, sub = f.labels['model'], f.labels['mesh'], f.labels['sub']
    f.relocations.pop(f.labels['enable'] + 16)
    skin = [(1, 0, 2, 0), (0, 1, 1, 1), (0, 0, 2, 0)]
    xyz_uv = [(-2, 3, 5, .5, -1), (7, -4, 2, 2, 3), (-1, 9, -6, -2, 4)]
    vertices = f.alloc('raw_data', b''.join(struct.pack('<5f', *p) + bytes(s) for p, s in zip(xyz_uv, skin)))
    enable = (packet(0x200, 0x03000000, 0x557B, 0x40100000, 0, 0x3210, 0x40180000, consecutive=True)
              + packet(0x2BB, 0x18740, 0, consecutive=True)
              + packet(0x242, 4, mask=1)
              + packet(0x2C0, 0x80000006, *float_words(0, 0, 6, 0), consecutive=True)
              + packet(0x2C0, 0x80000007)
              + packet(0x2C1, *float_words(1, .5, .5, .5, .5, 1, 1, .25))
              + packet(0x232, 4) + packet(0x233, *UP, consecutive=True))
    enabled = f.stream(mesh + 8, enable)
    f.pointer(enabled + 16, vertices, 0x26)
    names = [f.text(b'Origin'), f.text(b'Arm')]
    bones = f.alloc('contents', 200)
    f.data[model] = 2
    f.data[model + 1] = 1
    f.pointer(model + 0x70, bones, 0)
    f.word(model + 0x74, 2)
    f.pointer(model + 0x78, f.tree(names), 0)
    for b in range(2):
        at = bones + b * 100
        f.word(at, 0x2800000)
        f.word(at + 4, 0 if b else 0xffff)
        struct.pack_into('<3f', f.data, at + 8, 1, 1, 1)
        struct.pack_into('<3f', f.data, at + 32, *((1, 2, 3) if b else (0, 0, 0)))
        struct.pack_into('<12f', f.data, at + 44, 1, 0, 0, -b, 0, 1, 0, -2 * b, 0, 0, 1, -3 * b)
        f.pointer(at + 92, names[b], 1)
    f.data[sub] = 1
    f.data[sub + 2] = 2
    f.data[sub + 6] = 1
    return f.finish()


def character_pack():
    """MM pack: the character BCH and a GF1 pack with slots 1, 3 and 4 (the
    zone loader's idle, walk and run): slot 1 holds Arm x at 1, slots 3 and 4
    move it from 1 to 5 over 4 frames."""
    def motion(codes, frames, lists, values):
        out = bytearray(struct.pack('<HH', len(codes), frames))
        for i in range(0, len(codes), 8):
            word = sum(c << (3 * j) for j, c in enumerate(codes[i:i + 8]))
            out += struct.pack('<I', word)[:3]
        for keys in lists:
            out += bytes([len(keys)] + keys)
        out += bytes((-len(out)) % 4)
        return bytes(out + struct.pack('<%df' % len(values), *values))
    skeleton = bytes([2, 1, 0, 0, 0]) + b'Arm\0'
    skeleton += bytes((-len(skeleton)) % 4) + struct.pack('<14f', 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 0, 0, 0, 1)
    hold = motion([0, 0, 1, 1, 1, 5], 2, [], [1])
    walk = motion([0, 0, 1, 1, 1, 6], 4, [[]], [1, 5])
    table = 4 + 4 * 6
    parts = [skeleton, hold, b'', walk, walk]
    offsets, at = [], table
    for part in parts:
        offsets.append(at if part else 0)
        at += len(part)
    offsets.append(at)
    pack = struct.pack('<I6I', 5, *offsets) + b''.join(parts)
    return minipack(b'MM', [lit_skinned_bch(), pack])


def tiles(walkable=WALKABLE):
    words = [0x01000021] * 1600
    for x, z in walkable:
        words[z * 40 + x] = 0x20
    return struct.pack('<HH', 40, 40) + struct.pack('<1600I', *words)


def field():
    room = lit_fixture()
    return minipack(b'GR', [tiles(), room.finish(), collision(FLOOR), b'', b'', b''], 128)


def entities(npcs=(), warps=()):
    """ZO section 1: entity header and records (pk3DS/CTRMap layout), then a
    stand-in for the zone script that the loader does not read."""
    body = b''
    for npc in npcs:
        record = bytearray(0x30)
        struct.pack_into('<8H', record, 0, npc['id'], npc['model'], 0, 0, 0, npc['script'], npc['face'], 0)
        struct.pack_into('<HHf', record, 0x28, npc['x'], npc['z'], 0)
        body += record
    for warp in warps:
        body += struct.pack('<2H2BH3h2hH2h', warp['zone'], warp['warp'], warp['face'], 5, 0,
                            warp['x'] * 18 + 9, 0, warp['z'] * 18 + 9, 1, 1, 0, 0, 0)
    header = struct.pack('<I4BI', 8 + len(body), 0, len(npcs), len(warps), 0, 0)
    return header + body + b'SCRIPT'


def zone(area, matrix_id, npcs=(), warps=()):
    header = bytearray(56)
    struct.pack_into('<HH', header, 2, area, matrix_id)
    return minipack(b'ZO', [header, entities(npcs, warps), b'', b''])


NPC = {'id': 7, 'model': 40, 'script': 12, 'face': 2, 'x': 22, 'z': 22}


def write_romfs(root, registry=True, npc_model=3):
    zones = [zone(0, 0, npcs=[NPC], warps=[{'zone': 1, 'warp': 0, 'face': 1, 'x': 21, 'z': 23}]),
             zone(0, 1, warps=[{'zone': 0, 'warp': 0, 'face': 3, 'x': 19, 'z': 21},
                               {'zone': 2, 'warp': 0, 'face': 0, 'x': 20, 'z': 20}]),
             zone(0, 2, warps=[{'zone': 0, 'warp': 0, 'face': 0, 'x': 0, 'z': 0}])]
    files = {
        'a/0/1/2': garc(zones),
        'a/0/4/2': garc([matrix((0,)), matrix((1,)), matrix((-1,))]),
        'a/0/4/1': garc([field(), field()]),
        'a/0/1/3': garc([minipack(b'AD', [b''] * 11 + [texture_pack()])]),
        'a/0/0/3': garc([b'\0'] * 137 + [minipack(b'BM', [environment()])]),
        'a/0/2/1': garc([b'\0', b'\0', character_pack(), character_pack()]),
        'a/1/4/9': garc([struct.pack('<H18xH2x', 40, npc_model) if registry else b'']),
    }
    for path, data in files.items():
        (root / path).parent.mkdir(parents=True, exist_ok=True)
        (root / path).write_bytes(data)


class BCHZoneViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run([str(VIEWER), '--romfs', str(root), *map(str, arguments)], text=True,
                                capture_output=True, env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=60)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def lines(self, text, prefix):
        return [line for line in text.split('\n') if line.startswith(prefix)]

    def test_zone_scene_and_scripted_steps(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_romfs(root)
            out = self.run_viewer(root, '--zone', 0, '--inspect').stdout
            self.assertIn('zone=0 area=0 matrix=0 region=0 furniture=0 npcs=1 warps=1 triggers=0,0 '
                          'environment=a/0/0/3:137 player=a/0/2/1:2 motions=idle:1,walk:3,run:4\n', out)
            self.assertIn('npc index=0 id=7 registry_model=40 model_entry=3 tile=22,22 face=2 script=12 '
                          'stored_height=0 drawn=yes posed=slot1\n', out)
            self.assertIn('warp index=0 target_zone=1 target_warp=0 tiles=21,23+1x1 face=1 transition=5 '
                          'directionality=0 height=0 supported=yes\n', out)
            self.assertIn('zone_walk zone=0 cell=20,21 facing=0 moves=0 blocked=0 warps=0', out)
            # R; D; R into the NPC (blocked, facing it); D into the warp: zone 1,
            # arriving on warp 0's tile (19,21) and stepping right to (20,21).
            out = self.run_viewer(root, '--zone', 0, '--steps', 'RDRD', '--inspect').stdout
            self.assertIn('warp from_zone=0 warp=0 target_zone=1 target_warp=0 result=loaded '
                          'arrival=cell:20,21 stepped=1\n', out)
            self.assertIn('zone_walk zone=1 cell=20,21 facing=1 moves=3 blocked=1 warps=1', out)
            # Zone 1's warp 1 targets a matrix without a region: not loaded,
            # the step falls back to a plain (blocked) move and the scene stays.
            back = self.run_viewer(root, '--zone', 1, '--arrive', 0, '--steps', 'U', '--inspect')
            self.assertIn('warp from_zone=1 warp=1 target_zone=2 target_warp=0 result=not-loaded\n', back.stdout)
            self.assertIn('zone 2: matrix 2 has no region', back.stderr)
            self.assertIn('zone_walk zone=1 cell=20,21 facing=2 moves=1 blocked=1 warps=1', back.stdout)

    def test_runtime_walk_talk_and_warp(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_romfs(root)
            captures = []
            for frame in (0, 10, 30):
                capture = root / f'frame{frame}.bmp'
                result = self.run_viewer(root, '--zone', 0, '--play', 'RDR.D', '--software', '--frames', 130,
                                         '--capture-frame', capture, '--capture-at', frame)
                captures.append(capture.read_bytes())
            self.assertEqual(len(set(captures)), 3)  # the player walks and the camera follows
            self.assertIn('talk zone=0 npc=0 id=7 script=12\n', result.stdout)
            self.assertIn('warp from_zone=0 warp=0 target_zone=1 target_warp=0 result=loaded', result.stdout)
            self.assertEqual(result.stdout.count('\ntalk '), 1)
            test = self.run_viewer(root, '--zone', 0, '--software', '--self-test-input')
            self.assertIn('Input self-test passed: orbit, zoom, reset, Escape, WASD queue, talk.', test.stderr)
            # Running (lower case) takes 5 motion frames per step, walking 11:
            # at rendered frame 14 (7 motion frames) the run has finished its
            # first step and started the second, the walk has not.
            shots = {}
            for play in ('rd', 'Rd', 'R'):
                capture = root / f'{play}.bmp'
                self.run_viewer(root, '--zone', 0, '--play', play, '--software', '--frames', 15,
                                '--capture-frame', capture, '--capture-at', 14)
                shots[play] = capture.read_bytes()
            self.assertNotEqual(shots['rd'], shots['R'])
            self.assertEqual(shots['Rd'], shots['R'])

    def test_npc_without_registry_and_failures(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_romfs(root, npc_model=9)  # registry names a missing a/0/2/1 entry
            out = self.run_viewer(root, '--zone', 0, '--steps', 'RDR', '--inspect')
            self.assertIn('model_entry=9 tile=22,22 face=2 script=12 stored_height=0 drawn=no posed=rest',
                          out.stdout)
            self.assertIn('zone_walk zone=0 cell=21,22 facing=1 moves=2 blocked=1', out.stdout)  # still blocks
            for arguments in (('--zone', 0, '--archive', 'a/0/4/1'), ('--zone', 0, '--textured'),
                              ('--zone', 0, '--walk'), ('--arrive', 0), ('--play', 'U'),
                              ('--zone', 0, '--play', 'X'), ('--zone', 0, '--capture-at', 1),
                              ('--zone', 0, '--zone', 1), ('--zone', 0, '--inspect', '--play', 'U')):
                with self.subTest(arguments=arguments):
                    extra = () if '--inspect' in arguments else ('--inspect',)
                    self.run_viewer(root, *arguments, *extra, success=False)
            self.run_viewer(root, '--zone', 0, '--arrive', 3, '--inspect', success=False)
            self.run_viewer(root, '--zone', 9, '--inspect', success=False)
            outdoor = self.run_viewer(root, '--zone', 2, '--inspect', success=False)
            self.assertIn('matrix 2 has no region', outdoor.stderr)
            destination = root / 'never.bmp'
            self.run_viewer(root, '--zone', 2, '--software', '--frames', 1, '--capture-frame', destination,
                            success=False)
            self.assertFalse(destination.exists())
            help_text = subprocess.run([str(VIEWER), '--help'], text=True, capture_output=True,
                                       check=True, timeout=15).stdout
            for option in ('--zone N', '--arrive N', '--play UDLR', '--capture-at N'):
                self.assertIn(option, help_text)


if __name__ == '__main__':
    unittest.main()
