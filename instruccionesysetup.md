# battledecomp — Montaje del proyecto y workflow de trabajo

Eres el ingeniero que lleva este proyecto de decompilación de **Star Wars: Battlefront: Renegade Squadron (PSP, MIPS Allegrex)**. Este documento define **cómo se organiza la carpeta y cómo se trabaja**: estructura, flujos, logs, bitácora y estado. Úsalo para montar `~/battledecomp` y como método de trabajo durante todo el proyecto.

Usa las herramientas que necesites (terminal, edición de archivos, Ghidra en headless, MCP de Ghidra si está disponible, lo que haga falta). Instala lo que te sirva y apúntalo en `env/` (ver abajo).

## 1. Contexto

- Objetivo a largo plazo: decompilación funcional y, si es viable, matching a nivel de instrucciones. Meta final: aprender y entender el motor; quizá un port.
- Se empieza acotado: primero entender el ejecutable, luego un sistema pequeño (armas, IA de bots, menús...).
- Tengo mi ISO de UMD. Necesito el EBOOT.BIN descifrado.
- Mi sistema es Ubuntu. El proyecto vive en `~/battledecomp` (sistema de archivos de Linux).
- Mi nivel: conocimientos medios, terminal y Git básicos; no soy experto en MIPS ni en Ghidra. Haz tú todo el trabajo posible y explícame lo importante.
- Me comunico contigo en español. Código, nombres de archivos y comandos en inglés.
- Si no encuentras la ISO, pregúntame la ruta.

## 2. Estructura de carpetas

Crea esto en `~/battledecomp`:

```
battledecomp/
├── AGENTS.md            # contexto permanente: qué es esto, mapa de carpetas, comandos clave, protocolo de sesión
├── GEMINI.md            # una línea: "Lee AGENTS.md"
├── README.md            # qué es el proyecto y cómo reproducir el entorno
├── Makefile             # targets: doctor, backup, journal
├── .gitignore
├── docs/
│   ├── 00-STATE.md      # estado vivo: dónde estamos, qué falta, próximo paso
│   ├── ROADMAP.md       # fases del proyecto (hechas y futuras)
│   ├── GLOSSARY.md      # glosario explicado para mí (MIPS, PRX, NID, VFPU, matching...)
│   ├── journal/         # bitácora: un archivo por día, YYYY-MM-DD.md
│   ├── decisions/       # ADRs: ADR-0001-titulo.md
│   ├── knowledge/       # hallazgos y mapas: recon-report.md, memory-map.md, imports.md, compiler.md...
│   ├── systems/         # una ficha por sistema del juego (weapons.md, bot-ai.md, menus.md...)
│   └── howto/           # guías cortas: reabrir Ghidra, relanzar análisis, restaurar backup...
├── orig/
│   ├── iso/             # la ISO (copia) y sus hashes
│   ├── extracted/       # contenido extraído de la ISO
│   └── bin/             # EBOOT/BOOT cifrados y descifrados, módulos .prx
├── ghidra/
│   ├── projects/        # proyectos de Ghidra
│   ├── scripts/         # scripts headless propios
│   └── exports/         # salidas de los scripts (funciones, imports, strings...)
├── recon/raw/           # volcados crudos (strings, manifiestos, listados)
├── scripts/
│   ├── lib/             # run.sh, journal.sh, common.sh
│   ├── setup/           # NN-nombre.sh: instalaciones y configuración, repetibles
│   ├── recon/           # scripts de reconocimiento
│   ├── doctor.sh        # comprueba que todo el entorno está en orden
│   └── backup.sh        # copia comprimida de lo importante
├── tools/               # herramientas descargadas o compiladas (ghidra, pspdecrypt, ...)
├── env/                 # versions.lock, downloads.md (URL + hash de lo descargado), checksums.txt, activate.sh
├── logs/
│   ├── commands/        # salida completa de cada comando ejecutado con run.sh + index.tsv
│   ├── setup/
│   └── ghidra/
├── backups/
└── src/ include/ config/ asm/ build/    # vacías con .gitkeep; para la futura fase de decompilación
```

**Git:** `git init` en la raíz, rama `main`. Se versionan `docs/`, `scripts/`, `ghidra/scripts/`, `env/`, `config/`, `src/`, `include/` y los archivos raíz. Las carpetas pesadas o generadas (`orig/`, `tools/`, `logs/`, `backups/`, `ghidra/projects/`, `ghidra/exports/`, `recon/raw/`) van en `.gitignore`. Primer commit al terminar el montaje.

## 3. Archivos base que debes crear

**`AGENTS.md`** debe incluir: objetivo del proyecto, mapa de carpetas, comandos clave (`make doctor`, cómo activar `env/activate.sh`, cómo relanzar cada script de `scripts/setup/`), el protocolo de sesión (sección 4) y las convenciones (sección 5).

**`scripts/lib/run.sh <etiqueta> -- <comando...>`**: ejecuta el comando, guarda stdout+stderr con marca de tiempo en `logs/commands/YYYYmmdd-HHMMSS-<etiqueta>.log`, añade una línea (fecha, etiqueta, comando, código de salida) a `logs/commands/index.tsv` y devuelve el código de salida del comando.

**`scripts/lib/journal.sh "Título" "Texto"`**: añade una entrada a `docs/journal/YYYY-MM-DD.md` (la crea si no existe) con este formato:

```
## HH:MM — Título
- Qué hice:
- Comandos / scripts:
- Resultado y evidencia:
- Problemas:
- Decisiones (enlace al ADR si hay):
- Siguiente paso:
```

**`docs/00-STATE.md`** (se sobrescribe tras cada paso importante):

```
# Estado actual
- Última actualización:
- Fase actual y progreso:
- Completado (con enlace a la evidencia):
- Bloqueos / pendiente de mí:
- Próximo paso exacto:
- Archivos clave y su ruta:
```

**`docs/systems/<sistema>.md`** (una por sistema que se vaya estudiando):

```
# <Sistema>
- Qué hace en el juego:
- Funciones y direcciones relevantes (dirección | nombre propuesto | confianza | notas):
- Estructuras de datos y tamaños:
- Dependencias con otros sistemas:
- Estado (sin empezar / en análisis / decompilado / verificado):
- Dudas abiertas:
```

**`scripts/doctor.sh`**: tabla OK/FALLO con la existencia de carpetas y archivos base, las versiones de las herramientas registradas en `env/versions.lock`, los hashes de `env/checksums.txt` y el estado del repo Git.

**`scripts/backup.sh`**: comprime `docs/`, `env/`, `scripts/`, `ghidra/scripts/` y `ghidra/projects/` en `backups/battledecomp-YYYYmmdd-HHMMSS.tar.gz` y conserva los 5 más recientes.

**`Makefile`**: `make doctor`, `make backup`, `make journal T="título" B="texto"`.

## 4. Workflow de trabajo

**Al empezar cualquier sesión:** lee `AGENTS.md`, `docs/00-STATE.md` y la última entrada de `docs/journal/`. Así retomas el trabajo sin depender de lo que recuerdes.

**Ciclo de trabajo (repítelo en cada tarea):**
1. Define la tarea en una frase y el resultado esperado.
2. Hazla. Lo repetible va en un script (`scripts/...` o `ghidra/scripts/...`), los comandos puntuales pasan por `scripts/lib/run.sh`.
3. Verifica con un comando y mira la salida real antes de dar nada por hecho.
4. Registra: entrada en la bitácora; si aprendiste algo del juego, en `docs/knowledge/` o en la ficha del sistema; si decidiste entre alternativas, un ADR corto.
5. Actualiza `docs/00-STATE.md`.
6. Commit pequeño con mensaje claro (`docs: ...`, `scripts: ...`, `recon: ...`).

**Al terminar una sesión o fase:** resumen en la bitácora, `00-STATE.md` al día, commit, y un mensaje para mí con lo hecho, lo que queda y lo que necesites de mí.

## 5. Convenciones

- Instalaciones y configuración van en `scripts/setup/NN-nombre.sh`, repetibles (si lo ejecutas dos veces no rompe nada) y con `set -euo pipefail`. Lo que instales se anota en `env/versions.lock`; lo que descargues, con URL y hash en `env/downloads.md`.
- Las salidas grandes (strings, desensamblados, listados) van a archivo (`recon/raw/` o `ghidra/exports/`) y se consultan con `head`, `grep`, `wc` para no saturar el contexto. En `docs/knowledge/` solo resúmenes y ejemplos cortos.
- Los procesos largos (análisis de Ghidra, compilaciones) se lanzan en segundo plano con `nohup ... > logs/...log 2>&1 &` y se consultan con `tail`.
- Si algo falla tres veces con enfoques distintos, anota en la bitácora lo que probaste y los errores exactos, y propón alternativas o pídeme ayuda.
- Las decisiones importantes (versión de Ghidra, método de descifrado, compilador candidato...) llevan ADR con contexto, opciones, decisión y motivo.
- Nombres de funciones/símbolos propuestos en las fichas de sistema: dirección, nombre, nivel de confianza (alta/media/baja) y cómo se verificó.
- Cuando uses un término técnico por primera vez, explícalo en 1-2 líneas y añádelo a `docs/GLOSSARY.md`.
- Una pregunta cada vez cuando necesites algo de mí, con pasos exactos y copiables.

## 6. Qué hacer ahora

1. Confirma en 5-8 líneas que has entendido el método y pregúntame la ruta de la ISO si no la encuentras.
2. Crea la estructura, los archivos base y los scripts de la sección 3. Haz `git init` y el primer commit.
3. Prepara el entorno que necesites para el proyecto (extraer la ISO, descifrar el EBOOT, Ghidra en headless con soporte Allegrex, herramientas de apoyo) y regístralo todo siguiendo el workflow.
4. Haz un reconocimiento inicial del ejecutable y guárdalo en `docs/knowledge/recon-report.md`.
5. Cierra con `make doctor`, un backup, `00-STATE.md` actualizado y un resumen para mí.
