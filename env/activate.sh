#!/usr/bin/env bash
# Source this file to load project environment variables:
#   source env/activate.sh

export BATTLEDECOMP_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export GHIDRA_INSTALL_DIR="/opt/ghidra_12.1.4_PUBLIC"
export PATH="${BATTLEDECOMP_ROOT}/scripts:${BATTLEDECOMP_ROOT}/tools/bin:${PATH}"

echo "battledecomp environment activated."
echo "Root: ${BATTLEDECOMP_ROOT}"
echo "Ghidra: ${GHIDRA_INSTALL_DIR}"
