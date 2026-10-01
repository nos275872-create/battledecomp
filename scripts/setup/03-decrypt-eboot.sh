#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/../lib/common.sh"

BIN_DIR="${ORIG_DIR}/bin"
ENCRYPTED_EBOOT="${BIN_DIR}/EBOOT.BIN.enc"
DECRYPTED_EBOOT="${BIN_DIR}/EBOOT.BIN.dec"
CANONICAL_EBOOT="${BIN_DIR}/EBOOT.BIN"
BOOT_BIN="${BIN_DIR}/BOOT.BIN"
PSPDECRYPT="${TOOLS_DIR}/bin/pspdecrypt"

echo "=== [03] Descifrando ejecutable EBOOT.BIN de PSP ==="

if [ ! -f "${ENCRYPTED_EBOOT}" ]; then
    echo "ERROR: No existe el binario cifrado en ${ENCRYPTED_EBOOT}." >&2
    echo "Ejecute primero ./scripts/setup/01-extract-iso.sh" >&2
    exit 1
fi

if [ ! -x "${PSPDECRYPT}" ]; then
    echo "Compilando/instalando pspdecrypt..."
    "${SCRIPT_DIR}/02-install-pspdecrypt.sh"
fi

if [ ! -f "${DECRYPTED_EBOOT}" ]; then
    echo "Descifrando con pspdecrypt..."
    "${PSPDECRYPT}" "${ENCRYPTED_EBOOT}" -o "${DECRYPTED_EBOOT}"
fi

# Verificar con BOOT.BIN si existe en el disco
if [ -f "${BOOT_BIN}" ]; then
    BOOT_HASH="$(sha256sum "${BOOT_BIN}" | cut -d' ' -f1)"
    DEC_HASH="$(sha256sum "${DECRYPTED_EBOOT}" | cut -d' ' -f1)"
    echo "Hash BOOT.BIN:        ${BOOT_HASH}"
    echo "Hash EBOOT descifrado: ${DEC_HASH}"
    if [ "${BOOT_HASH}" = "${DEC_HASH}" ]; then
        echo "VERIFICACIÓN EXITOSA: EBOOT descifrado coincide 100% con BOOT.BIN del UMD."
    else
        echo "AVISO: BOOT.BIN y EBOOT descifrado difieren."
    fi
fi

# Crear copia canónica en orig/bin/EBOOT.BIN
cp -f "${DECRYPTED_EBOOT}" "${CANONICAL_EBOOT}"

echo "Registro en env/checksums.txt..."
sed -i '/EBOOT\.BIN/d' "${ENV_DIR}/checksums.txt" 2>/dev/null || true
sha256sum "${CANONICAL_EBOOT}" | sed "s|${ROOT_DIR}/||" >> "${ENV_DIR}/checksums.txt"

echo "Ejecutable listo en ${CANONICAL_EBOOT}:"
file "${CANONICAL_EBOOT}"
ls -lh "${CANONICAL_EBOOT}"
