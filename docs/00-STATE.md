# Estado actual — battledecomp

- **Última actualización:** 2026-10-02 12:00
- **Fase actual y progreso:**
  - **Fase 0 (Montaje del entorno e infraestructura):** 100% COMPLETADA.
  - **Fase 1 (Reconocimiento estático del binario):** 100% COMPLETADA.
  - **Fase 2 (Mapeo de arquitectura y subsistemas):** 92% EN PROGRESO.
- **Completado (con enlace a la evidencia):**
  - Entorno verificado y en estado saludable tras reinicio (`make doctor`).
  - Análisis completo de Ghidra Headless completado con éxito ([03-run-ghidra-recon.sh](file:///home/jcgar2/battledecomp/scripts/recon/03-run-ghidra-recon.sh)).
  - Catálogo de **12.280 funciones** exportadas en `ghidra/exports/functions.tsv`.
  - Decompilación de la cadena completa de carga de contenedores Asura (`0x0017603C`, `0x00023B7C`, `0x00023BEC`, `0x00024D70`, `0x00025158`, `0x00021ADC`, `0x00027E88`, `0x00171F04`, `0x00171DA8`).
  - Decompilación completa del sistema de **Blueprints y Atributos de Combate** (`0x0001A07C`, `0x0001A0C8`, `0x00018718`, `0x00017BC0`, `0x00017D90`, `0x00029D88`, `0x00029B50`):
    - Decodificación binaria de los chunks `'BLUE'` en `COMMON.ASR` con deserialización en 4 bytes de enteros, flotantes, booleanos y cadenas de caracteres.
    - Implementación del cargador en C++17 ([include/core/asura_blueprint.h](file:///home/jcgar2/battledecomp/include/core/asura_blueprint.h), [src/core/asura_blueprint.cpp](file:///home/jcgar2/battledecomp/src/core/asura_blueprint.cpp)).
    - Mapeo completo de las 21 armas con atributos numéricos reales de combate (cadencia, recarga, munición, calor/enfriamiento, proyectil, daño a infantería y vehículos) en [include/systems/weapons.h](file:///home/jcgar2/battledecomp/include/systems/weapons.h) y [src/systems/weapons.cpp](file:///home/jcgar2/battledecomp/src/systems/weapons.cpp), documentado en [docs/systems/weapons.md](file:///home/jcgar2/battledecomp/docs/systems/weapons.md) y [ADR-0005](file:///home/jcgar2/battledecomp/docs/decisions/ADR-0005-weapon-blueprint-attributes.md).
  - Decompilación y reconstrucción del subsistema de **IA de Bots y Soldados**:
    - Funciones Allegrex analizadas y decompiladas: `0x00188A44` (constructor), `0x00188C30` / `0x00188CC0` (destructores), `0x00188D58` (update loop), `0x00189284` (tracking de objetivo), `0x00188DB8` (filtro de amenazas vivas), `0x00189854` (eliminación y shift de amenazas), `0x0018959C` (coordenadas de target), `0x00188EF4` (switch de modos y telemetría 3D).
    - Mapeo de 9 modos de comportamiento tácticos y 11 sub-acciones de soldados/sables documentadas en [docs/systems/bot-ai.md](file:///home/jcgar2/battledecomp/docs/systems/bot-ai.md) y [ADR-0004](file:///home/jcgar2/battledecomp/docs/decisions/ADR-0004-bot-ai-architecture.md).
    - Implementación C++17 en [include/ai/bot_ai.h](file:///home/jcgar2/battledecomp/include/ai/bot_ai.h) y [src/ai/bot_ai.cpp](file:///home/jcgar2/battledecomp/src/ai/bot_ai.cpp).
  - Decompilación y reconstrucción del subsistema de **Personalización de Soldados y Clases (Customisation & Loadouts)**:
    - Funciones Allegrex analizadas y decompiladas: `0x00224878` (`Customisation_InitManager`), `0x0022379C` (`FE_Customisation_OnInitMenu`), `0x00223A44` (`FE_Customisation_Credits`), `0x00224148` (`FE_Customisation_ListCarouselNames`), `0x0022466C` (`FE_Customisation_ListCarouselCosts`), `0x002304EC` (`Customisation_GetItemCost`), `0x002305F8` (`Customisation_GetStatCost`).
    - Mapeo completo de la tabla de 8 categorías `0x002DBD0C` y sus 46 opciones de equipamiento militar y atributos físicos progresivos.
    - Implementación de la economía de créditos (100 cr estándar, deducción dinámica y validación de reglas de sobrepresupuesto) en [include/systems/customisation.h](file:///home/jcgar2/battledecomp/include/systems/customisation.h) y [src/systems/customisation.cpp](file:///home/jcgar2/battledecomp/src/systems/customisation.cpp), documentado en [docs/systems/customisation.md](file:///home/jcgar2/battledecomp/docs/systems/customisation.md) y [ADR-0006](file:///home/jcgar2/battledecomp/docs/decisions/ADR-0006-soldier-customisation-architecture.md).
  - Suite de pruebas completa integrada en el Makefile (`make test`), 100% pasando:
    - [tests/test_weapons_loader.cpp](file:///home/jcgar2/battledecomp/tests/test_weapons_loader.cpp): valida 21 armas en 5 idiomas a partir de `WEAPONNAMES.ASR`.
    - [tests/test_bot_ai.cpp](file:///home/jcgar2/battledecomp/tests/test_bot_ai.cpp): valida 100% de la lógica de estados, capacidad de 5 amenazas, eliminación por shift, filtrado y tracking.
    - [tests/test_weapon_attributes.cpp](file:///home/jcgar2/battledecomp/tests/test_weapon_attributes.cpp): valida la carga de los 16 blueprints de `COMMON.ASR` y los atributos de combate de las 21 armas.
    - [tests/test_customisation.cpp](file:///home/jcgar2/battledecomp/tests/test_customisation.cpp): valida las 8 categorías, resolución de costes de Blueprints y cálculo de loadouts de infantería.
- **Bloqueos / pendiente de mí:** Ninguno.
- **Próximo paso exacto:**
  1. Conectar la selección de armamento del controlador táctico de bots (`BotAIController`) con la lógica de equipamiento (`SoldierLoadout`) según la distancia y el tipo de amenaza (infantería vs vehículos).
  2. Iniciar el reconocimiento del subsistema de vehículos y torretas (Blueprints `Tank`, `Turret`, `Flyer`, `Walker`).
- **Archivos clave y su ruta:**
  - Binario descifrado: `orig/bin/EBOOT.BIN`
  - Informe de reconocimiento: [docs/knowledge/recon-report.md](file:///home/jcgar2/battledecomp/docs/knowledge/recon-report.md)
  - Desglose de librerías y NIDs: `recon/raw/prx_analysis.md`
  - Exportaciones de memoria: `ghidra/exports/memory_blocks.tsv`
  - Registro de versiones y checksums: [env/versions.lock](file:///home/jcgar2/battledecomp/env/versions.lock), [env/checksums.txt](file:///home/jcgar2/battledecomp/env/checksums.txt)
