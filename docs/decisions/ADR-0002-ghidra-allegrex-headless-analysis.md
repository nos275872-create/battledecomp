# ADR-0002: Elección de Ghidra y Módulo Allegrex para Ingeniería Inversa

## Estado
Aceptado

## Contexto
La CPU de la PSP es un MIPS Allegrex (derivado de MIPS32 R4000 con unidad vectorial VFPU de 128 bits y extensiones propietarias). Se requiere una plataforma de desensamblado y decompilación automatizable en entorno Linux (servidor/CLI) para analizar los ~2.94 MB de código máquina del juego y asistir en la reimplementación funcional en C/C++.

## Alternativas Consideradas
1. **IDA Pro:** Plataforma comercial de referencia.
   - *Desventaja:* Requiere licencia propietaria de pago y plugins específicos para PSP/Allegrex; difícil integración headless abierta.
2. **Ghidra base (con procesador MIPS32 estándar `MIPS:LE:32:default`):**
   - *Desventaja:* No decodifica instrucciones de la VFPU (Vector Floating Point Unit) ni llamadas específicas del loader de PSP, generando instrucciones inválidas en rutinas matemáticas, de física y renderizado.
3. **Ghidra con extensión `kotcrab/ghidra-allegrex`:**
   - *Ventajas:* Soporte completo de la arquitectura Allegrex (incluyendo VFPU decodificada y decompilada a pseudocódigo), reconocimiento automático de PRX/ELF de PSP, tablas de reubicación SCE tipo A/B y soporte headless.

## Decisión
Se adopta **Ghidra 12.1.4_PUBLIC** con el módulo de procesador **ghidra-allegrex v21.4** instalado en `/opt/ghidra_12.1.4_PUBLIC/Ghidra/Processors/Allegrex`. El identificador de procesador configurado es `Allegrex:LE:32:default`.

La ejecución se realiza de manera headless mediante `/opt/ghidra_12.1.4_PUBLIC/support/analyzeHeadless` con scripts de exportación automatizados en `ghidra/scripts/` para volcar mapas de funciones y memoria sin intervención manual.

## Consecuencias
- Desensamblado y decompilación precisos tanto para instrucciones de propósito general como vectoriales (VFPU).
- Capacidad de ejecutar análisis completos y scripts en segundo plano registrando logs en `logs/ghidra/`.
