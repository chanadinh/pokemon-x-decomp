"""Black-box checks of the platform layer in the display loop: the process
image (code.bin with and without an exheader), the save archive's atomic
write, the bottom screen and scripted taps, and the pad script. Synthetic
RomFS and code.bin only."""
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

from test_bch_battle_viewer import write_battle_romfs
from test_bch_save_viewer import write_code_bin
from test_model_viewer import VIEWER


def bmp_size(data):
    return struct.unpack_from('<ii', data, 18)


def cwav(rate=32000, samples=28, loop=1, loop_start=14):
    """A one-channel DSP-ADPCM BCWAV laid out as platform/audio.h reads it."""
    d = bytearray(0x200)
    d[0:4] = b'CWAV'
    struct.pack_into('<HHIIH', d, 4, 0xfeff, 0x40, 0x02010000, len(d), 2)
    struct.pack_into('<HHII', d, 0x14, 0x7000, 0, 0x40, 0xc0)
    struct.pack_into('<HHII', d, 0x20, 0x7001, 0, 0x100, 0x100)
    d[0x40:0x44] = b'INFO'
    struct.pack_into('<IBB2xIII', d, 0x44, 0xc0, 2, loop, rate, loop_start, samples)
    struct.pack_into('<IHHI', d, 0x5c, 1, 0x7100, 0, 0x0c)
    struct.pack_into('<HHIHHI', d, 0x68, 0x1f00, 0, 0, 0x0300, 0, 0x14)
    struct.pack_into('<h', d, 0x7c + 4, 2048)  # coefficient pair 1: c1 = 1.0
    d[0x100:0x104] = b'DATA'
    struct.pack_into('<I', d, 0x104, 0x100)
    d[0x108:0x118] = bytes([0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11] * 2)
    return bytes(d)


class BCHPlatformViewerTests(unittest.TestCase):
    def run_viewer(self, root, *arguments, success=True):
        result = subprocess.run([str(VIEWER), '--romfs', str(root), *map(str, arguments)], text=True,
                                capture_output=True, env={**os.environ, 'SDL_VIDEODRIVER': 'dummy'}, timeout=120)
        self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
        return result

    def test_platform_services(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_battle_romfs(root)
            code = root / 'code.bin'
            write_code_bin(code)
            common = ('--zone', 0, '--partner', 1, '--encounter-rate', 100, '--software')
            # Without an exheader code.bin is mapped flat at 0x00100000.
            out = self.run_viewer(root, *common, '--code-bin', code, '--steps', 'RR', '--inspect').stdout
            self.assertIn('process_image source=code.bin-flat regions=1 fault=none\n', out)
            # With one, its three segments; the tables are read at their addresses.
            pages = (len(code.read_bytes()) + 4095) // 4096
            image = code.read_bytes() + bytes(pages * 4096 - len(code.read_bytes()))
            exefs = root / 'exefs'
            exefs.mkdir()
            (exefs / 'code.bin').write_bytes(image)
            ex = bytearray(2048)
            text_pages, ro_pages = 0x460, pages - 0x460 - 1
            struct.pack_into('<III', ex, 0x10, 0x00100000, text_pages, text_pages * 4096)
            struct.pack_into('<III', ex, 0x20, 0x00100000 + text_pages * 4096, ro_pages, ro_pages * 4096)
            struct.pack_into('<III', ex, 0x30, 0x00100000 + (text_pages + ro_pages) * 4096, 1, 16)
            struct.pack_into('<I', ex, 0x3c, 64)
            (exefs / 'exheader.bin').write_bytes(bytes(ex))
            out = self.run_viewer(root, *common, '--code-bin', exefs / 'code.bin', '--steps', 'RR', '--inspect').stdout
            self.assertIn('process_image source=code.bin+exheader regions=3 fault=none\n', out)
            # A wrong exheader falls back to the flat mapping.
            struct.pack_into('<I', ex, 0x14, text_pages + 1)
            (exefs / 'exheader.bin').write_bytes(bytes(ex))
            out = self.run_viewer(root, *common, '--code-bin', exefs / 'code.bin', '--steps', 'RR', '--inspect').stdout
            self.assertIn('process_image source=code.bin-flat regions=1 fault=none\n', out)
            # The save archive writes atomically: no temporary file is left.
            save = root / 'saves' / 'game.sav'
            save.parent.mkdir()
            self.run_viewer(root, *common, '--code-bin', code, '--save', save, '--play', 's', '--frames', 3)
            self.assertTrue(save.exists())
            self.assertEqual(sorted(p.name for p in save.parent.iterdir()), ['game.sav'])
            # The bottom screen: two screens in one window, taps on its buttons.
            subprocess.run([str(VIEWER), '--demo', '--bottom-screen', '--inspect'], capture_output=True, check=False)
            self.assertNotEqual(subprocess.run([str(VIEWER), '--demo', '--bottom-screen', '--inspect'],
                                               capture_output=True).returncode, 0)  # needs --zone
            self.run_viewer(root, *common, '--touch-play', '5:1,1', '--frames', 10, success=False)  # no bottom
            self.run_viewer(root, *common, '--bottom-screen', '--touch-play', '5:1,1', success=False)  # no frames
            for bad in ('5:320,1', '5:1', '9:1,1;5:2,2', 'x', ''):
                self.run_viewer(root, *common, '--bottom-screen', '--touch-play', bad, '--frames', 10, success=False)
            capture = root / 'two.bmp'
            # Steps onto the grass, two message pages, then a tap on RUN.
            run = self.run_viewer(root, *common, '--bottom-screen', '--play', 'RR..', '--touch-play', '60:160,206',
                                  '--frames', 80, '--capture-frame', capture, '--capture-at', 50).stdout
            self.assertIn('touch x=160 y=206 phase=1\n', run)
            self.assertIn('battle turn=1 message="Got away safely!"\n', run)
            shot = capture.read_bytes()
            self.assertEqual(bmp_size(shot), (800, -960) if bmp_size(shot)[1] < 0 else (800, 960))
            # A tap between the buttons does nothing; FIGHT then a move runs a turn
            # (taps one frame apart: a touch held over frames is one press).
            fight = self.run_viewer(root, *common, '--bottom-screen', '--play', 'RR..',
                                    '--touch-play', '60:5,235;62:160,60;64:80,50', '--frames', 80).stdout
            self.assertIn('battle turn=1 message="Froakie used Pound!"\n', fight)
            # Audio: BCWAVs found inside a RomFS file, one played through the mixer.
            (root / 'sound').mkdir()
            (root / 'sound' / 'test.bin').write_bytes(bytes(0x40) + cwav() + b'CWAVjunk' + bytes(64))
            listing = self.run_viewer(root, '--list-sounds', 'sound/test.bin').stdout
            self.assertIn('sound offset=0x40 channels=1 encoding=dsp-adpcm rate=32000 samples=28 loop=1 loop_start=14\n',
                          listing)
            self.assertIn('sound offset=0x240 result="invalid BCWAV"\n', listing)
            self.assertIn('cwav_magics=2 decoded=1\n', listing)
            self.run_viewer(root, '--list-sounds', '../x', success=False)
            self.run_viewer(root, '--zone', 0, '--list-sounds', 'sound/test.bin', success=False)
            env = {**os.environ, 'SDL_VIDEODRIVER': 'dummy', 'SDL_AUDIODRIVER': 'dummy'}
            played = subprocess.run([str(VIEWER), '--romfs', str(root), *map(str, common), '--sound', 'sound/test.bin@0x40',
                                     '--frames', '5'], capture_output=True, text=True, env=env, timeout=120)
            self.assertEqual(played.returncode, 0, played.stderr)
            self.assertRegex(played.stdout, r'audio driver=\S+ mix_rate=32728 device_rate=\d+ voices=24 '
                                            r'sound=sound/test.bin@0x40 channels=1 encoding=dsp-adpcm rate=32000 '
                                            r'samples=28 loop=1\n')
            self.run_viewer(root, *common, '--sound', 'sound/test.bin@', '--frames', 5, success=False)
            # The pad script: 'x' is B and moves nobody (it once turned the player).
            still = self.run_viewer(root, '--zone', 0, '--software', '--play', 'xxxx', '--frames', 20).stdout
            self.assertNotIn('encounter ', still)
            self.run_viewer(root, '--zone', 0, '--software', '--play', 'R?', '--frames', 5, success=False)

    def test_vertex_shader_option(self):
        # The synthetic battle models carry no shader resources, so with
        # --vertex-shader every material keeps its stored colors: same frame.
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_battle_romfs(root)
            shots = []
            for extra in ((), ('--vertex-shader',)):
                capture = root / f'battle{len(shots)}.bmp'
                out = self.run_viewer(root, '--zone', 0, '--partner', 1, '--encounter-rate', 100, '--software',
                                      '--play', 'RR', '--frames', 40, '--capture-frame', capture,
                                      '--capture-at', 30, *extra).stdout
                self.assertIn('battle_model side=partner', out)
                shots.append(capture.read_bytes())
            self.assertEqual(shots[0], shots[1])


if __name__ == '__main__':
    unittest.main()
