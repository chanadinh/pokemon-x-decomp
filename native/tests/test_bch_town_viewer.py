"""Black-box checks of multi-region --zone scenes (towns): a hand-authored
RomFS whose zone 3 spans two matrix cells, with a prop in each region, an NPC
and a warp in world tiles, added to the synthetic RomFS of
test_bch_zone_viewer. No game data."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_lighting_viewer import lit_fixture
from test_bch_viewer import texture_pack
from test_bch_walk_viewer import FLOOR
from test_bch_zone_viewer import NPC, entities, tiles, write_romfs, zone
from test_field_viewer import collision, garc, matrix, minipack
from test_model_viewer import VIEWER

# Row 21 runs from x=30 of region 0 into x=0..9 of region 1 (scene x=40..49).
LEFT = tuple((x, 21) for x in range(30, 40))
RIGHT = tuple((x, 21) for x in range(0, 10))


def town_matrix(zone_ids, width=3, height=2, fields=(-1, 0, 1, -1, -1, -1)):
    """MM with zone and LOD tables: field ids per cell, then (width*4) x
    (height*4) sub-cell zone ids (row-major), then LOD ids."""
    cells = width * height
    body = struct.pack('<4H', 1, 0, width, height) + struct.pack('<%dh' % cells, *fields)
    body += struct.pack('<%dh' % (cells * 16), *zone_ids) + struct.pack('<%dh' % cells, *([-1] * cells))
    cameras = struct.pack('<I4fi', 1, 0, 720, 0, 720, 0) + bytes(300)
    return minipack(b'MM', [body, cameras])


def zone_table(width=3, height=2, owner=3):
    """Cells (1,0) and (2,0) belong to `owner` except the last sub-column of
    cell (2,0), which is zone 7; the rest is no zone."""
    ids = [-1] * (width * 4 * height * 4)
    for z in range(4):
        for x in range(4, 12):
            ids[z * width * 4 + x] = 7 if x == 11 else owner
    return ids


def props(*records):
    out = struct.pack('<I', len(records))
    for uid, position, yaw in records:
        out += struct.pack('<I9fI', uid, 1, 1, 1, 0, yaw, 0, *position, 0)
    return out + bytes(8)  # padding, as the game's sections have


def registry(*pairs):
    out = struct.pack('<I', len(pairs))
    for reference, model in pairs:
        out += struct.pack('<HH', reference, model) + bytes(76)
    return out


def region(walkable, prop_records):
    room = lit_fixture()
    return minipack(b'GR', [tiles(walkable), room.finish(), collision(FLOOR), props(*prop_records), b'', b''], 128)


def write_town(root, with_registry=True):
    write_romfs(root)
    # World x = local + 360 + 720 * cell: cell 1 starts at world 1080 (tile 40).
    town = zone(0, 3, npcs=[{**NPC, 'x': 40 + 41, 'z': 21}],
                warps=[{'zone': 0, 'warp': 0, 'face': 0, 'x': 40 + 35, 'z': 20}])
    home = zone(0, 0, warps=[{'zone': 3, 'warp': 0, 'face': 1, 'x': 21, 'z': 23}])
    zones = garc([home, zone(0, 1), zone(0, 2), town])
    (root / 'a/0/1/2').write_bytes(zones)
    matrices = [matrix((0,)), matrix((1,)), matrix((0, 1), width=2), town_matrix(zone_table())]
    (root / 'a/0/4/2').write_bytes(garc(matrices))
    # Prop uid 9 (registry: model 1) in each region; uid 4 is not registered
    # and names a/0/2/4 entry 4, which does not exist.
    fields = [region(LEFT, [(9, (1260, 0, 378), 0), (4, (1120, 0, 360), 0)]),
              region(RIGHT, [(9, (1080 + 660, 0, 400), 90)])]
    (root / 'a/0/4/1').write_bytes(garc(fields))
    sections = [registry((9, 1)) if with_registry else b'', texture_pack()] + [b''] * 9 + [texture_pack()]
    (root / 'a/0/1/3').write_bytes(garc([minipack(b'AD', sections)]))
    prop_model = lit_fixture()
    (root / 'a/0/2/4').write_bytes(garc([b'\0', minipack(b'BM', [prop_model.finish()]), minipack(b'BM', [b'x'])]))


class BCHTownViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run([str(VIEWER), '--romfs', str(root), *map(str, arguments)], text=True,
                                capture_output=True, env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=120)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def test_regions_props_and_world_tiles(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_town(root)
            out = self.run_viewer(root, '--zone', 3, '--inspect').stdout
            self.assertIn('zone_regions zone=3 matrix=3 cells=2x1 origin_cell=1,0 scene=0 region=1@720,0 '
                          'tile_origin=40,0\n', out)
            # Scene tiles: world tiles less 40 per origin cell.
            self.assertIn('npc index=0 id=7 registry_model=40 model_entry=3 tile=41,21 ', out)
            self.assertIn('warp index=0 target_zone=0 target_warp=0 tiles=35,20+1x1 ', out)
            # Positions: world less 360 + 720 * origin cell; yaw in degrees.
            self.assertIn('prop index=0 region=0 uid=9 model_entry=a/0/2/4:1 position=180,0,18 yaw_degrees=0 '
                          'scale=1,1,1 drawn=yes\n', out)
            self.assertIn('prop index=1 region=0 uid=4 model_entry=a/0/2/4:4 position=40,0,0 yaw_degrees=0 '
                          'scale=1,1,1 drawn=no\n', out)
            self.assertIn('prop index=2 region=1 uid=9 model_entry=a/0/2/4:1 position=660,0,40 yaw_degrees=90 '
                          'scale=1,1,1 drawn=yes\n', out)
            self.assertIn('zone_props zone=3 props=3 models=2 drawn=2 not_drawn=1 registry=a/0/1/3:0:0 '
                          'textures=a/0/1/3:0:1 pose=rest\n', out)
            # Composite grid; the start is the first walkable tile of zone 3's
            # own sub-cells.
            self.assertIn('grid=80x40 standard_cells=20 ', out)
            self.assertIn('zone_walk zone=3 cell=30,21 ', out)
            # Walk east across the region seam to the NPC (blocked at 41,21).
            out = self.run_viewer(root, '--zone', 3, '--steps', 'R' * 12, '--inspect').stdout
            self.assertIn('zone_walk zone=3 cell=40,21 facing=1 moves=10 blocked=2 ', out)
            # The warp at scene tile (35,20) loads zone 0.
            out = self.run_viewer(root, '--zone', 3, '--steps', 'RRRRRU', '--inspect').stdout
            self.assertIn('warp from_zone=3 warp=0 target_zone=0 target_warp=0 result=loaded', out)
            # A matrix without a zone table: every cell with a region.
            out = self.run_viewer(root, '--zone', 2, '--inspect').stdout
            self.assertIn('zone_regions zone=2 matrix=2 cells=2x1 origin_cell=0,0 scene=0 region=1@720,0 '
                          'tile_origin=0,0\n', out)
            # Without the registry uid 9 names a/0/2/4 entry 9: not drawn.
            write_town(root, with_registry=False)
            out = self.run_viewer(root, '--zone', 3, '--inspect').stdout
            self.assertIn('zone_props zone=3 props=3 models=2 drawn=0 not_drawn=3 ', out)

    def test_runtime_props_bands_and_failures(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_town(root)
            shots = {}
            for name, extra in (('one', ('--threads', 1)), ('four', ('--threads', 4)), ('bare', ('--no-props',)),
                                ('far', ('--play', 'RRRRRRRRR'))):
                capture = root / f'{name}.bmp'
                self.run_viewer(root, '--zone', 3, '--software', '--frames', 120, '--capture-frame', capture,
                                '--capture-at', 119, *extra)
                shots[name] = capture.read_bytes()
            self.assertEqual(shots['one'], shots['four'])  # bands draw the same pixels
            self.assertNotEqual(shots['one'], shots['bare'])  # the props are drawn
            self.assertNotEqual(shots['one'], shots['far'])  # the camera follows across the seam
            for arguments in (('--threads', 0), ('--threads', 9), ('--threads', 2, '--threads', 2)):
                with self.subTest(arguments=arguments):
                    self.run_viewer(root, '--zone', 3, *arguments, '--inspect', success=False)
            # A zone whose bounding box starts on a cell that is not its own.
            ids = zone_table()
            for z in range(4, 8):
                for x in range(4, 8):
                    ids[z * 12 + x] = 3
            (root / 'a/0/4/2').write_bytes(garc([matrix((0,)), matrix((1,)), matrix((0, 1), width=2),
                                                 town_matrix(ids, fields=(-1, -1, 1, -1, 0, -1))]))
            failed = self.run_viewer(root, '--zone', 3, '--inspect', success=False)
            self.assertIn('the first cell of its bounding box is not its own', failed.stderr)
            # No cell of the matrix belongs to the zone.
            (root / 'a/0/4/2').write_bytes(garc([matrix((0,)), matrix((1,)), matrix((0, 1), width=2),
                                                 town_matrix(zone_table(owner=5))]))
            failed = self.run_viewer(root, '--zone', 3, '--inspect', success=False)
            self.assertIn('no cell of matrix 3 belongs to it', failed.stderr)


if __name__ == '__main__':
    unittest.main()
