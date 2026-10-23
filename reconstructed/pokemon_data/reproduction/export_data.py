#!/usr/bin/env python3
"""Reproduce the read-only Pokemon data evidence export on Linux."""
import argparse
import json
import os
from pathlib import Path
import subprocess


ADDRESSES = [
    '0014f6b8', '0014ff4c', '00168548', '00168360', '001686e8',
    '00168028', '001687f8', '00168780', '0014fd04', '00167ffc',
    '00168740', '00168054', '00168568', '0016830c', '001687cc',
    '001682a4', '00168594', '00168404', '00168090', '0016810c',
    '0016814c', '00168384', '001683c4', '001680cc', '001688c8',
    '00168278', '00168430', '00168468', '001684a0', '00167db0',
    '00167f98', '00168714', '00168660', '001685f0', '0014f220',
    '0016b358', '0016b3d4', '00176168', '001761e8', '00174e0c',
    '00174e6c', '00167fb4',
]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, default=Path(__file__).resolve().parents[3])
    parser.add_argument('--project', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--ghidra', type=Path)
    parser.add_argument('--jdk', type=Path)
    args = parser.parse_args()
    root = args.workspace.resolve()
    project = args.project.resolve()
    if not project.is_file() or project.suffix != '.gpr':
        parser.error('--project must name an existing .gpr file')
    ghidra = (args.ghidra or root / 'tools/ghidra_12.1.4_PUBLIC').resolve()
    jdk = (args.jdk or root / 'tools/jdk-21.0.13+9').resolve()
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=False)
    state = root / 'tools/runtime-state'
    state.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env.update(JAVA_HOME=str(jdk), PATH=str(jdk / 'bin') + os.pathsep + env.get('PATH', ''),
               JAVA_TOOL_OPTIONS=env.get('JAVA_TOOL_OPTIONS', '') + ' -Duser.home=' + str(state),
               XDG_CONFIG_HOME=str(state / 'config'), XDG_CACHE_HOME=str(state / 'cache'))
    command = [str(ghidra / 'support/analyzeHeadless'), str(project.parent), project.stem,
               '-process', 'main_text.bin', '-noanalysis', '-readOnly',
               '-scriptPath', str(Path(__file__).resolve().parent), '-postScript',
               'ExportFunctionSet.java', str(out), *ADDRESSES]
    (out / 'command.json').write_text(json.dumps(command, indent=2) + '\n')
    with (out / 'headless.log').open('w') as stream:
        result = subprocess.run(command, env=env, stdout=stream, stderr=subprocess.STDOUT)
    log = (out / 'headless.log').read_text(errors='replace')
    if result.returncode or 'EXACT_FUNCTION_EXPORT_COMPLETE count=42' not in log:
        raise SystemExit(log[-6000:])
    rows = json.loads((out / 'functions.json').read_text())
    if [row['address'] for row in rows] != ADDRESSES or not all(row['decompiled'] for row in rows):
        raise SystemExit('Export list or decompilation status differs from the expected baseline')
    print(f'Exported {len(rows)} functions to {out}')


if __name__ == '__main__':
    main()
