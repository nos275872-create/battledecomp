#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/../lib/common.sh"

BIN_FILE="${ORIG_DIR}/bin/EBOOT.BIN"
RAW_DIR="${ROOT_DIR}/recon/raw"

echo "=== [Recon] Extrayendo cadenas de texto de ${BIN_FILE} ==="

if [ ! -f "${BIN_FILE}" ]; then
    echo "ERROR: ${BIN_FILE} no encontrado." >&2
    exit 1
fi

mkdir -p "${RAW_DIR}"

# 1. Cadenas ASCII de longitud >= 4
strings -a -t x -n 4 "${BIN_FILE}" > "${RAW_DIR}/strings_ascii.txt"
TOTAL_ASCII="$(wc -l < "${RAW_DIR}/strings_ascii.txt")"
echo "Cadenas ASCII extraídas: ${TOTAL_ASCII}"

# 2. Cadenas UTF-16LE de longitud >= 4
strings -a -t x -e l -n 4 "${BIN_FILE}" > "${RAW_DIR}/strings_utf16le.txt"
TOTAL_UTF16="$(wc -l < "${RAW_DIR}/strings_utf16le.txt")"
echo "Cadenas UTF-16LE extraídas: ${TOTAL_UTF16}"

# 3. Filtrar cadenas de depuración interesantes (rutas de código fuente, assertions, compilador)
grep -iE '\.(cpp|c|h|hpp)' "${RAW_DIR}/strings_ascii.txt" > "${RAW_DIR}/strings_source_files.txt" || true
grep -iE 'gcc|assert|build|rebellion|asura|lucas' "${RAW_DIR}/strings_ascii.txt" > "${RAW_DIR}/strings_signatures.txt" || true

echo "Cadenas con referencias a código fuente (.cpp/.c/.h): $(wc -l < "${RAW_DIR}/strings_source_files.txt")"
echo "Cadenas de firmas de motor/compilador: $(wc -l < "${RAW_DIR}/strings_signatures.txt")"
