#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/common.sh"

TODAY="$(date +"%Y-%m-%d")"
TIME_HHMM="$(date +"%H:%M")"
JOURNAL_FILE="${DOCS_DIR}/journal/${TODAY}.md"

TITLE="${1:-"Actualización de sesión"}"
TEXT="${2:-""}"
COMMANDS="${3:-""}"
RESULTS="${4:-""}"
PROBLEMS="${5:-"Ninguno"}"
DECISIONS="${6:-"N/A"}"
NEXT_STEP="${7:-""}"

mkdir -p "${DOCS_DIR}/journal"

# If journal file for today doesn't exist, create it with header
if [ ! -f "${JOURNAL_FILE}" ]; then
    cat <<EOF > "${JOURNAL_FILE}"
# Bitácora de trabajo — ${TODAY}

EOF
fi

# Append entry
cat <<EOF >> "${JOURNAL_FILE}"
## ${TIME_HHMM} — ${TITLE}
- Qué hice: ${TEXT}
- Comandos / scripts: ${COMMANDS}
- Resultado y evidencia: ${RESULTS}
- Problemas: ${PROBLEMS}
- Decisiones (enlace al ADR si hay): ${DECISIONS}
- Siguiente paso: ${NEXT_STEP}

EOF

echo "Journal updated: ${JOURNAL_FILE}"
