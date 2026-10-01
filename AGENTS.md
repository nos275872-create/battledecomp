# AGENTS.md — Protocolo y Contexto Permanente

Este archivo define el contexto permanente, las convenciones y el flujo de trabajo para cualquier agente de IA o ingeniero que trabaje en el proyecto **battledecomp**.

---

## 1. Objetivo del Proyecto

Decompilar **Star Wars: Battlefront: Renegade Squadron (PSP, MIPS Allegrex)**.
- **Objetivo a largo plazo:** Decompilación funcional y matching a nivel de instrucciones (C/C++). Meta final: aprender y entender el motor de juego (Renegade Engine / Rebellion); eventual portabilidad.
- **Enfoque inicial acotado:**
  1. Entender el ejecutable (`EBOOT.BIN` / PRX, puntos de entrada, estructura de memoria, librerías importadas).
  2. Identificar y decompilar subsistemas aislados y pequeños (gestión de armas, lógica básica de bots, menús/UI).
- **Idioma y comunicación:** Comunicación con el usuario en **español**. Código fuente, nombres de archivos, identificadores y comandos de terminal en **inglés**.

---

## 2. Mapa de Carpetas

```
battledecomp/
├── AGENTS.md            # Contexto permanente, mapa de carpetas, comandos clave, protocolo
├── GEMINI.md            # Redirección rápida ("Lee AGENTS.md")
├── README.md            # Visión general y guía de reproducción del entorno
├── Makefile             # Comandos rápidos: doctor, backup, journal
├── .gitignore           # Exclusiones de Git (archivos pesados, dumps crudos, proyectos)
├── docs/
│   ├── 00-STATE.md      # Estado vivo: dónde estamos, qué falta, próximo paso
│   ├── ROADMAP.md       # Fases del proyecto (completadas y futuras)
│   ├── GLOSSARY.md      # Glosario técnico accesible (MIPS, PRX, NID, VFPU, matching...)
│   ├── journal/         # Bitácora diaria: YYYY-MM-DD.md
│   ├── decisions/       # ADRs (Architecture Decision Records): ADR-NNNN-titulo.md
│   ├── knowledge/       # Hallazgos y mapas técnicos (recon-report.md, memory-map.md, etc.)
│   ├── systems/         # Ficha por subsistema del juego (weapons.md, bot-ai.md...)
│   └── howto/           # Guías prácticas cortas
├── orig/
│   ├── iso/             # Copia de la ISO de UMD y hashes SHA256
│   ├── extracted/       # Contenido extraído del disco
│   └── bin/             # EBOOT/BOOT cifrados y descifrados, módulos .prx
├── ghidra/
│   ├── projects/        # Proyectos headless y GUI de Ghidra
│   ├── scripts/         # Scripts de análisis en Ghidra (Java/Python)
│   └── exports/         # Salidas de scripts (funciones, imports, tablas de símbolos)
├── recon/raw/           # Volcados brutos (strings, manifiestos, dumps hex)
├── scripts/
│   ├── lib/             # run.sh, journal.sh, common.sh
│   ├── setup/           # NN-nombre.sh: aprovisionamiento repetible e idempotente
│   ├── recon/           # Scripts de reconocimiento estático y dinámico
│   ├── doctor.sh        # Diagnóstico del entorno y herramientas
│   └── backup.sh        # Copia comprimida rotativa (5 versiones)
├── tools/               # Binarios y herramientas compiladas/descargadas
├── env/                 # versions.lock, downloads.md, checksums.txt, activate.sh
├── logs/
│   ├── commands/        # Salida completa de run.sh + index.tsv
│   ├── setup/           # Logs de ejecución de scripts de setup
│   └── ghidra/          # Logs de análisis headless de Ghidra
├── backups/             # Archivos tar.gz generados por backup.sh
└── src/ include/ config/ asm/ build/    # Código decompilado, cabeceras, asm y artefactos de compilación
```

---

## 3. Comandos Clave

- **Diagnóstico del entorno:**
  ```bash
  make doctor
  # o bien: ./scripts/doctor.sh
  ```
- **Activar variables de entorno:**
  ```bash
  source env/activate.sh
  ```
- **Ejecutar comandos trazables (OBLIGATORIO para tareas puntuales):**
  ```bash
  scripts/lib/run.sh <tag> -- <comando y argumentos>
  ```
- **Añadir entrada a la bitácora:**
  ```bash
  make journal T="Título del hito" B="Descripción breve o cuerpo"
  # o bien: ./scripts/lib/journal.sh "Título" "Texto"
  ```
- **Crear respaldo:**
  ```bash
  make backup
  # o bien: ./scripts/backup.sh
  ```
- **Relanzar scripts de setup:**
  ```bash
  ./scripts/setup/01-extract-iso.sh
  ./scripts/setup/02-decrypt-eboot.sh
  ./scripts/setup/03-setup-ghidra.sh
  ```

---

## 4. Protocolo de Sesión

### Al comenzar cualquier sesión
1. Lee `AGENTS.md`.
2. Lee `docs/00-STATE.md` para entender el punto exacto de avance y próximos pasos.
3. Lee la última entrada en `docs/journal/` para recordar el contexto inmediato reciente.

### Ciclo de trabajo (para cada tarea)
1. **Definir:** Expresa en una frase clara la tarea y el resultado esperado.
2. **Ejecutar:**
   - Si es repetible, crea o actualiza un script en `scripts/...` o `ghidra/scripts/...`.
   - Si es puntual, ejecútalo mediante `scripts/lib/run.sh <etiqueta> -- <comando>`.
3. **Verificar:** Comprueba la salida real (código de salida, contenido de ficheros) antes de asumir éxito.
4. **Registrar:**
   - Añade una entrada a `docs/journal/YYYY-MM-DD.md`.
   - Si se obtuvo conocimiento nuevo del juego, documéntalo en `docs/knowledge/` o `docs/systems/`.
   - Si se tomó una decisión arquitectónica o técnica entre alternativas, redacta un ADR en `docs/decisions/`.
5. **Actualizar estado:** Modifica `docs/00-STATE.md`.
6. **Commit:** Realiza un commit pequeño y atómico (`git commit -m "tipo: descripción"`).

### Al terminar una sesión o fase
- Resumen completo en la bitácora del día.
- `docs/00-STATE.md` actualizado con el bloqueo actual o siguiente paso.
- Commit final en Git.
- Reporte conciso al usuario en español explicando lo realizado, el estado actual y qué se necesita de él.

---

## 5. Convenciones del Proyecto

1. **Scripts de instalación (`scripts/setup/NN-*.sh`):**
   - Deben comenzar con `set -euo pipefail`.
   - Deben ser estrictamente idempotentes (ejecutarlos múltiples veces no rompe nada).
   - Registrar herramientas en `env/versions.lock` y descargas con URL + SHA256 en `env/downloads.md`.
2. **Manejo de salidas pesadas:**
   - Salidas largas (listas de strings, desensamblados masivos) van a `recon/raw/` o `ghidra/exports/`.
   - Analizarlas con `head`, `grep`, `wc`, `awk` sin saturar el contexto del agente.
   - En `docs/knowledge/` solo incluir resúmenes sintetizados y fragmentos mínimos de ejemplo.
3. **Procesos en segundo plano:**
   - Tareas largas (análisis Ghidra) se ejecutan con logs redirigidos y se monitorizan con comprobación de estado o notificaciones.
4. **Regla de los 3 fallos:**
   - Si algo falla 3 veces con enfoques distintos, se registra en la bitácora el detalle de los errores, se formulan alternativas y se consulta al usuario.
5. **Decisiones Técnicas (ADR):**
   - Decisiones de herramientas, compiladores, flags o métodos de descifrado requieren ADR con: Contexto, Alternativas, Decisión y Justificación.
6. **Símbolos y funciones:**
   - Al proponer funciones en `docs/systems/`: registrar dirección virtual, nombre propuesto, nivel de confianza (alta/media/baja) y método de comprobación.
7. **Glosario:**
   - Cada concepto técnico nuevo se define en 1-2 líneas y se añade a `docs/GLOSSARY.md`.
8. **Consultas al usuario:**
   - Una pregunta a la vez, con comandos o pasos exactos si se requiere acción suya.
