# battledecomp — Star Wars Battlefront: Renegade Squadron Decompilation

Proyecto de ingeniería inversa y decompilación para **Star Wars: Battlefront: Renegade Squadron** (Sony PlayStation Portable - PSP, arquitectura MIPS Allegrex).

El objetivo es comprender la arquitectura del motor (desarrollado originalmente por Rebellion Developments), documentar sus subsistemas (armas, IA de bots, lógica de red y menús) y producir una reimplementación/decompilación funcional en C/C++.

---

## Estructura del Proyecto

- `docs/`: Documentación técnica viva, bitácoras (`journal/`), decisiones de diseño (`decisions/`), mapas de memoria y sistemas analizados.
- `orig/`: Contenedor para la ISO original del UMD, archivos extraídos del sistema de ficheros de PSP y ejecutables (`EBOOT.BIN`, módulos `.prx`). *Ignorado en git*.
- `ghidra/`: Proyectos y scripts automatizados de Ghidra en modo headless.
- `recon/`: Scripts y volcados crudos de strings, tablas de símbolos y análisis estático.
- `scripts/`: Herramientas de automatización (`doctor.sh`, `backup.sh`, `lib/run.sh`).
- `tools/`: Binarios y herramientas de descifrado y análisis. *Ignorado en git*.
- `env/`: Definiciones de entorno, versiones fijadas y script `activate.sh`.
- `src/`, `include/`: Código fuente C/C++ decompilado.

---

## Requisitos y Reproducción del Entorno

### Dependencias del Sistema (Ubuntu / Debian)
- `build-essential` (make, gcc, g++)
- `git`
- `p7zip-full`
- `openjdk-21-jdk` (o superior)
- `python3`

### Herramientas de Decompilación
- **Ghidra** (versión 12.1.4 o compatible)
- Soporte para MIPS32 Little-Endian / Allegrex
- Utilidades de descifrado de PSP (`prxdecrypter` / script de descifrado)

### Primeros Pasos

1. Clonar el repositorio y entrar al directorio:
   ```bash
   cd ~/battledecomp
   ```

2. Activar variables de entorno:
   ```bash
   source env/activate.sh
   ```

3. Verificar la integridad y herramientas instaladas:
   ```bash
   make doctor
   ```

4. Extraer la ISO y descifrar el binario:
   ```bash
   ./scripts/setup/01-extract-iso.sh
   ./scripts/setup/02-decrypt-eboot.sh
   ```

5. Ejecutar diagnóstico o pruebas:
   ```bash
   make backup
   ```

Consulte [AGENTS.md](AGENTS.md) para el protocolo de trabajo detallado y [docs/00-STATE.md](docs/00-STATE.md) para conocer el estado actual del desarrollo.
