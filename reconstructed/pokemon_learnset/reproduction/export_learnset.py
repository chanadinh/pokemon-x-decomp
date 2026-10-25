#!/usr/bin/env python3
"""Reproduce the read-only Pokemon learnset evidence export on Linux."""
import argparse
import json
import os
from pathlib import Path
import subprocess


ADDRESSES = ['0014f294', '0014f2a4', '0014f2b8', '0014f310', '0016b1e8', '0016b224', '0016b25c', '00167ca4', '00175fb4', '0015e958', '00138214', '00174c0c', '00175c78', '00300ef8']


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
               '-scriptPath', str(root / 'reconstructed/pokemon_stats/reproduction'), '-postScript',
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
