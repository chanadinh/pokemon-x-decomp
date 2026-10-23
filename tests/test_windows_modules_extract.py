"""Synthetic RomFS module-packaging test; no game code or assets are used."""
import hashlib
import json
import os
from pathlib import Path
import shlex
import struct
import subprocess
import tempfile
import unittest
import zipfile


ROOT = Path(__file__).resolve().parents[1]
PWSH = Path(os.environ.get('POKEMON_X_PWSH', ROOT / 'tools/pwsh/pwsh'))
TOOL = Path(os.environ.get('POKEMON_X_3DSTOOL', ROOT / 'tools/3dstool-linux/3dstool'))
SCRIPT = ROOT / 'windows/Extract-PokemonX-Modules.ps1'


class ModulesExtractionTests(unittest.TestCase):
    def test_modules_preserve_paths_exclude_assets_and_preserve_input(self):
        for path in (PWSH, TOOL, SCRIPT):
            if not path.is_file():
                self.skipTest(f'Required tool/script missing: {path}')
        with tempfile.TemporaryDirectory(prefix='synthetic-x-modules-') as temp:
            base = Path(temp)
            tree = base / 'source tree'
            expected = {
                'DllTest.cro': b'SYNTHETIC CRO BYTES\0',
                'nested/Another.cro': b'ANOTHER SYNTHETIC CRO\0',
                'static.crs': b'SYNTHETIC CRS BYTES\0',
                '.crr/static.crr': b'SYNTHETIC CRR BYTES\0',
            }
            assets = {'textures/example.bcres': b'EXCLUDE THIS SYNTHETIC ASSET', 'readme.txt': b'EXCLUDE THIS TEXT'}
            for name, data in (expected | assets).items():
                path = tree / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(data)
            romfs = base / 'romfs.bin'
            result = subprocess.run([str(TOOL), '-cvtf', 'romfs', str(romfs), '--romfs-dir', str(tree)],
                                    capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            raw_romfs = romfs.read_bytes()
            padded_size = (len(raw_romfs) + 511) // 512 * 512
            ncch = bytearray(0xA00 + padded_size)
            ncch[0x100:0x104] = b'NCCH'
            struct.pack_into('<I', ncch, 0x104, len(ncch) // 512)
            struct.pack_into('<Q', ncch, 0x118, 0x0004000000055D00)
            ncch[0x150:0x15A] = b'CTR-P-TEST'
            ncch[0x18D] = 3
            ncch[0x18F] = 4
            struct.pack_into('<III', ncch, 0x1B0, 0xA00 // 512, padded_size // 512, 1)
            ncch[0xA00:0xA00 + len(raw_romfs)] = raw_romfs
            previous = base / 'prior extraction'
            (previous / 'work').mkdir(parents=True)
            main = previous / 'work/main.cxi'
            main.write_bytes(ncch)
            (previous / 'upload').mkdir()
            original_manifest = b'{"fixture":"synthetic only"}\n'
            (previous / 'upload/manifest.json').write_bytes(original_manifest)
            before = hashlib.sha256(main.read_bytes()).hexdigest()
            main_mtime = main.stat().st_mtime_ns
            env = os.environ.copy()
            for key, suffix in [('XDG_CACHE_HOME', 'cache'), ('XDG_CONFIG_HOME', 'config'), ('XDG_DATA_HOME', 'data')]:
                env[key] = str(base / 'powershell-state' / suffix)
                Path(env[key]).mkdir(parents=True)
            result = subprocess.run([
                str(PWSH), '-NoLogo', '-NoProfile', '-NonInteractive', '-File', str(SCRIPT),
                '-ExtractionFolder', str(previous), '-OutputRoot', str(base / 'output with spaces'),
                '-ToolPath', str(TOOL),
            ], capture_output=True, text=True, timeout=90, env=env)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertEqual(hashlib.sha256(main.read_bytes()).hexdigest(), before)
            self.assertEqual((previous / 'upload/manifest.json').read_bytes(), original_manifest)
            zips = list((base / 'output with spaces').rglob('PokemonX-modules-upload.zip'))
            self.assertEqual(len(zips), 1, result.stdout + result.stderr)
            upload_zip = zips[0]
            job = upload_zip.parent
            saved_manifest = (job / 'manifest.json').read_bytes()

            def assert_archive():
                with zipfile.ZipFile(upload_zip) as archive:
                    self.assertEqual(set(archive.namelist()), {'modules/' + name for name in expected} |
                                     {'manifest.json', 'original-extraction-manifest.json'})
                    for name, data in expected.items():
                        self.assertEqual(archive.read('modules/' + name), data)
                    self.assertEqual(archive.read('manifest.json'), saved_manifest)
                    self.assertEqual(archive.read('original-extraction-manifest.json'), original_manifest)
                    report = json.loads(archive.read('manifest.json').decode('utf-8-sig'))
                    self.assertEqual(report['counts'], {'cro': 2, 'crs': 1, 'crr': 1})
                    self.assertEqual(report['source_main_cxi']['sha256'], before)
                    for row in report['files']:
                        self.assertEqual(row['sha256'], hashlib.sha256(archive.read(row['path'])).hexdigest())

            assert_archive()

            # Reproduce the old failure boundary: extraction and manifest are
            # complete, but ZIP construction left an empty legacy partial file.
            upload_zip.unlink()
            legacy_partial = job / 'PokemonX-modules-upload.zip.partial'
            legacy_partial.write_bytes(b'')
            extracted_files = {
                path.relative_to(job): (hashlib.sha256(path.read_bytes()).hexdigest(), path.stat().st_mtime_ns)
                for path in (job / 'work').rglob('*') if path.is_file()
            }
            invocation_marker = base / 'extractor was executed'
            failing_tool = base / 'failing extractor'
            failing_tool.write_text('#!/bin/sh\ntouch ' + shlex.quote(str(invocation_marker)) + '\nexit 99\n')
            failing_tool.chmod(0o755)
            resume_command = [
                str(PWSH), '-NoLogo', '-NoProfile', '-NonInteractive', '-File', str(SCRIPT),
                '-ExtractionFolder', str(previous), '-OutputRoot', str(base / 'output with spaces'),
                '-ToolPath', str(failing_tool), '-ResumeLatest',
            ]
            result = subprocess.run(resume_command, capture_output=True, text=True, timeout=90, env=env)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertFalse(invocation_marker.exists(), 'Resume executed the extractor')
            assert_archive()
            self.assertEqual((job / 'manifest.json').read_bytes(), saved_manifest)
            self.assertEqual(legacy_partial.read_bytes(), b'')
            self.assertEqual(list(job.glob('*.partial')), [legacy_partial])
            self.assertEqual(
                {
                    path.relative_to(job): (hashlib.sha256(path.read_bytes()).hexdigest(), path.stat().st_mtime_ns)
                    for path in (job / 'work').rglob('*') if path.is_file()
                },
                extracted_files,
            )
            self.assertEqual(hashlib.sha256(main.read_bytes()).hexdigest(), before)
            self.assertEqual(main.stat().st_mtime_ns, main_mtime)
            self.assertEqual((previous / 'upload/manifest.json').read_bytes(), original_manifest)
            self.assertEqual(list((base / 'output with spaces').glob('PokemonX-modules-*')), [job])

            # An altered module of the same size must fail against saved hashes,
            # without replacing provenance or silently extracting it again.
            upload_zip.unlink()
            altered_module = job / 'work/romfs/DllTest.cro'
            altered_module.write_bytes(b'X' + expected['DllTest.cro'][1:])
            result = subprocess.run(resume_command, capture_output=True, text=True, timeout=90, env=env)
            self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn('Module differs from the saved manifest', result.stdout + result.stderr)
            self.assertFalse(upload_zip.exists())
            self.assertFalse(invocation_marker.exists(), 'Rejected resume executed the extractor')
            self.assertEqual((job / 'manifest.json').read_bytes(), saved_manifest)
            self.assertEqual(list(job.glob('*.partial')), [legacy_partial])
            self.assertEqual(hashlib.sha256(main.read_bytes()).hexdigest(), before)
            self.assertEqual(main.stat().st_mtime_ns, main_mtime)
            self.assertEqual((previous / 'upload/manifest.json').read_bytes(), original_manifest)


if __name__ == '__main__':
    unittest.main()
