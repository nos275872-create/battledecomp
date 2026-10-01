#!/usr/bin/env bash
set -eo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/lib/common.sh"

echo "========================================================"
echo "          battledecomp — Diagnóstico del Sistema        "
echo "========================================================"
echo ""

TOTAL_FAIL=0

check_status() {
    local label="$1"
    local status="$2" # 0 for OK, non-zero for FAIL
    local details="$3"

    if [ "$status" -eq 0 ]; then
        printf "%-40s [  \033[32mOK\033[0m  ] %s\n" "$label" "$details"
    else
        printf "%-40s [ \033[31mFALLO\033[0m ] %s\n" "$label" "$details"
        TOTAL_FAIL=$((TOTAL_FAIL + 1))
    fi
}

echo "--- 1. Carpetas Base ---"
for dir in docs docs/journal docs/decisions docs/knowledge docs/systems docs/howto \
           orig/iso orig/extracted orig/bin \
           ghidra/projects ghidra/scripts ghidra/exports \
           recon/raw scripts/lib scripts/setup scripts/recon \
           tools env logs/commands logs/setup logs/ghidra backups \
           src include config asm build; do
    if [ -d "${ROOT_DIR}/${dir}" ]; then
        check_status "Directorio: ${dir}" 0 "Existe"
    else
        check_status "Directorio: ${dir}" 1 "No existe"
    fi
done

echo ""
echo "--- 2. Archivos Base ---"
for file in AGENTS.md GEMINI.md README.md Makefile .gitignore \
            docs/00-STATE.md docs/ROADMAP.md docs/GLOSSARY.md \
            scripts/lib/common.sh scripts/lib/run.sh scripts/lib/journal.sh \
            env/activate.sh env/versions.lock env/downloads.md env/checksums.txt; do
    if [ -f "${ROOT_DIR}/${file}" ]; then
        check_status "Archivo: ${file}" 0 "Presente"
    else
        check_status "Archivo: ${file}" 1 "Ausente"
    fi
done

echo ""
set +o pipefail
echo "--- 3. Herramientas del Sistema ---"
for tool in git make python3 java 7z gcc; do
    if command -v "$tool" >/dev/null 2>&1; then
        tool_ver="$("$tool" --version 2>&1 | head -n 1 || true)"
        check_status "Comando: ${tool}" 0 "${tool_ver}"
    else
        check_status "Comando: ${tool}" 1 "No instalado en el PATH"
    fi
done

# Ghidra check
if [ -x "/opt/ghidra_12.1.4_PUBLIC/support/analyzeHeadless" ]; then
    check_status "Ghidra Headless (/opt/...)" 0 "Disponible (12.1.4)"
elif command -v ghidra >/dev/null 2>&1; then
    check_status "Ghidra en PATH" 0 "Disponible"
else
    check_status "Ghidra" 1 "No encontrado"
fi

if [ -d "/opt/ghidra_12.1.4_PUBLIC/Ghidra/Processors/Allegrex" ]; then
    check_status "Ghidra Módulo Allegrex" 0 "Instalado (Allegrex:LE:32:default)"
else
    check_status "Ghidra Módulo Allegrex" 1 "No instalado"
fi

if [ -x "${TOOLS_DIR}/bin/pspdecrypt" ]; then
    check_status "Herramienta pspdecrypt" 0 "Compilado y disponible"
else
    check_status "Herramienta pspdecrypt" 1 "Falta compilar o instalar"
fi

if [ -f "${ORIG_DIR}/bin/EBOOT.BIN" ]; then
    check_status "Ejecutable EBOOT.BIN descifrado" 0 "Presente en orig/bin/EBOOT.BIN"
else
    check_status "Ejecutable EBOOT.BIN descifrado" 1 "Ausente"
fi

echo ""
echo "--- 4. Estado de Git ---"
if [ -d "${ROOT_DIR}/.git" ]; then
    BRANCH="$(git -C "${ROOT_DIR}" branch --show-current 2>/dev/null || echo "sin rama")"
    STATUS_LINES="$(git -C "${ROOT_DIR}" status --porcelain 2>/dev/null | wc -l)"
    check_status "Repositorio Git" 0 "Rama: '${BRANCH}', cambios pendientes: ${STATUS_LINES}"
else
    check_status "Repositorio Git" 1 "No inicializado (falta git init)"
fi

echo ""
echo "--- 5. Sumas de Verificación (env/checksums.txt) ---"
if [ -f "${ENV_DIR}/checksums.txt" ]; then
    # Only verify lines that look like sha256 hashes
    CHECKSUM_ENTRIES="$(grep -E '^[a-fA-F0-9]{64}' "${ENV_DIR}/checksums.txt" || true)"
    if [ -z "${CHECKSUM_ENTRIES}" ]; then
        check_status "Checksums" 0 "Sin entradas registradas aún"
    else
        set +e
        (cd "${ROOT_DIR}" && sha256sum -c --status "${ENV_DIR}/checksums.txt" 2>/dev/null)
        CHK_STATUS=$?
        set -e
        if [ "$CHK_STATUS" -eq 0 ]; then
            check_status "Checksums" 0 "Todos coinciden"
        else
            check_status "Checksums" 1 "Discrepancia en sumas de verificación"
        fi
    fi
else
    check_status "Checksums" 1 "Falta env/checksums.txt"
fi

echo ""
echo "========================================================"
if [ "$TOTAL_FAIL" -eq 0 ]; then
    echo -e "\033[32mResultado: Todos los chequeos obligatorios han pasado con éxito.\033[0m"
    exit 0
else
    echo -e "\033[31mResultado: Se detectaron ${TOTAL_FAIL} fallo(s).\033[0m"
    exit 1
fi
