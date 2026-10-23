#!/usr/bin/env python3
"""Validate an extracted 3DS code pair, then import/analyze/export with Ghidra."""
import argparse
import datetime
import json
import os
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--code", type=Path, required=True)
    parser.add_argument("--exheader", type=Path, required=True)
    parser.add_argument("--decompressed", action="store_true",
                        help="Confirm code was decompressed, even if the original header has the compression bit set")
    parser.add_argument("--max-functions", type=int, default=100,
                        help="Pseudocode batch size; 0 exports all discovered functions (default: 100)")
    parser.add_argument("--ghidra", type=Path, default=ROOT / "tools/ghidra_12.1.4_PUBLIC")
    parser.add_argument("--jdk", type=Path, default=ROOT / "tools/jdk-21.0.13+9")
    args = parser.parse_args()
    if args.max_functions < 0:
        parser.error("--max-functions must be nonnegative")
    launcher = args.ghidra.resolve() / "support/analyzeHeadless"
    if not launcher.is_file() or not (args.jdk / "bin/javac").is_file():
        parser.error("Ghidra and a full JDK are required; provide --ghidra and --jdk paths")

    stamp = datetime.datetime.now(datetime.timezone.utc).strftime("%Y%m%dT%H%M%S%fZ")
    output = ROOT / "outputs" / stamp
    output.mkdir(parents=True, exist_ok=False)
    manifest_path = output / "manifest.json"
    command = [sys.executable, str(ROOT / "scripts/inspect_code.py"),
               "--code", str(args.code.resolve()), "--exheader", str(args.exheader.resolve()),
               "--output", str(manifest_path)]
    if args.decompressed:
        command.append("--decompressed")
    subprocess.run(command, check=True)
    manifest = json.loads(manifest_path.read_text())
    project_name = "code_" + stamp
    state = ROOT / "tools/runtime-state"
    state.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env["JAVA_HOME"] = str(args.jdk.resolve())
    env["PATH"] = str(args.jdk.resolve() / "bin") + os.pathsep + env.get("PATH", "")
    # Keep this tool's caches/configuration within the workspace.
    env["JAVA_TOOL_OPTIONS"] = env.get("JAVA_TOOL_OPTIONS", "") + ' -Duser.home="' + str(state) + '"'
    env["XDG_CONFIG_HOME"] = str(state / "config")
    env["XDG_CACHE_HOME"] = str(state / "cache")
    command = [str(launcher), str(ROOT / "projects"), project_name,
               "-import", str(args.code.resolve()), "-loader", "BinaryLoader",
               "-processor", "ARM:LE:32:v6", "-cspec", "default",
               "-scriptPath", str(ROOT / "ghidra_scripts"),
               "-preScript", "Import3DSCode.java", str(manifest_path),
               "-postScript", "ExportDecomp.java", str(output), str(args.max_functions),
               "-analysisTimeoutPerFile", "1800", "-max-cpu", "2"]
    log_path = output / "headless.log"
    print(f"Analyzing with Ghidra. Log: {log_path}", flush=True)
    with log_path.open("w") as log:
        result = subprocess.run(command, env=env, stdout=log, stderr=subprocess.STDOUT)
    summary_path = output / "summary.json"
    log_text = log_path.read_text(errors="replace")
    if (result.returncode or not summary_path.is_file() or "3DS_IMPORT_COMPLETE" not in log_text
            or "REPORT: Save failed for:" in log_text
            or not (ROOT / "projects" / (project_name + ".gpr")).is_file()):
        print(log_text[-10000:], file=sys.stderr)
        raise SystemExit(f"Analysis/export failed; inspect {log_path}")
    summary = json.loads(summary_path.read_text())
    if summary.get("code_sha256") != manifest["code"]["sha256"]:
        raise SystemExit(f"Export provenance mismatch; inspect {log_path}")
    expected = [{key: segment[key] for key in ("name", "address", "size", "read", "write", "execute")}
                | {"initialized": segment["offset"] is not None}
                for segment in manifest["segments"] if segment["size"]]
    if sorted(summary["memory_blocks"], key=lambda row: row["address"]) != sorted(expected, key=lambda row: row["address"]):
        raise SystemExit(f"Ghidra memory map differs from validated ExHeader map; inspect {log_path}")
    summary["analysis_timeout_reported"] = "REPORT: Analysis timed out" in log_text or "analysis timed out" in log_text.lower()
    summary_path.write_text(json.dumps(summary, indent=2) + "\n")
    print(json.dumps(summary, indent=2))
    print(f"Output: {output}")
    print(f"Saved Ghidra project: {ROOT / 'projects' / project_name}.gpr")


if __name__ == "__main__":
    main()
