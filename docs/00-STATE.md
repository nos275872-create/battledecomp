# Estado actual — battledecomp

- **Última actualización:** 2026-10-02 09:29
- **Fase actual y progreso:**
  - **Fase 0 (Montaje del entorno e infraestructura):** 100% COMPLETADA.
  - **Fase 1 (Reconocimiento estático del binario):** 95% COMPLETADA.
  - **Fase 2 (Mapeo de arquitectura y subsistemas):** 20% EN PROGRESO.
- **Completado (con enlace a la evidencia):**
  - Entorno verificado y en estado saludable tras reinicio (`make doctor`).
  - Proceso de análisis profundo en Ghidra Headless con procesador Allegrex activo en segundo plano ([03-run-ghidra-recon.sh](file:///home/jcgar2/battledecomp/scripts/recon/03-run-ghidra-recon.sh)).
  - Localizada la función principal de dispatcher de IA de soldados/bots en `0x00188EF4` (`BotAI_UpdateStateAndBehavior`) y tabla de modos de combate en `0x003175B0` ([bot-ai.md](file:///home/jcgar2/battledecomp/docs/systems/bot-ai.md)).
  - Localizada la función de carga de recursos del motor Asura en `0x0017603C` (`Text_InitResourceArchives`) y función `Asura_ResourceMgr_LoadArchive` en `0x00023B7C` para paquetes `.asr` de armas ([weapons.md](file:///home/jcgar2/battledecomp/docs/systems/weapons.md)).
  - Herramienta de búsqueda de referencias cruzadas MIPS implementada ([04-find-xrefs.py](file:///home/jcgar2/battledecomp/scripts/recon/04-find-xrefs.py)).
- **Bloqueos / pendiente de mí:** Ninguno. Proceso Ghidra ejecutándose de forma desatendida.
- **Próximo paso exacto:**
  1. Recibir `ghidra/exports/functions.tsv` al terminar el análisis Ghidra.
  2. Iniciar la estructura de cabeceras C++ (`include/`) y fuentes (`src/`) para el subsistema de IA y armas.
- **Archivos clave y su ruta:**
  - Binario descifrado: `orig/bin/EBOOT.BIN`
  - Informe de reconocimiento: [docs/knowledge/recon-report.md](file:///home/jcgar2/battledecomp/docs/knowledge/recon-report.md)
  - Desglose de librerías y NIDs: `recon/raw/prx_analysis.md`
  - Exportaciones de memoria: `ghidra/exports/memory_blocks.tsv`
  - Registro de versiones y checksums: [env/versions.lock](file:///home/jcgar2/battledecomp/env/versions.lock), [env/checksums.txt](file:///home/jcgar2/battledecomp/env/checksums.txt)
