#!/usr/bin/env python3
"""Reproduce the read-only Pokemon stats evidence export on Linux."""
import argparse
import json
import os
from pathlib import Path
import subprocess


ADDRESSES = [
    '0013aa84', '0014fb00', '0014fbd4', '0014fda4', '0014fe78',
    '0014fa2c', '0014f220', '0014f930', '0014fd54', '0014ff74',
    '0014ffa4', '0014ffd4', '00150004', '00150034', '00152c3c',
    '00152c68', '00152c94', '00152cc0', '00152cec', '00152d20',
    '00152d54', '00152d88', '00152db4', '00152de0', '00152e14',
    '00152ea8', '00152f54', '00152fa8', '00152fe0', '0015306c',
    '00167db0', '00167e18', '00167e2c', '00167f98', '001685f0',
    '00168660', '00168ae4', '00168b3c', '00168b84', '0016b2ec',
    '0016b318', '0016b348', '0016b358', '0016b3d4', '0016b45c',
]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, default=Path(__file__).resolve().parents[3])
    parser.add_argument('--project', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--ghidra', type=Path)
    parser.add_argument('--jdk', type=Path)
    parser.add_argument('--project-owner', help='Recorded owner of a restored local Ghidra project')
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
    if args.project_owner:
        env['JAVA_TOOL_OPTIONS'] += ' -Duser.name=' + args.project_owner
    command = [str(ghidra / 'support/analyzeHeadless'), str(project.parent), project.stem,
               '-process', 'main_text.bin', '-noanalysis', '-readOnly',
               '-scriptPath', str(Path(__file__).resolve().parent), '-postScript',
               'ExportFunctionSet.java', str(out), *ADDRESSES]
    (out / 'command.json').write_text(json.dumps(command, indent=2) + '\n')
    with (out / 'headless.log').open('w') as stream:
        result = subprocess.run(command, env=env, stdout=stream, stderr=subprocess.STDOUT)
    log = (out / 'headless.log').read_text(errors='replace')
    if result.returncode or f'EXACT_FUNCTION_EXPORT_COMPLETE count={len(ADDRESSES)}' not in log:
        raise SystemExit(log[-6000:])
    rows = json.loads((out / 'functions.json').read_text())
    if [row['address'] for row in rows] != ADDRESSES or not all(row['decompiled'] for row in rows):
        raise SystemExit('Export list or decompilation status differs from the expected baseline')
    print(f'Exported {len(rows)} functions to {out}')


if __name__ == '__main__':
    main()
