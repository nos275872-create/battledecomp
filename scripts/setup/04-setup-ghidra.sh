#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/../lib/common.sh"

GHIDRA_PATH="${GHIDRA_INSTALL_DIR:-/opt/ghidra_12.1.4_PUBLIC}"
EXT_DIR="${TOOLS_DIR}/ghidra-ext"
ZIP_FILE="${EXT_DIR}/ghidra-allegrex-v21.4.zip"
DOWNLOAD_URL="https://github.com/kotcrab/ghidra-allegrex/releases/download/v21.4/ghidra_12.1.3_PUBLIC_20260825_ghidra-allegrex.zip"
TARGET_PROCESSOR_DIR="${GHIDRA_PATH}/Ghidra/Processors/Allegrex"

echo "=== [04] Configurando Ghidra Headless con módulo Allegrex ==="

if [ ! -d "${GHIDRA_PATH}" ]; then
    echo "ERROR: Directorio de Ghidra no encontrado en ${GHIDRA_PATH}" >&2
    exit 1
fi

mkdir -p "${EXT_DIR}"

if [ ! -f "${ZIP_FILE}" ]; then
    echo "Descargando extensión ghidra-allegrex v21.4..."
    curl -fL "${DOWNLOAD_URL}" -o "${ZIP_FILE}"
fi

SHA_EXT="$(sha256sum "${ZIP_FILE}" | cut -d' ' -f1)"
echo "Checksum ghidra-allegrex: ${SHA_EXT}"

if [ ! -d "${TARGET_PROCESSOR_DIR}" ]; then
    echo "Instalando módulo Allegrex en ${TARGET_PROCESSOR_DIR}..."
    TEMP_EXT="/tmp/allegrex_install_$$"
    mkdir -p "${TEMP_EXT}"
    7z x -y "${ZIP_FILE}" -o"${TEMP_EXT}" >/dev/null
    sudo -n cp -r "${TEMP_EXT}/ghidra-allegrex" "${TARGET_PROCESSOR_DIR}"
    rm -rf "${TEMP_EXT}"
fi

# Asegurar permisos de lectura en todo el módulo y limpiar duplicados
sudo -n chmod -R a+rX "${TARGET_PROCESSOR_DIR}"
sudo -n rm -rf "${GHIDRA_PATH}/Ghidra/Extensions/ghidra-allegrex"

# Asegurar permisos de ejecución en binarios nativos de Ghidra (decompiler, demangler)
sudo -n chmod +x "${GHIDRA_PATH}"/Ghidra/Features/Decompiler/os/linux_x86_64/* \
                 "${GHIDRA_PATH}"/GPL/DemanglerGnu/os/linux_x86_64/* 2>/dev/null || true

# Actualizar registros de versiones y descargas
grep -q "ghidra-allegrex" "${ENV_DIR}/downloads.md" || \
    printf "| ghidra-allegrex-v21.4.zip | %s | %s | Extensión de soporte para PSP Allegrex en Ghidra |\n" "${DOWNLOAD_URL}" "${SHA_EXT}" >> "${ENV_DIR}/downloads.md"

grep -q "GHIDRA_ALLEGREX" "${ENV_DIR}/versions.lock" || \
    printf "GHIDRA_ALLEGREX=v21.4 (Allegrex:LE:32:default)\n" >> "${ENV_DIR}/versions.lock"

echo "=== Ghidra con soporte Allegrex configurado correctamente ==="
grep "id=\"Allegrex" "${TARGET_PROCESSOR_DIR}/data/languages/allegrex.ldefs"
