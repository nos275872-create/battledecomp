# Glosario Técnico del Proyecto

Términos clave sobre arquitectura de PlayStation Portable (PSP), ingeniería inversa y decompilación:

- **Allegrex:** CPU de la PSP diseñada por Sony, basada en la arquitectura MIPS32 R4000 con extensiones propietarias (incluye coprocesador vectorial VFPU y soporte little-endian).
- **MIPS:** Arquitectura de procesador RISC (Reduced Instruction Set Computer) caracterizada por instrucciones de tamaño fijo de 32 bits y slots de retardo (*delay slots*).
- **Delay Slot:** Característica de MIPS en la que la instrucción que va inmediatamente después de un salto o llamada a función se ejecuta siempre antes de que el salto sea efectivo.
- **EBOOT.BIN:** Archivo ejecutable principal de un juego o aplicación de PSP, ubicado habitualmente en `PSP_GAME/SYSDIR/EBOOT.BIN`. En discos UMD comerciales viene firmado y cifrado mediante claves de Sony.
- **PRX (PlayStation Relocatable Executable):** Formato de binario ejecutable y biblioteca dinámica en PSP derivado de ELF, que contiene tablas de reubicación y descriptores de módulos del sistema.
- **NID (Name ID):** Hash criptográfico de 32 bits (normalmente derivado de SHA-1) usado en el SDK de PSP para identificar funciones y variables exportadas/importadas sin exponer los nombres en texto plano.
- **VFPU (Vector Floating Point Unit):** Coprocesador matemático especializado de la CPU Allegrex capaz de procesar operaciones vectoriales y matriciales de coma flotante de 1 a 4 componentes en paralelo.
- **Matching:** Proceso y meta en proyectos de decompilación donde el código C/C++ reescrito, al compilarse con las herramientas y flags originales, genera exactamente los mismos bytes de código máquina que el binario original.
- **Ghidra:** Suite de ingeniería inversa y desensamblado desarrollada por la NSA, con soporte para decompilación a pseudocódigo C de múltiples arquitecturas.
- **Headless Analyzer:** Modo por línea de comandos de Ghidra que permite analizar binarios y ejecutar scripts automatizados sin abrir la interfaz gráfica.
- **UMD (Universal Media Disc):** Formato de disco óptico propietario desarrollado por Sony utilizado como medio de distribución físico para la consola PSP.
- **ELF (Executable and Linkable Format):** Formato estándar de archivo binario para ejecutables, código objeto y bibliotecas compartidas utilizado en sistemas Unix y plataformas como PSP.
