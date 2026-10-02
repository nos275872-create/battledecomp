.PHONY: all doctor backup journal help build test clean

CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -Iinclude

SRCS_CORE = src/core/asura_archive.cpp src/core/resource_mgr.cpp
SRCS_SYS = src/systems/weapons.cpp
OBJS = $(SRCS_CORE:.cpp=.o) $(SRCS_SYS:.cpp=.o)

all: help

help:
	@echo "battledecomp — Objetivos disponibles:"
	@echo "  make doctor                 - Ejecutar diagnóstico completo del entorno"
	@echo "  make backup                 - Generar copia comprimida de seguridad"
	@echo "  make journal T=\"...\" B=\"...\" - Añadir nueva entrada a la bitácora"
	@echo "  make build                  - Compilar subsistemas C++ y herramientas de test"
	@echo "  make test                   - Ejecutar suite de pruebas y validación"
	@echo "  make clean                  - Limpiar artefactos de compilación"

doctor:
	@./scripts/doctor.sh

backup:
	@./scripts/backup.sh

journal:
	@./scripts/lib/journal.sh "$(T)" "$(B)"

build/bin:
	mkdir -p build/bin

build: build/bin tests/test_weapons_loader.cpp $(SRCS_CORE) $(SRCS_SYS)
	$(CXX) $(CXXFLAGS) $(SRCS_CORE) $(SRCS_SYS) tests/test_weapons_loader.cpp -o build/bin/test_weapons_loader

test: build
	./build/bin/test_weapons_loader

clean:
	rm -rf build/bin

