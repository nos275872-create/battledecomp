.PHONY: all doctor backup journal help build test clean

CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -Iinclude

SRCS_CORE = src/core/asura_archive.cpp src/core/resource_mgr.cpp src/core/asura_blueprint.cpp
SRCS_SYS = src/systems/weapons.cpp src/systems/customisation.cpp src/systems/vehicles.cpp src/systems/boarding.cpp
SRCS_AI = src/ai/bot_ai.cpp

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

build: build/bin tests/test_weapons_loader.cpp tests/test_bot_ai.cpp tests/test_weapon_attributes.cpp tests/test_customisation.cpp tests/test_vehicles.cpp tests/test_boarding.cpp $(SRCS_CORE) $(SRCS_SYS) $(SRCS_AI)
	$(CXX) $(CXXFLAGS) $(SRCS_CORE) $(SRCS_SYS) tests/test_weapons_loader.cpp -o build/bin/test_weapons_loader
	$(CXX) $(CXXFLAGS) $(SRCS_AI) tests/test_bot_ai.cpp -o build/bin/test_bot_ai
	$(CXX) $(CXXFLAGS) $(SRCS_CORE) $(SRCS_SYS) tests/test_weapon_attributes.cpp -o build/bin/test_weapon_attributes
	$(CXX) $(CXXFLAGS) $(SRCS_CORE) $(SRCS_SYS) tests/test_customisation.cpp -o build/bin/test_customisation
	$(CXX) $(CXXFLAGS) $(SRCS_CORE) $(SRCS_SYS) tests/test_vehicles.cpp -o build/bin/test_vehicles
	$(CXX) $(CXXFLAGS) $(SRCS_CORE) $(SRCS_SYS) tests/test_boarding.cpp -o build/bin/test_boarding

test: build
	./build/bin/test_weapons_loader
	./build/bin/test_bot_ai
	./build/bin/test_weapon_attributes
	./build/bin/test_customisation
	./build/bin/test_vehicles
	./build/bin/test_boarding

clean:
	rm -rf build/bin
