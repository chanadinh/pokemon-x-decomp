"""Black-box checks of multi-region --zone scenes (towns): a hand-authored
RomFS whose zone 3 spans two matrix cells next to zone 4's cell, with props,
an NPC and a warp in world tiles, added to the synthetic RomFS of
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

# Row 21 runs from x=30 of region 0 through region 1 into x=0..9 of region 2
# (scene x=70..129); region 1 also has (1,22), where the NPC stands.
LEFT = tuple((x, 21) for x in range(30, 40))
MIDDLE = tuple((x, 21) for x in range(40)) + ((1, 22),)
RIGHT = tuple((x, 21) for x in range(0, 10))


def town_matrix(zone_ids, width=4, height=2, fields=(-1, 0, 1, 2, -1, -1, -1, -1)):
    """MM with zone and LOD tables: field ids per cell, then (width*4) x
    (height*4) sub-cell zone ids (row-major), then LOD ids."""
    cells = width * height
    body = struct.pack('<4H', 1, 0, width, height) + struct.pack('<%dh' % cells, *fields)
    body += struct.pack('<%dh' % (cells * 16), *zone_ids) + struct.pack('<%dh' % cells, *([-1] * cells))
    cameras = struct.pack('<I4fi', 1, 0, 720, 0, 720, 0) + bytes(300)
    return minipack(b'MM', [body, cameras])


def zone_table(width=4, height=2, owner=3):
    """Cells (1,0) and (2,0) belong to `owner`, cell (3,0) to zone 4; the rest
    is no zone."""
    ids = [-1] * (width * 4 * height * 4)
    for z in range(4):
        for x in range(4, 16):
            ids[z * width * 4 + x] = 4 if x >= 12 else owner
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


def matrices(town):
    return garc([matrix((0,)), matrix((1,)), matrix((0, 1), width=2), town])


def write_town(root, with_registry=True):
    write_romfs(root)
    # World x = local + 360 + 720 * cell: cell 1 starts at world 1080 (tile 40).
    town = zone(0, 3, npcs=[{**NPC, 'x': 80 + 1, 'z': 22}],
                warps=[{'zone': 0, 'warp': 0, 'face': 0, 'x': 40 + 35, 'z': 20}])
    home = zone(0, 0, warps=[{'zone': 3, 'warp': 0, 'face': 1, 'x': 21, 'z': 23}])
    (root / 'a/0/1/2').write_bytes(garc([home, zone(0, 1), zone(0, 2), town, zone(0, 3)]))
    (root / 'a/0/4/2').write_bytes(matrices(town_matrix(zone_table())))
    # Prop uid 9 (registry: model 1) in regions 0 and 1; uid 4 is not
    # registered and names a/0/2/4 entry 4, which does not exist.
    fields = [region(LEFT, [(9, (1260, 0, 378), 0), (4, (1120, 0, 360), 0)]),
              region(MIDDLE, [(9, (1080 + 660, 0, 400), 90)]), region(RIGHT, [])]
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
            # The zone's cells (1,0) and (2,0) and a ring of neighbours: cell
            # (3,0) of zone 4 is loaded too; the scene frame is cell (0,0).
            self.assertIn('zone_regions zone=3 matrix=3 cells=4x2 origin_cell=0,0 scene=0@720,0 region=1@1440,0 '
                          'region=2@2160,0 tile_origin=0,0\n', out)
            # Scene tiles: world tiles less 40 per origin cell.
            self.assertIn('npc index=0 id=7 registry_model=40 model_entry=3 tile=81,22 ', out)
            self.assertIn('warp index=0 target_zone=0 target_warp=0 tiles=75,20+1x1 ', out)
            # Positions: world less 360 + 720 * origin cell; yaw in degrees.
            self.assertIn('prop index=0 region=0 uid=9 model_entry=a/0/2/4:1 position=900,0,18 yaw_degrees=0 '
                          'scale=1,1,1 drawn=yes\n', out)
            self.assertIn('prop index=1 region=0 uid=4 model_entry=a/0/2/4:4 position=760,0,0 yaw_degrees=0 '
                          'scale=1,1,1 drawn=no\n', out)
            self.assertIn('prop index=2 region=1 uid=9 model_entry=a/0/2/4:1 position=1380,0,40 yaw_degrees=90 '
                          'scale=1,1,1 drawn=yes\n', out)
            self.assertIn('zone_props zone=3 props=3 models=2 drawn=2 not_drawn=1 registry=a/0/1/3:0:0 '
                          'textures=a/0/1/3:0:1 pose=rest\n', out)
            # Composite grid; the start is the first walkable tile of zone 3's
            # own sub-cells.
            self.assertIn('grid=160x80 standard_cells=61 ', out)
            self.assertIn('zone_walk zone=3 cell=70,21 ', out)
            # East across the region seam; the NPC below (81,22) blocks.
            out = self.run_viewer(root, '--zone', 3, '--steps', 'R' * 11 + 'D', '--inspect').stdout
            self.assertIn('zone_walk zone=3 cell=81,21 facing=0 moves=11 blocked=1 ', out)
            # On into zone 4's cell: the zone switches, keeping the world tile.
            out = self.run_viewer(root, '--zone', 3, '--steps', 'R' * 55, '--inspect').stdout
            self.assertIn('zone_cross from_zone=3 to_zone=4 world_tile=120,21 result=loaded cell=40,21\n', out)
            self.assertIn('zone_walk zone=4 cell=45,21 facing=1 moves=55 blocked=0 ', out)
            self.assertIn('zone_regions zone=4 matrix=3 cells=2x2 origin_cell=2,0 scene=2@720,0 region=1@0,0 '
                          'tile_origin=80,0\n', self.run_viewer(root, '--zone', 4, '--inspect').stdout)
            # And back: zone 3 again at the same world tile.
            out = self.run_viewer(root, '--zone', 3, '--steps', 'R' * 52 + 'L' * 3, '--inspect').stdout
            self.assertIn('zone_cross from_zone=4 to_zone=3 world_tile=119,21 result=loaded cell=119,21\n', out)
            # The warp at scene tile (75,20) loads zone 0.
            out = self.run_viewer(root, '--zone', 3, '--steps', 'RRRRRU', '--inspect').stdout
            self.assertIn('warp from_zone=3 warp=0 target_zone=0 target_warp=0 result=loaded', out)
            # A matrix without a zone table: every cell with a region.
            out = self.run_viewer(root, '--zone', 2, '--inspect').stdout
            self.assertIn('zone_regions zone=2 matrix=2 cells=2x1 origin_cell=0,0 scene=0@0,0 region=1@720,0 '
                          'tile_origin=0,0\n', out)
            # Without the registry uid 9 names a/0/2/4 entry 9: not drawn.
            write_town(root, with_registry=False)
            out = self.run_viewer(root, '--zone', 3, '--inspect').stdout
            self.assertIn('zone_props zone=3 props=3 models=2 drawn=0 not_drawn=3 ', out)

    def test_runtime_props_bands_crossing_and_failures(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_town(root)
            shots = {}
            for name, extra in (('one', ('--threads', 1)), ('four', ('--threads', 4)), ('bare', ('--no-props',)),
                                ('far', ('--play', 'RRRRRRRRR')), ('giant', ('--silly', 'giant'))):
                capture = root / f'{name}.bmp'
                self.run_viewer(root, '--zone', 3, '--software', '--frames', 120, '--capture-frame', capture,
                                '--capture-at', 119, *extra)
                shots[name] = capture.read_bytes()
            self.assertEqual(shots['one'], shots['four'])  # bands draw the same pixels
            self.assertNotEqual(shots['one'], shots['bare'])  # the props are drawn
            self.assertNotEqual(shots['one'], shots['far'])  # the camera follows across the seam
            self.assertNotEqual(shots['one'], shots['giant'])
            # Crossing during display; a moonwalk takes the same steps.
            for extra in ((), ('--silly', 'moonwalk')):
                result = self.run_viewer(root, '--zone', 3, '--play', 'r' * 52, '--software', '--frames', 600, *extra)
                self.assertIn('zone_cross from_zone=3 to_zone=4 world_tile=120,21 result=loaded cell=40,21\n',
                              result.stdout)
            for arguments in (('--threads', 0), ('--threads', 9), ('--threads', 2, '--threads', 2),
                              ('--silly', 'tiny'), ('--silly', 'giant', '--silly', 'giant')):
                with self.subTest(arguments=arguments):
                    self.run_viewer(root, '--zone', 3, *arguments, '--inspect', success=False)
            # A neighbouring region that fails to load is left out; the zone's
            # own regions must load. Zone 4 is no longer defined: walking into
            # its cell does not switch.
            (root / 'a/0/4/2').write_bytes(matrices(town_matrix(zone_table(), fields=(-1, 0, 1, 7, -1, -1, -1, -1))))
            out = self.run_viewer(root, '--zone', 3, '--steps', 'R' * 52, '--inspect')
            self.assertIn('region 7 failed to load (a neighbouring cell; left out)', out.stderr)
            self.assertIn('grid=160x80 standard_cells=51 ', out.stdout)
            (root / 'a/0/1/2').write_bytes(garc([zone(0, 0), zone(0, 1), zone(0, 2), zone(0, 3)]))
            (root / 'a/0/4/2').write_bytes(matrices(town_matrix(zone_table())))
            out = self.run_viewer(root, '--zone', 3, '--steps', 'R' * 55, '--inspect').stdout
            self.assertIn('zone_cross from_zone=3 to_zone=4 world_tile=120,21 result=not-loaded\n', out)
            self.assertEqual(out.count('zone_cross'), 1)  # not retried on every step
            self.assertIn('zone_walk zone=3 cell=125,21 ', out)
            (root / 'a/0/4/2').write_bytes(matrices(town_matrix(zone_table(), fields=(-1, 7, 1, 2, -1, -1, -1, -1))))
            failed = self.run_viewer(root, '--zone', 3, '--inspect', success=False)
            self.assertIn('member does not exist', failed.stderr)  # the zone's own region
            # No cell of the matrix belongs to the zone.
            (root / 'a/0/4/2').write_bytes(matrices(town_matrix(zone_table(owner=5))))
            failed = self.run_viewer(root, '--zone', 3, '--inspect', success=False)
            self.assertIn('no cell of matrix 3 belongs to it', failed.stderr)


if __name__ == '__main__':
    unittest.main()
