# Estado actual — battledecomp

- **Última actualización:** 2026-10-02 10:31
- **Fase actual y progreso:**
  - **Fase 0 (Montaje del entorno e infraestructura):** 100% COMPLETADA.
  - **Fase 1 (Reconocimiento estático del binario):** 100% COMPLETADA.
  - **Fase 2 (Mapeo de arquitectura y subsistemas):** 70% EN PROGRESO.
- **Completado (con enlace a la evidencia):**
  - Entorno verificado y en estado saludable tras reinicio (`make doctor`).
  - Análisis completo de Ghidra Headless completado con éxito ([03-run-ghidra-recon.sh](file:///home/jcgar2/battledecomp/scripts/recon/03-run-ghidra-recon.sh)).
  - Catálogo de **12.280 funciones** exportadas en `ghidra/exports/functions.tsv`.
  - Decompilación de la cadena completa de carga de contenedores Asura (`0x0017603C`, `0x00023B7C`, `0x00023BEC`, `0x00024D70`, `0x00025158`, `0x00021ADC`, `0x00027E88`, `0x00171F04`, `0x00171DA8`).
  - Implementación funcional en C++17 del cargador de contenedores `.asr` y gestor de recursos ([include/core/asura_archive.h](file:///home/jcgar2/battledecomp/include/core/asura_archive.h), [src/core/resource_mgr.cpp](file:///home/jcgar2/battledecomp/src/core/resource_mgr.cpp)).
  - Implementación del catálogo y subsistema de armas ([include/systems/weapons.h](file:///home/jcgar2/battledecomp/include/systems/weapons.h), [src/systems/weapons.cpp](file:///home/jcgar2/battledecomp/src/systems/weapons.cpp), [ADR-0003](file:///home/jcgar2/battledecomp/docs/decisions/ADR-0003-asura-resource-weapons-architecture.md)).
  - Decompilación y reconstrucción del subsistema de **IA de Bots y Soldados**:
    - Funciones Allegrex analizadas y decompiladas: `0x00188A44` (constructor), `0x00188C30` / `0x00188CC0` (destructores), `0x00188D58` (update loop), `0x00189284` (tracking de objetivo), `0x00188DB8` (filtro de amenazas vivas), `0x00189854` (eliminación y shift de amenazas), `0x0018959C` (coordenadas de target), `0x00188EF4` (switch de modos y telemetría 3D).
    - Mapeo de 9 modos de comportamiento tácticos y 11 sub-acciones de soldados/sables documentadas en [docs/systems/bot-ai.md](file:///home/jcgar2/battledecomp/docs/systems/bot-ai.md) y [ADR-0004](file:///home/jcgar2/battledecomp/docs/decisions/ADR-0004-bot-ai-architecture.md).
    - Implementación C++17 en [include/ai/bot_ai.h](file:///home/jcgar2/battledecomp/include/ai/bot_ai.h) y [src/ai/bot_ai.cpp](file:///home/jcgar2/battledecomp/src/ai/bot_ai.cpp).
  - Suite de pruebas completa integrada en el Makefile (`make test`):
    - [tests/test_weapons_loader.cpp](file:///home/jcgar2/battledecomp/tests/test_weapons_loader.cpp): valida 21 armas en 5 idiomas a partir de `WEAPONNAMES.ASR`.
    - [tests/test_bot_ai.cpp](file:///home/jcgar2/battledecomp/tests/test_bot_ai.cpp): valida 100% de la lógica de estados, capacidad de 5 amenazas, eliminación por shift, filtrado y tracking.
- **Bloqueos / pendiente de mí:** Ninguno.
- **Próximo paso exacto:**
  1. Conectar la selección de armamento de los bots en combate según la amenaza y distancia.
  2. Mapear las tablas de atributos y stats numéricos de combate para las 21 armas (daño, sobrecalentamiento, dispersión).
- **Archivos clave y su ruta:**
  - Binario descifrado: `orig/bin/EBOOT.BIN`
  - Informe de reconocimiento: [docs/knowledge/recon-report.md](file:///home/jcgar2/battledecomp/docs/knowledge/recon-report.md)
  - Desglose de librerías y NIDs: `recon/raw/prx_analysis.md`
  - Exportaciones de memoria: `ghidra/exports/memory_blocks.tsv`
  - Registro de versiones y checksums: [env/versions.lock](file:///home/jcgar2/battledecomp/env/versions.lock), [env/checksums.txt](file:///home/jcgar2/battledecomp/env/checksums.txt)
