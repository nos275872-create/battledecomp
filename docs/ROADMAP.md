# Hoja de Ruta — battledecomp

Este documento detalla las fases planificadas para la decompilación de **Star Wars: Battlefront: Renegade Squadron (PSP)**.

---

## Fase 0: Montaje del Entorno e Infraestructura
- [x] Definición de la estructura de carpetas y flujos de trabajo.
- [x] Configuración de scripts base (`run.sh`, `journal.sh`, `doctor.sh`, `backup.sh`, `Makefile`).
- [ ] Inicialización del repositorio Git y commit inicial.
- [ ] Extracción y verificación de integridad de la ISO del UMD.
- [ ] Descifrado del ejecutable principal (`EBOOT.BIN` -> ELF sin cifrar).
- [ ] Configuración de Ghidra en modo headless con soporte para MIPS Allegrex / PSP.

---

## Fase 1: Reconocimiento Estático del Binario
- [ ] Análisis de cabeceras ELF/PRX y mapa de secciones (`.text`, `.rodata`, `.data`, `.bss`).
- [ ] Extracción e indexación de cadenas de texto (`recon/raw/strings.txt`).
- [ ] Identificación de librerías del sistema y tablas de importación/exportación (NIDs de la PSP).
- [ ] Determinación del compilador original y flags probables (GCC version, optimización `-O2`, `-G0`, etc.).
- [ ] Elaboración del informe de reconocimiento inicial (`docs/knowledge/recon-report.md`).

---

## Fase 2: Mapeo de Arquitectura y Subsistemas
- [ ] Localización de la función de entrada (`entry_point` / `module_start`) y bucle principal del juego.
- [ ] Documentación del mapa de memoria (`docs/knowledge/memory-map.md`).
- [ ] Identificación de los subsistemas principales:
  - Sistema de armas y equipamiento (`docs/systems/weapons.md`).
  - Lógica e IA de soldados y bots (`docs/systems/bot-ai.md`).
  - Gestión de interfaz de usuario y menús (`docs/systems/menus.md`).
  - Motor de audio y efectos.

---

## Fase 3: Decompilación de Subsistemas Acotados
- [ ] Selección del primer subsistema aislado (ej. lógica de armas o tablas de datos).
- [ ] Decompilación a C/C++ funcional en `src/` e `include/`.
- [ ] Creación de tests unitarios o arnés de prueba para lógica aislada.

---

## Fase 4: Matching e Integración
- [ ] Configuración del pipeline de ensamblado/compilación para verificación de matching a nivel de instrucciones.
- [ ] Expansión gradual hacia otros subsistemas del motor.
