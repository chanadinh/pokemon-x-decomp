#!/usr/bin/env python3
"""Export the bounded field-scene functions from the saved project, read-only."""
import argparse
import json
import os
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, default=Path(__file__).resolve().parents[3])
    parser.add_argument('--project', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--project-owner', default='agent')
    args = parser.parse_args()
    root = args.workspace.resolve()
    project = args.project.resolve()
    if not project.is_file() or project.suffix != '.gpr':
        parser.error('--project must name an existing .gpr file')
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=False)
    here = Path(__file__).resolve().parent
    (out / 'requests.txt').write_bytes((here / 'export-targets.txt').read_bytes())
    jdk = root / 'tools/jdk-21.0.13+9'
    state = root / 'tools/runtime-state'
    state.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env.update(JAVA_HOME=str(jdk),
               PATH=str(jdk / 'bin') + os.pathsep + env.get('PATH', ''),
               JAVA_TOOL_OPTIONS=env.get('JAVA_TOOL_OPTIONS', '')
               + ' -Duser.home=' + str(state) + ' -Duser.name=' + args.project_owner,
               XDG_CONFIG_HOME=str(state / 'config'), XDG_CACHE_HOME=str(state / 'cache'))
    cmd = [str(root / 'tools/ghidra_12.1.4_PUBLIC/support/analyzeHeadless'),
           str(project.parent), project.stem, '-process', 'main_text.bin',
           '-noanalysis', '-readOnly', '-scriptPath', str(here),
           '-postScript', 'ExportFieldEvidence.java', str(out), 'once']
    (out / 'command.json').write_text(json.dumps(cmd, indent=2) + '\n')
    with (out / 'headless.log').open('w') as stream:
        result = subprocess.run(cmd, env=env, stdout=stream, stderr=subprocess.STDOUT)
    log = (out / 'headless.log').read_text(errors='replace')
    if result.returncode or 'FIELD_EVIDENCE_EXPORT_COMPLETE count=24' not in log:
        raise SystemExit(log[-6000:])
    subprocess.run(['python3', str(here / 'verify_original_code.py'),
                    '--workspace', str(root), '--exports', str(out)], check=True)


if __name__ == '__main__':
    main()
