#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/lib/common.sh"

TIMESTAMP="$(date +"%Y%m%d-%H%M%S")"
BACKUP_FILE="${ROOT_DIR}/backups/battledecomp-${TIMESTAMP}.tar.gz"

echo "Creando backup de seguridad en: ${BACKUP_FILE}..."

# Create archive of key components
tar -czf "${BACKUP_FILE}" \
    -C "${ROOT_DIR}" \
    docs/ \
    env/ \
    scripts/ \
    ghidra/scripts/ \
    ghidra/projects/ \
    2>/dev/null || true

echo "Backup completado con éxito: $(du -h "${BACKUP_FILE}" | cut -f1)"

# Maintain only the 5 most recent backups
echo "Rotando backups (conservando los 5 más recientes)..."
cd "${ROOT_DIR}/backups"
ls -1t battledecomp-*.tar.gz 2>/dev/null | tail -n +6 | xargs -r rm -f

echo "Backups actuales:"
ls -lh battledecomp-*.tar.gz
