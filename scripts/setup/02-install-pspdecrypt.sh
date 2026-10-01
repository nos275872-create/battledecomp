#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/../lib/common.sh"

TOOLS_SRC="${TOOLS_DIR}/src"
TOOLS_BIN="${TOOLS_DIR}/bin"
TARGET_BIN="${TOOLS_BIN}/pspdecrypt"
REPO_URL="https://github.com/John-K/pspdecrypt.git"

echo "=== [02] Instalando pspdecrypt ==="

mkdir -p "${TOOLS_SRC}" "${TOOLS_BIN}"

if [ -x "${TARGET_BIN}" ]; then
    echo "pspdecrypt ya está instalado en: ${TARGET_BIN}"
    exit 0
fi

SRC_DIR="${TOOLS_SRC}/pspdecrypt"
if [ ! -d "${SRC_DIR}" ]; then
    echo "Clonando repositorio ${REPO_URL}..."
    git clone --depth 1 "${REPO_URL}" "${SRC_DIR}"
fi

echo "Compilando pspdecrypt con GCC / G++..."
cd "${SRC_DIR}"
make CC=gcc CXX=g++ -j"$(nproc)"

cp -f pspdecrypt "${TARGET_BIN}"
chmod +x "${TARGET_BIN}"

COMMIT_HASH="$(git rev-parse HEAD)"
echo "pspdecrypt compilado con éxito (commit: ${COMMIT_HASH})"

# Registrar en env/downloads.md y env/versions.lock
grep -q "pspdecrypt" "${ENV_DIR}/downloads.md" || \
    printf "| pspdecrypt | %s (commit %s) | N/A (git) | Herramienta de descifrado de binarios PSP |\n" "${REPO_URL}" "${COMMIT_HASH}" >> "${ENV_DIR}/downloads.md"

grep -q "PSPDECRYPT" "${ENV_DIR}/versions.lock" || \
    printf "PSPDECRYPT=git commit %s\n" "${COMMIT_HASH}" >> "${ENV_DIR}/versions.lock"

echo "=== pspdecrypt instalado correctamente en ${TARGET_BIN} ==="
"${TARGET_BIN}" || true
