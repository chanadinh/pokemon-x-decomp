#!/usr/bin/env python3
"""Reproduce the guarded, read-only constructor evidence export on Linux."""
import argparse
import json
import os
from pathlib import Path
import subprocess


ADDRESSES = [
    '0039a5a8', '0039daf4', '0039a8a4', '00140120', '0012b6d0',
    '0013a7a0', '0012f9a4', '0012b744', '0012b808', '0013a7bc',
    '0012f978', '0014ff4c', '0013a8d8', '0014f324', '0014f6b8',
    '00301a9c', '00167fb4', '0013a768', '0013a720',
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
               'RepairPokemonConstructorForExport.java', str(out), *ADDRESSES]
    (out / 'command.json').write_text(json.dumps(command, indent=2) + '\n')
    with (out / 'headless.log').open('w') as stream:
        result = subprocess.run(command, env=env, stdout=stream, stderr=subprocess.STDOUT)
    log = (out / 'headless.log').read_text(errors='replace')
    if (result.returncode or 'POKEMON_CONSTRUCTOR_TEMPORARY_REPAIR_COMPLETE size=236' not in log
            or 'EXACT_FUNCTION_EXPORT_COMPLETE count=19' not in log):
        raise SystemExit(log[-6000:])
    rows = json.loads((out / 'functions.json').read_text())
    if [row['address'] for row in rows] != ADDRESSES or not all(row['decompiled'] for row in rows):
        raise SystemExit('Export list or decompilation status differs from the expected baseline')
    if rows[2]['body_bytes'] != 236:
        raise SystemExit('Unexpected converter function extent')
    print(f'Exported {len(rows)} functions with temporary repair to {out}')


if __name__ == '__main__':
    main()
