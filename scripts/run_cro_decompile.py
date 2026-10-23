#!/usr/bin/env python3
"""Import prepared CRO analysis blocks and export battle/field pseudocode."""
import argparse
import datetime
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--additional-seeds', type=Path)
    parser.add_argument('--per-module', type=int, default=100)
    args = parser.parse_args()
    if not 1 <= args.per_module <= 500:
        parser.error('--per-module must be 1..500')
    manifest = json.loads(args.manifest.read_text())
    args.output.mkdir(parents=True, exist_ok=False)
    anchors = []
    if args.additional_seeds:
        seeds = json.loads(args.additional_seeds.read_text())
        manifest['symbols'].extend(seeds)
        anchors.extend(s['address'] for s in seeds if s['kind'] == 'function')
    for symbol in manifest['symbols']:
        if symbol['module'] in ('DllBattle', 'DllField') and symbol['name'] == 'nnroControlObject_':
            anchors.append(symbol['address'])
    # Resolve prepared paths before copying the manifest into the run directory.
    for block in manifest['blocks']:
        if block.get('path'):
            block['path'] = str((args.manifest.resolve().parent / block['path']).resolve())
    imported = args.output.resolve() / 'import-manifest.json'
    imported.write_text(json.dumps(manifest, indent=2) + '\n')
    project = 'cro_' + datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
    jdk = (ROOT / 'tools/jdk-21.0.13+9').resolve()
    launcher = (ROOT / 'tools/ghidra_12.1.4_PUBLIC/support/analyzeHeadless').resolve()
    state = ROOT / 'tools/runtime-state'
    state.mkdir(exist_ok=True)
    env = os.environ.copy()
    env['JAVA_HOME'] = str(jdk)
    env['PATH'] = str(jdk / 'bin') + os.pathsep + env.get('PATH', '')
    env['JAVA_TOOL_OPTIONS'] = env.get('JAVA_TOOL_OPTIONS', '') + ' -Duser.home=' + str(state)
    env['XDG_CONFIG_HOME'] = str(state / 'config')
    env['XDG_CACHE_HOME'] = str(state / 'cache')
    raw = next(block['path'] for block in manifest['blocks'] if block.get('path'))
    command = [str(launcher), str(ROOT / 'projects'), project,
               '-import', raw, '-loader', 'BinaryLoader', '-processor', 'ARM:LE:32:v6', '-cspec', 'default',
               '-scriptPath', str(ROOT / 'ghidra_scripts'),
               '-preScript', 'ImportCROAnalysis.java', str(imported),
               '-postScript', 'ExportCROAnalysis.java', str(imported), str(args.output.resolve()), str(args.per_module)]
    if anchors:
        command += ['-postScript', 'ExportAnchorReferences.java', str(args.output.resolve() / 'anchors')]
        command += [format(address, 'x') for address in dict.fromkeys(anchors)]
    command += ['-analysisTimeoutPerFile', '900', '-max-cpu', '4']
    provenance = {'project': str(ROOT / 'projects' / (project + '.gpr')), 'command': command,
                  'base_prepared_manifest': str(args.manifest.resolve())}
    (args.output / 'run.json').write_text(json.dumps(provenance, indent=2) + '\n')
    log_path = args.output / 'headless.log'
    print('Running Ghidra. Log: ' + str(log_path), flush=True)
    with log_path.open('w') as stream:
        result = subprocess.run(command, env=env, stdout=stream, stderr=subprocess.STDOUT)
    log = log_path.read_text(errors='replace')
    summary_path = args.output / 'summary.json'
    if (result.returncode or not summary_path.exists() or 'CRO_ANALYSIS_IMPORT_COMPLETE' not in log
            or 'CRO_ANALYSIS_EXPORT_COMPLETE' not in log or 'REPORT: Save failed for:' in log
            or not Path(provenance['project']).exists()):
        raise SystemExit('Ghidra import/export failed. Inspect ' + str(log_path) + '\n' + log[-6000:])
    summary = json.loads(summary_path.read_text())
    expected = [{key: block[key] for key in ('address', 'size', 'read', 'write', 'execute')}
                | {'initialized': bool(block.get('path'))}
                for block in manifest['blocks'] if block['size']]
    actual = [{key: block[key] for key in ('address', 'size', 'read', 'write', 'execute', 'initialized')}
              for block in summary['memory_blocks']]
    if sorted(expected, key=lambda x: x['address']) != sorted(actual, key=lambda x: x['address']):
        raise SystemExit('Ghidra memory map does not match prepared blocks')
    summary['memory_map_verified'] = True
    summary['analysis_timeout_reported'] = 'analysis timed out' in log.lower()
    summary_path.write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps({k: v for k, v in summary.items() if k not in ('memory_blocks', 'by_module')}, indent=2))
    print(json.dumps({k: v for k, v in summary['by_module'].items() if k in ('DllBattle', 'DllField')}, indent=2))
    print('Saved project: ' + provenance['project'])


if __name__ == '__main__':
    main()
