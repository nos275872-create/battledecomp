.PHONY: all doctor backup journal help

all: help

help:
	@echo "battledecomp — Objetivos disponibles:"
	@echo "  make doctor                 - Ejecutar diagnóstico completo del entorno"
	@echo "  make backup                 - Generar copia comprimida de seguridad"
	@echo "  make journal T=\"...\" B=\"...\" - Añadir nueva entrada a la bitácora"

doctor:
	@./scripts/doctor.sh

backup:
	@./scripts/backup.sh

journal:
	@./scripts/lib/journal.sh "$(T)" "$(B)"
