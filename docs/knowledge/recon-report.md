# Informe de Reconocimiento Inicial: EBOOT.BIN
**Juego:** Star Wars: Battlefront: Renegade Squadron (PSP)  
**ID / Región:** Europe (swbfffpsp)  
**Fecha de análisis:** 2026-10-02  
**Autor:** battledecomp  

---

## 1. Metadatos del Ejecutable

- **Archivo analizado:** `orig/bin/EBOOT.BIN`
- **Formato:** ELF 32-bit Little-Endian (formato PRX ejecutable de PSP, tipo `0xFFA0`)
- **Arquitectura:** MIPS Allegrex (MIPS-II / MIPS32 R4000 con extensiones VFPU)
- **Tamaño del archivo binario:** 4.2 MB (4,405,808 bytes)
- **Punto de entrada (`Entry Point`):** `0x00281B98`
- **Suma de verificación SHA-256:**  
  `7e0938f26c677c49a20fff92dce3d5f9c9a553ca1d3885683ea0adf787184deb`
- **Verificación de descifrado:** El `EBOOT.BIN` original cifrado (`~PSP`, tag `C0CB167C`) fue descifrado mediante `pspdecrypt` y verificado como **100% idéntico** byte a byte al `BOOT.BIN` incluido en el disco UMD.

---

## 2. Motor de Juego Identificado: Asura Engine

A través de la estructura `sceModuleInfo` y el análisis de cadenas, se confirma que el juego utiliza el **Asura Engine** desarrollado internamente por **Rebellion Developments**:

- **Nombre de módulo PRX:** `Asura`
- **Versión de módulo:** 1.1 (`0x0101`)
- **Gestión de memoria:** Se detecta la clase `Asura_MemHeap` con el log de fallo:  
  `"Asura_MemHeap::Alloc() failed to allocate %i bytes. Largest block is %i."`
- **Capa de red:** Clases `Asura_NetConnection` con soporte de rechazo por versión, contraseña o capacidad (`ASURA_NDR_VERSION`, `ASURA_NDR_PASSWORD`, `ASURA_NDR_SERVERFULL`).
- **Middleware multijugador:** Integración del SDK de **GameSpy** (`gp.h`, servidores `gpcm.gamespy.com`, `gpsp.gamespy.com`, `gamestats.gamespy.com/swbfffpsp`).
- **Formato de recursos:** Archivos empaquetados `.asr` (*Asura Resource*), tales como `Graphics\HUD_Weapons_Prequel.asr`, `Graphics\HUD_Weapons_Classic.asr`, `text/weaponnames.asr` y `text/vehiclenames.asr`.

---

## 3. Mapa de Memoria y Secciones ELF

El ejecutable consta de 64 encabezados de sección y 2 segmentos `LOAD`:

| Sección | Dirección Virtual | Tamaño en Bytes | Permisos | Propósito |
|---------|-------------------|-----------------|----------|-----------|
| `.init` | `0x00000000` | 36 B | `R-X` | Inicialización temprana |
| `.text` | `0x00000040` | 2,942,828 B (~2.94 MB) | `R-X` | Código máquina ejecutable principal |
| `.fini` | `0x002CE7AC` | 28 B | `R-X` | Finalización |
| `.sceStub.text.*` | `0x002CE7C8` | ~1.9 KB | `R-X` | Stubs de llamadas al sistema PSP |
| `.lib.ent` / `.lib.stub` | `0x002CEF5C` | ~580 B | `R--` | Tablas de enlace dinámico PRX |
| `.rodata.sceModuleInfo` | `0x002CF1A8` | 52 B | `R--` | Metadatos y cabecera del módulo SCE |
| `.rodata` | `0x002CF800` | 270,952 B (~265 KB) | `R--` | Constantes, literales y cadenas de texto |
| `.data` | `0x00311A68` | 35,508 B (~35 KB) | `RW-` | Variables globales inicializadas |
| `.eh_frame` / `.gcc_except_table` | `0x0031A51C` | ~5.7 KB | `RW-` | Soporte para excepciones y RTTI de C++ |
| `.ctors` / `.dtors` | `0x0031BBA0` | 2,340 B | `RW-` | Constructores/destructores estáticos C++ |
| `.bss` | `0x0031C500` | 1,148,784 B (~1.15 MB) | `RW-` | Variables globales sin inicializar |

**Huella de memoria estática en RAM:** ~4.4 MB (`0x00000000` a `0x00434C70`).

---

## 4. Dependencias del SDK de PSP (Librerías del Sistema)

El binario importa **28 bibliotecas oficiales** de Sony a través de stubs y NIDs:

1. **Gestión de Procesos y Memoria:** `SysMemUserForUser`, `ThreadManForUser`, `LoadExecForUser`, `ModuleMgrForUser`, `Kernel_Library`.
2. **Entrada / Salida y Almacenamiento:** `IoFileMgrForUser`, `sceUmdUser`.
3. **Gráficos y Pantalla:** `sceGe_user` (GPU Graphics Engine), `sceDisplay` (control de VBLANK y buffer de vídeo).
4. **Controladores e Interfaz:** `sceCtrl` (botones y stick analógico), `sceUtility` (diálogos del sistema PSP).
5. **Sonido y Vídeo:** `sceAudio`, `sceSasCore` (sintetizador por software), `sceAtrac3plus`, `sceMpeg`, `scePsmf`.
6. **Red y Comunicaciones:** `sceNet`, `sceNetInet`, `sceNetAdhoc`, `sceNetAdhocctl`, `sceNetApctl`, `sceNetResolver`, `sceWlanDrv`.
7. **Sistema y Energía:** `scePower`, `sceRtc` (reloj de tiempo real), `sceSuspendForUser`, `UtilsForUser`, `StdioForUser`.

---

## 5. Subsistemas de Gameplay Identificados

A partir del volcado de strings (`recon/raw/strings_ascii.txt`), se han identificado cadenas clave que delimitan subsistemas de interés para la decompilación:

### A. Subsistema de Combate y Armas
- Máquina de estados para combate con sables de luz:
  - `INCOMING_LIGHTSABER_THROW`
  - `MOVING_FOR_LIGHTSABER_ATTACK`
  - `PERFORMING_LIGHTSABER_ATTACK`
  - `MOVING_AWAY_AFTER_LIGHTSABER_ATTACK`
- Archivos de armas: `HUD_Weapons_Prequel.asr`, `HUD_Weapons_Classic.asr`, `text/weaponnames.asr`.

### B. Subsistema de IA de Vehículos
- Máquina de estados clara para navegación y combate espacial/terrestre:
  - `Vehicle - UNSET`
  - `Vehicle - NONE`
  - `Vehicle - TAKE_OFF`
  - `Vehicle - GENERAL_COMBAT`
  - `Vehicle - ATTACK_SHIP`
  - `Vehicle - DEFEND_SHIP`
  - `Vehicle - FOLLOW_SHIP`
  - `Vehicle - CAPTURE_FLAG`
  - `Vehicle - ATTACK_TARGET`
  - `Vehicle - LAND_IN_HANGAR`
  - `Vehicle - GO_TO_POSITION`

### C. Subsistema de Bots e IA de Soldados
- Parámetros de dificultad y nivel: `Difficulty level of the MP servers bots [0=Normal, 1=Elite] (for GUI)`
- Lógica de auto-balance de bots:
  - `"Sets the minimum number of players in a singleplayer game. Bots will be added (or removed) to meet this value."`
  - `"Sets the minimum number of players in a multiplayer infrastructure game. Bots will be added (or removed) to meet this value."`
  - `"Sets the minimum number of players in a multiplayer adhoc game. Bots will be added (or removed) to meet this value."`
- Facciones de bots: `text/botnames_character.asr`, `text/botnames_cis.asr`, `text/botnames_republic.asr`, `text/botnames_alliance.asr`.

---

## 6. Diagnóstico del Compilador y Flujo de Entrada

1. **Cadena de herramientas original:** GCC (Sony PSP SDK oficial de 2007). Presencia de `.gcc_except_table`, `.eh_frame`, `.ctors`, y `.dtors` confirma que la mayor parte del motor y la lógica de juego está escrita en **C++**.
2. **Punto de entrada (`0x00281B98`):**
   - Inicializa registros de argumentos y pila (`sp = sp - 16`).
   - Llama a `0x002CEE58` (`sceKernelSetCompilerVersion`).
   - Llama a `0x002CEE78` (`sceKernelSetCompiledSdkVersion` con versión de SDK `0x0306` = PSP SDK 3.06).
   - Salta al código de inicialización del runtime C++ (constructores estáticos en `.ctors`) antes de llamar a la función `main` del juego.
