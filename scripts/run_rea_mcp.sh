#!/usr/bin/env bash
# Local REA MCP launcher. It does not change agent configuration.
set -euo pipefail
rea_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
rea_entry="$rea_root/tools/rea/node_modules/rea-agents/scripts/rea.mjs"
rea_node=${REA_NODE:-}
if [[ -z "$rea_node" ]]; then
    if command -v node >/dev/null 2>&1; then
        rea_node=$(command -v node)
    elif [[ -x "$rea_root/tools/rea-runtime/bin/node" ]]; then
        rea_node="$rea_root/tools/rea-runtime/bin/node"
    else
        echo 'REA requires a supported local Node.js installation; set REA_NODE.' >&2
        exit 2
    fi
fi
if [[ ! -f "$rea_entry" ]]; then
    echo 'Install the pinned package with: npm install --prefix tools/rea --save-exact rea-agents@6.8.0' >&2
    exit 2
fi
export GHIDRA_INSTALL_DIR=${GHIDRA_INSTALL_DIR:-"$rea_root/tools/ghidra_12.1.4_PUBLIC"}
export JAVA_HOME=${JAVA_HOME:-"$rea_root/tools/jdk-21.0.13+9"}
export REA_ANALYSIS_PROVIDER=${REA_ANALYSIS_PROVIDER:-ghidra}
export REA_GHIDRA_LANGUAGE_ID=${REA_GHIDRA_LANGUAGE_ID:-ARM:LE:32:v6}
export REA_GHIDRA_COMPILER_SPEC_ID=${REA_GHIDRA_COMPILER_SPEC_ID:-default}
export REA_GHIDRA_STARTUP_TIMEOUT_MS=${REA_GHIDRA_STARTUP_TIMEOUT_MS:-900000}
export PATH="$JAVA_HOME/bin:$(dirname -- "$rea_node"):$PATH"
exec "$rea_node" "$rea_entry" mcp
