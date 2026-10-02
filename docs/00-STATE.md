# Estado actual — battledecomp

- **Última actualización:** 2026-10-02 10:14
- **Fase actual y progreso:**
  - **Fase 0 (Montaje del entorno e infraestructura):** 100% COMPLETADA.
  - **Fase 1 (Reconocimiento estático del binario):** 100% COMPLETADA.
  - **Fase 2 (Mapeo de arquitectura y subsistemas):** 50% EN PROGRESO.
- **Completado (con enlace a la evidencia):**
  - Entorno verificado y en estado saludable tras reinicio (`make doctor`).
  - Análisis completo de Ghidra Headless completado con éxito ([03-run-ghidra-recon.sh](file:///home/jcgar2/battledecomp/scripts/recon/03-run-ghidra-recon.sh)).
  - Catálogo de **12.280 funciones** exportadas en `ghidra/exports/functions.tsv`.
  - Decompilación de la cadena completa de carga de contenedores Asura (`0x0017603C`, `0x00023B7C`, `0x00023BEC`, `0x00024D70`, `0x00025158`, `0x00021ADC`, `0x00027E88`, `0x00171F04`, `0x00171DA8`).
  - Implementación funcional en C++17 del cargador de contenedores `.asr` y gestor de recursos:
    - [include/core/asura_archive.h](file:///home/jcgar2/battledecomp/include/core/asura_archive.h) y [src/core/asura_archive.cpp](file:///home/jcgar2/battledecomp/src/core/asura_archive.cpp).
    - [include/core/resource_mgr.h](file:///home/jcgar2/battledecomp/include/core/resource_mgr.h) y [src/core/resource_mgr.cpp](file:///home/jcgar2/battledecomp/src/core/resource_mgr.cpp).
  - Implementación del catálogo y subsistema de armas (21 armas principales catalogadas e indexadas en 5 idiomas y resolución de recursos HUD por era Prequel/Classic):
    - [include/systems/weapons.h](file:///home/jcgar2/battledecomp/include/systems/weapons.h) y [src/systems/weapons.cpp](file:///home/jcgar2/battledecomp/src/systems/weapons.cpp).
    - [docs/systems/weapons.md](file:///home/jcgar2/battledecomp/docs/systems/weapons.md).
    - [ADR-0003](file:///home/jcgar2/battledecomp/docs/decisions/ADR-0003-asura-resource-weapons-architecture.md).
  - Suite de pruebas de validación automatizada [tests/test_weapons_loader.cpp](file:///home/jcgar2/battledecomp/tests/test_weapons_loader.cpp) integrada en el Makefile (`make test`), ejecutada con éxito sobre los recursos extraídos de la ISO (`WEAPONNAMES.ASR`).
- **Bloqueos / pendiente de mí:** Ninguno.
- **Próximo paso exacto:**
  1. Decompilar y reconstruir el subsistema de IA de bots (`0x00188EF4` `BotAI_UpdateStateAndBehavior` y tabla `0x003175B0`) en `include/ai/` y `src/ai/`.
  2. Mapear las tablas de atributos y stats numéricos de combate para las 21 armas.
- **Archivos clave y su ruta:**
  - Binario descifrado: `orig/bin/EBOOT.BIN`
  - Informe de reconocimiento: [docs/knowledge/recon-report.md](file:///home/jcgar2/battledecomp/docs/knowledge/recon-report.md)
  - Desglose de librerías y NIDs: `recon/raw/prx_analysis.md`
  - Exportaciones de memoria: `ghidra/exports/memory_blocks.tsv`
  - Registro de versiones y checksums: [env/versions.lock](file:///home/jcgar2/battledecomp/env/versions.lock), [env/checksums.txt](file:///home/jcgar2/battledecomp/env/checksums.txt)
