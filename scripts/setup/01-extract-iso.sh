#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/../lib/common.sh"

ARCHIVE_NAME="Star Wars - Battlefront - Renegade Squadron (Europe) (En,Fr,De,Es,It).7z"
ARCHIVE_PATH="${ROOT_DIR}/${ARCHIVE_NAME}"
ISO_TARGET_DIR="${ORIG_DIR}/iso"
EXTRACTED_DIR="${ORIG_DIR}/extracted"
BIN_DIR="${ORIG_DIR}/bin"

echo "=== [01] Extrayendo y verificando imagen ISO del juego ==="

if [ ! -f "${ARCHIVE_PATH}" ]; then
    echo "ERROR: No se encontró el archivo de origen: ${ARCHIVE_PATH}" >&2
    exit 1
fi

mkdir -p "${ISO_TARGET_DIR}" "${EXTRACTED_DIR}" "${BIN_DIR}"

# 1. Extraer ISO desde .7z si no existe aún en orig/iso/
ISO_FILE="$(find "${ISO_TARGET_DIR}" -maxdepth 1 -name "*.iso" | head -n 1 || true)"
if [ -z "${ISO_FILE}" ]; then
    echo "Extrayendo archivo .iso del contenedor .7z..."
    7z e "${ARCHIVE_PATH}" "*.iso" -o"${ISO_TARGET_DIR}" -y
    ISO_FILE="$(find "${ISO_TARGET_DIR}" -maxdepth 1 -name "*.iso" | head -n 1)"
fi

echo "ISO localizada en: ${ISO_FILE}"

# 2. Calcular SHA-256 si no está registrado
HASH_FILE="${ISO_TARGET_DIR}/sha256.txt"
if [ ! -f "${HASH_FILE}" ]; then
    echo "Calculando suma SHA-256 de la ISO..."
    sha256sum "${ISO_FILE}" > "${HASH_FILE}"
fi
echo "SHA-256 de la ISO:"
cat "${HASH_FILE}"

# 3. Extraer contenido del UMD (sistema de ficheros PSP_GAME) a orig/extracted/
if [ ! -d "${EXTRACTED_DIR}/PSP_GAME" ]; then
    echo "Extrayendo contenido del UMD a ${EXTRACTED_DIR}..."
    7z x "${ISO_FILE}" -o"${EXTRACTED_DIR}" -y
fi

# 4. Copiar EBOOT.BIN cifrado y posibles módulos PRX a orig/bin/
if [ -f "${EXTRACTED_DIR}/PSP_GAME/SYSDIR/EBOOT.BIN" ]; then
    cp -u "${EXTRACTED_DIR}/PSP_GAME/SYSDIR/EBOOT.BIN" "${BIN_DIR}/EBOOT.BIN.enc"
    echo "EBOOT.BIN (cifrado original) copiado a ${BIN_DIR}/EBOOT.BIN.enc"
fi

if [ -f "${EXTRACTED_DIR}/PSP_GAME/SYSDIR/BOOT.BIN" ]; then
    cp -u "${EXTRACTED_DIR}/PSP_GAME/SYSDIR/BOOT.BIN" "${BIN_DIR}/BOOT.BIN"
    echo "BOOT.BIN copiado a ${BIN_DIR}/BOOT.BIN"
fi

# Copiar cualquier .prx encontrado en SYSDIR o USRDIR
find "${EXTRACTED_DIR}/PSP_GAME" -name "*.prx" -exec cp -u {} "${BIN_DIR}/" \; 2>/dev/null || true

echo "=== Extracción completada con éxito ==="
ls -lh "${BIN_DIR}"
