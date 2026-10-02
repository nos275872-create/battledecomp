#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/../lib/common.sh"

PROJECT_DIR="${ROOT_DIR}/ghidra/projects"
PROJECT_NAME="battledecomp_ghidra"
SCRIPT_PATH="${ROOT_DIR}/ghidra/scripts"
TARGET_BIN="${ORIG_DIR}/bin/EBOOT.BIN"
LOG_FILE="${LOGS_DIR}/ghidra/headless-analysis.log"
HEADLESS="/opt/ghidra_12.1.4_PUBLIC/support/analyzeHeadless"

echo "=== [Recon] Ejecutando análisis Headless con Ghidra (Allegrex) ==="

if [ ! -f "${TARGET_BIN}" ]; then
    echo "ERROR: ${TARGET_BIN} no existe." >&2
    exit 1
fi

mkdir -p "${PROJECT_DIR}" "${ROOT_DIR}/ghidra/exports" "${LOGS_DIR}/ghidra"

echo "Iniciando análisis headless en segundo plano..."
echo "Proyecto: ${PROJECT_DIR}/${PROJECT_NAME}"
echo "Binario:  ${TARGET_BIN}"
echo "Log:      ${LOG_FILE}"

export GHIDRA_MAXMEM="2048m"

# Ejecutar Ghidra headless con procesador Allegrex y script de exportación
if [ -f "${PROJECT_DIR}/${PROJECT_NAME}.gpr" ]; then
    echo "Proyecto existente detectado. Analizando archivo importado..."
    "${HEADLESS}" "${PROJECT_DIR}" "${PROJECT_NAME}" \
        -process "EBOOT.BIN" \
        -scriptPath "${SCRIPT_PATH}" \
        -postScript "ExportSummary.java" \
        -max-cpu 4 \
        2>&1 | tee "${LOG_FILE}"
else
    "${HEADLESS}" "${PROJECT_DIR}" "${PROJECT_NAME}" \
        -import "${TARGET_BIN}" \
        -processor "Allegrex:LE:32:default" \
        -scriptPath "${SCRIPT_PATH}" \
        -postScript "ExportSummary.java" \
        -overwrite \
        -max-cpu 4 \
        2>&1 | tee "${LOG_FILE}"
fi

echo "=== Análisis Ghidra finalizado ==="
ls -lh "${ROOT_DIR}/ghidra/exports/"
