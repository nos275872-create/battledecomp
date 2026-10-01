# Estado actual — battledecomp

- **Última actualización:** 2026-10-02 01:46 (Cierre de sesión)
- **Fase actual y progreso:**
  - **Fase 0 (Montaje del entorno e infraestructura):** 100% COMPLETADA.
  - **Fase 1 (Reconocimiento estático del binario):** 90% COMPLETADA.
- **Completado (con enlace a la evidencia):**
  - Estructura completa de carpetas, scripts base (`scripts/lib/run.sh`, `journal.sh`, `doctor.sh`, `backup.sh`, `Makefile`) e inicialización de Git con rama `main`.
  - Extracción automatizada e idempotente de la ISO europea ([01-extract-iso.sh](file:///home/jcgar2/battledecomp/scripts/setup/01-extract-iso.sh)).
  - Compilación e instalación de herramienta nativa `pspdecrypt` ([02-install-pspdecrypt.sh](file:///home/jcgar2/battledecomp/scripts/setup/02-install-pspdecrypt.sh)).
  - Descifrado de `EBOOT.BIN` verificado como 100% idéntico a `BOOT.BIN` (SHA-256: `7e0938f26c677c49a20fff92dce3d5f9c9a553ca1d3885683ea0adf787184deb`) ([03-decrypt-eboot.sh](file:///home/jcgar2/battledecomp/scripts/setup/03-decrypt-eboot.sh)).
  - Instalación y verificación del módulo Allegrex en Ghidra 12.1.4 ([04-setup-ghidra.sh](file:///home/jcgar2/battledecomp/scripts/setup/04-setup-ghidra.sh)).
  - Redacción de ADRs arquitectónicos: [ADR-0001](file:///home/jcgar2/battledecomp/docs/decisions/ADR-0001-eboot-decryption-strategy.md) y [ADR-0002](file:///home/jcgar2/battledecomp/docs/decisions/ADR-0002-ghidra-allegrex-headless-analysis.md).
  - Extracción y clasificación de strings ASCII/UTF-16 ([01-extract-strings.sh](file:///home/jcgar2/battledecomp/scripts/recon/01-extract-strings.sh)).
  - Análisis completo de importaciones PRX y 28 bibliotecas PSP ([02-parse-prx.py](file:///home/jcgar2/battledecomp/scripts/recon/02-parse-prx.py) -> [prx_analysis.md](file:///home/jcgar2/battledecomp/recon/raw/prx_analysis.md)).
  - Informe de reconocimiento exhaustivo ([recon-report.md](file:///home/jcgar2/battledecomp/docs/knowledge/recon-report.md)).
  - Fichas iniciales de subsistemas ([weapons.md](file:///home/jcgar2/battledecomp/docs/systems/weapons.md), [bot-ai.md](file:///home/jcgar2/battledecomp/docs/systems/bot-ai.md), [menus.md](file:///home/jcgar2/battledecomp/docs/systems/menus.md)).
- **Bloqueos / pendiente de mí:** Ninguno. Sesión cerrada de forma limpia; procesos en segundo plano detenidos sin bloqueos residuales.
- **Próximo paso exacto al retomar:**
  1. Relanzar `./scripts/recon/03-run-ghidra-recon.sh` para completar el análisis headless y generar `functions.tsv`.
  2. Mapear las funciones del subsistema seleccionado (armas / sables en `weapons.md` o auto-balance de bots en `bot-ai.md`).
  3. Comenzar la decompilación a C/C++ en `src/` e `include/`.
- **Archivos clave y su ruta:**
  - Binario descifrado: `orig/bin/EBOOT.BIN`
  - Informe de reconocimiento: [docs/knowledge/recon-report.md](file:///home/jcgar2/battledecomp/docs/knowledge/recon-report.md)
  - Desglose de librerías y NIDs: `recon/raw/prx_analysis.md`
  - Exportaciones de memoria: `ghidra/exports/memory_blocks.tsv`
  - Registro de versiones y checksums: [env/versions.lock](file:///home/jcgar2/battledecomp/env/versions.lock), [env/checksums.txt](file:///home/jcgar2/battledecomp/env/checksums.txt)
