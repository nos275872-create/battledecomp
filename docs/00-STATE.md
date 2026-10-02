# Estado actual — battledecomp

- **Última actualización:** 2026-10-02 09:41
- **Fase actual y progreso:**
  - **Fase 0 (Montaje del entorno e infraestructura):** 100% COMPLETADA.
  - **Fase 1 (Reconocimiento estático del binario):** 100% COMPLETADA.
  - **Fase 2 (Mapeo de arquitectura y subsistemas):** 30% EN PROGRESO.
- **Completado (con enlace a la evidencia):**
  - Entorno verificado y en estado saludable tras reinicio (`make doctor`).
  - Análisis completo de Ghidra Headless completado con éxito ([03-run-ghidra-recon.sh](file:///home/jcgar2/battledecomp/scripts/recon/03-run-ghidra-recon.sh)).
  - Catálogo de **12.280 funciones** exportadas en `ghidra/exports/functions.tsv`.
  - Descompilador nativo de Ghidra habilitado y script de decompilación automatizada operativo ([DecompileAddress.java](file:///home/jcgar2/battledecomp/ghidra/scripts/DecompileAddress.java)).
  - Primeras funciones del motor Asura decompiladas a C en `ghidra/exports/decompiled/`:
    - `0x0017603C`: [FUN_0017603c_0017603c.c](file:///home/jcgar2/battledecomp/ghidra/exports/decompiled/FUN_0017603c_0017603c.c) (`Text_InitResourceArchives`)
    - `0x00023B7C`: [FUN_00023b7c_00023b7c.c](file:///home/jcgar2/battledecomp/ghidra/exports/decompiled/FUN_00023b7c_00023b7c.c) (`Asura_ResourceMgr_LoadArchive`)
    - `0x00023BEC`: [FUN_00023bec_00023bec.c](file:///home/jcgar2/battledecomp/ghidra/exports/decompiled/FUN_00023bec_00023bec.c) (`Asura_ResourceMgr_LoadCore`)
  - Dispatcher principal de IA de bots y soldados en `0x00188EF4` (`BotAI_UpdateStateAndBehavior`) y tabla de modos de combate en `0x003175B0` ([bot-ai.md](file:///home/jcgar2/battledecomp/docs/systems/bot-ai.md)).
  - Rutina de carga de recursos y tabla de paquetes `.asr` de armas en `0x00317BE0` ([weapons.md](file:///home/jcgar2/battledecomp/docs/systems/weapons.md)).
- **Bloqueos / pendiente de mí:** Ninguno.
- **Próximo paso exacto:**
  1. Reconstruir en C++ funcional el subsistema de carga de recursos de texto y armas en `src/core/resource_mgr.cpp` e `include/core/resource_mgr.h`.
  2. Decompilar la función del dispatcher de IA `0x00188EF4` y documentar la máquina de estados en `src/ai/bot_ai.cpp`.
- **Archivos clave y su ruta:**
  - Binario descifrado: `orig/bin/EBOOT.BIN`
  - Informe de reconocimiento: [docs/knowledge/recon-report.md](file:///home/jcgar2/battledecomp/docs/knowledge/recon-report.md)
  - Desglose de librerías y NIDs: `recon/raw/prx_analysis.md`
  - Exportaciones de memoria: `ghidra/exports/memory_blocks.tsv`
  - Registro de versiones y checksums: [env/versions.lock](file:///home/jcgar2/battledecomp/env/versions.lock), [env/checksums.txt](file:///home/jcgar2/battledecomp/env/checksums.txt)
