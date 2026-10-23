#!/usr/bin/env python3
"""Export exact functions from an existing Ghidra project without saving changes."""
import argparse
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--project', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('addresses', nargs='+', type=lambda s: int(s, 16))
    args = parser.parse_args()
    project = args.project.resolve()
    if not project.is_file() or project.suffix != '.gpr':
        parser.error('--project must name an existing .gpr file')
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=False)
    jdk = (ROOT / 'tools/jdk-21.0.13+9').resolve()
    state = ROOT / 'tools/runtime-state'
    env = os.environ.copy()
    env.update(JAVA_HOME=str(jdk), PATH=str(jdk / 'bin') + os.pathsep + env.get('PATH', ''),
               JAVA_TOOL_OPTIONS=env.get('JAVA_TOOL_OPTIONS', '') + ' -Duser.home=' + str(state),
               XDG_CONFIG_HOME=str(state / 'config'), XDG_CACHE_HOME=str(state / 'cache'))
    cmd = [str((ROOT / 'tools/ghidra_12.1.4_PUBLIC/support/analyzeHeadless').resolve()),
           str(project.parent), project.stem, '-process', 'main_text.bin', '-noanalysis',
           '-readOnly', '-scriptPath', str(ROOT / 'ghidra_scripts'), '-postScript',
           'ExportFunctionSet.java', str(out)] + [format(a, 'x') for a in args.addresses]
    (out / 'command.json').write_text(json.dumps(cmd, indent=2) + '\n')
    with (out / 'headless.log').open('w') as stream:
        result = subprocess.run(cmd, env=env, stdout=stream, stderr=subprocess.STDOUT)
    log = (out / 'headless.log').read_text(errors='replace')
    if result.returncode or 'EXACT_FUNCTION_EXPORT_COMPLETE' not in log:
        raise SystemExit(log[-6000:])
    rows = json.loads((out / 'functions.json').read_text())
    if [int(row['address'], 16) for row in rows] != args.addresses:
        raise SystemExit('Exported function list differs from request')
    if not all(row['decompiled'] for row in rows):
        raise SystemExit('Some functions failed to decompile; inspect functions.json')
    print(f'Exported {len(rows)} functions to {out}')


if __name__ == '__main__':
    main()
