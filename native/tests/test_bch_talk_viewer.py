"""Black-box checks of --zone dialogue: a hand-assembled zone script (Pawn 3.3
image in the GF encoding) and an encrypted story-text file added to the
synthetic RomFS of test_bch_zone_viewer. No game data."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_zone_viewer import NPC, entities, write_romfs, zone
from test_field_viewer import garc, minipack
from test_model_viewer import VIEWER
from test_viewer import bmp_pixel

PAPER = (250, 250, 246)


def encode(word):
    """GF word encoding: 7-bit groups, most significant first, continuation
    in bit 7, the first group's bit 6 is the sign; shortest form."""
    value = word - (1 << 32) if word >= 1 << 31 else word
    n = 1
    while not -(1 << (7 * n - 1)) <= value < 1 << (7 * n - 1):
        n += 1
    out = bytearray()
    for i in range(n - 1, -1, -1):
        out.append(((value >> (7 * i)) & 0x7F) | (0x80 if i else 0))
    return bytes(out)


def packed(op, value):
    return (op | (value & 0xFFFF) << 16) & 0xFFFFFFFF


def script(line, event=12):
    """main switches on the pubvar 0xD7477C97: `event` shows `line` through
    native 0x9ADF1616, anything else returns."""
    code = [packed(0xD2, 0), 0x2E, packed(0xA2, 0), 0x81, 0]  # HALT_P; PROC; LOAD_P_PRI 0; SWITCH
    case = len(code)
    code += [packed(0xBC, line), 0x87, 0, 4]                    # PUSH_P_C line; SYSREQ_N 0, 4
    end = len(code)
    code += [0x59, 0x30]                                        # ZERO_PRI; RETN
    table = len(code)
    code += [0x82, 1, 0, event, 0]                              # CASETBL 1 default; event
    code[4] = 4 * (table - 3)
    code[table + 2] = 4 * (end - (table + 2)) + 4
    code[table + 4] = 4 * (case - (table + 4)) + 4
    natives, pubvar = [0x9ADF1616], 0xD7477C97
    cod = 0x3c + 8 * len(natives) + 8 + 4
    dat, hea = cod + 4 * len(code), cod + 4 * len(code) + 4
    body = b''.join(encode(w & 0xFFFFFFFF) for w in code + [0])
    header = struct.pack('<IHBBHH12I', cod + len(body), 0xF1E0, 10, 10, 0x1C, 8, cod, dat, hea, hea + 256, 4,
                         0x3c, 0x3c, 0x3c + 8 * len(natives), 0x3c + 8 * len(natives),
                         0x3c + 8 * len(natives) + 8, 0x3c + 8 * len(natives) + 8, 0x3c + 8 * len(natives) + 8)
    tables = b''.join(struct.pack('<II', 0, h) for h in natives) + struct.pack('<II', 0, pubvar) + bytes(4)
    return header + tables + body


def text_file(lines):
    """pk3DS TextFile layout, each line encrypted with its key."""
    data = bytearray(0x10)
    blob = bytearray()
    table = bytearray()
    key = 0x7C89
    for line in lines:
        units = list(line) + [0]
        offset = 4 + 8 * len(lines) + len(blob)
        table += struct.pack('<IHH', offset, len(units), 0)
        k = key
        for u in units:
            blob += struct.pack('<H', u ^ k)
            k = ((k << 3) | (k >> 13)) & 0xFFFF
        if len(blob) % 4:
            blob += b'\0\0'
        key = (key + 0x2983) & 0xFFFF
    section = struct.pack('<I', 4 + len(table) + len(blob)) + table + blob
    struct.pack_into('<HHIII', data, 0, 1, len(lines), len(section), 0, 0x10)
    return bytes(data + section)


def units(text):
    out = []
    for part in text.split('\f'):
        if out:
            out += [0x10, 1, 0xBE01]
        out += [ord(c) for c in part]
    return out


LINES = [units('Hello'), units('Hi there!\nNice to meet you.\fSecond page.')]


def add_dialogue(root, text=True):
    """Zone 0 of the synthetic RomFS gains a section-2 script and text id 3."""
    header = bytearray(56)
    struct.pack_into('<HHH', header, 2, 0, 0, 3)
    zone0 = minipack(b'ZO', [header, entities([NPC], [{'zone': 1, 'warp': 0, 'face': 1, 'x': 21, 'z': 23}]),
                             script(1), b''])
    zones = [zone0, zone(0, 1, warps=[{'zone': 0, 'warp': 0, 'face': 3, 'x': 19, 'z': 21}]), zone(0, 2)]
    (root / 'a/0/1/2').write_bytes(garc(zones))
    if text:
        (root / 'a/0/8').mkdir(parents=True, exist_ok=True)
        (root / 'a/0/8/2').write_bytes(garc([b'\0', b'\0', b'\0', text_file(LINES)]))


class BCHTalkViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run([str(VIEWER), '--romfs', str(root), *map(str, arguments)], text=True,
                                capture_output=True, env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=60)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def paper(self, path):
        data = path.read_bytes()
        return sum(bmp_pixel(data, x, y) == PAPER for y in range(380, 480, 2) for x in range(0, 800, 2))

    def test_dialogue(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_romfs(root)
            add_dialogue(root)
            out = self.run_viewer(root, '--zone', 0, '--inspect').stdout
            self.assertIn('dialogue zone=0 script=yes text=a/0/8/2:3:yes lines=2 natives=policy-inferred '
                          'female=1\n', out)
            self.assertIn('npc_dialogue index=0 script=12 result=halted steps=', out)
            self.assertIn(' lines=1\n', out)
            # Walk to the NPC (R, D, R into it) and talk; then turn the pages.
            capture = root / 'talk.bmp'
            result = self.run_viewer(root, '--zone', 0, '--play', 'RDR.', '--software', '--frames', 80,
                                     '--capture-frame', capture, '--capture-at', 79)
            self.assertIn('talk zone=0 npc=0 id=7 script=12\n', result.stdout)
            self.assertIn('dialogue zone=0 script=12 result=halted lines=1\n', result.stdout)
            self.assertIn('message zone=0 line=1 page=1 text="Hi there!\\nNice to meet you."\n', result.stdout)
            self.assertIn('message zone=0 line=1 page=2 text="Second page."\n', result.stdout)
            self.assertGreater(self.paper(capture), 500)  # the window is open
            closed = root / 'closed.bmp'
            self.run_viewer(root, '--zone', 0, '--play', 'RDR...', '--software', '--frames', 80,
                            '--capture-frame', closed, '--capture-at', 79)
            self.assertEqual(self.paper(closed), 0)  # two pages turned: closed
            # While the window is open the player does not walk.
            held = self.run_viewer(root, '--zone', 0, '--play', 'RDR.D', '--software', '--frames', 120)
            self.assertNotIn('warp from_zone=0', held.stdout)
            walked = self.run_viewer(root, '--zone', 0, '--play', 'RDR...D', '--software', '--frames', 120)
            self.assertIn('warp from_zone=0 warp=0', walked.stdout)

    def test_without_text(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_romfs(root)
            add_dialogue(root, text=False)
            out = self.run_viewer(root, '--zone', 0, '--inspect').stdout
            self.assertIn('dialogue zone=0 script=yes text=a/0/8/2:3:no lines=0', out)
            result = self.run_viewer(root, '--zone', 0, '--play', 'RDR.', '--software', '--frames', 60)
            self.assertIn('talk zone=0 npc=0', result.stdout)
            self.assertNotIn('message zone=', result.stdout)


if __name__ == '__main__':
    unittest.main()
