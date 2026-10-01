#!/usr/bin/env bash
set -eo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/common.sh"

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <tag> [--] <command...>" >&2
    exit 1
fi

TAG="$1"
shift

if [ "$1" = "--" ]; then
    shift
fi

COMMAND_STR="$*"
TIMESTAMP="$(date +"%Y%m%d-%H%M%S")"
DATE_ISO="$(date -Iseconds)"
LOG_FILE="${LOGS_DIR}/commands/${TIMESTAMP}-${TAG}.log"
INDEX_FILE="${LOGS_DIR}/commands/index.tsv"

# Create header in index.tsv if not present
if [ ! -f "${INDEX_FILE}" ]; then
    printf "timestamp\ttag\tcommand\texit_code\n" > "${INDEX_FILE}"
fi

# Execute command capturing all output and preserving exit code
set +e
"$@" 2>&1 | tee "${LOG_FILE}"
EXIT_CODE="${PIPESTATUS[0]}"
set -e

# Record entry in index.tsv (sanitize tabs/newlines in command string)
CLEAN_CMD="$(echo "${COMMAND_STR}" | tr '\t\n' '  ')"
printf "%s\t%s\t%s\t%d\n" "${DATE_ISO}" "${TAG}" "${CLEAN_CMD}" "${EXIT_CODE}" >> "${INDEX_FILE}"

exit "${EXIT_CODE}"
