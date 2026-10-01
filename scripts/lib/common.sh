#!/usr/bin/env bash
set -euo pipefail

# Common environment definitions for battledecomp
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
export LOGS_DIR="${ROOT_DIR}/logs"
export DOCS_DIR="${ROOT_DIR}/docs"
export ENV_DIR="${ROOT_DIR}/env"
export TOOLS_DIR="${ROOT_DIR}/tools"
export ORIG_DIR="${ROOT_DIR}/orig"
export GHIDRA_DIR="${ROOT_DIR}/ghidra"

# Ensure essential logging directories exist
mkdir -p "${LOGS_DIR}/commands" "${LOGS_DIR}/setup" "${LOGS_DIR}/ghidra" "${ROOT_DIR}/backups"
