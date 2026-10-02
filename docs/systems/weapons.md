# Sistema de Armas y Equipamiento (Weapons)

- **Qué hace en el juego:** Gestiona los tipos de armamento (blasters, lanzacohetes, granadas, sables de luz), mecánicas de disparo, cadencia, calentamiento/munición, dispersión, proyectiles y animaciones asociadas. Incluye máquinas de estado de ataque cuerpo a cuerpo para sables (`INCOMING_LIGHTSABER_THROW`, `PERFORMING_LIGHTSABER_ATTACK`).
- **Funciones y direcciones relevantes (dirección | nombre propuesto | confianza | notas):**
  - `0x0017603C` | `Text_InitResourceArchives` | Alta | Inicializa y carga en bucle todos los archivos `.asr` de texto del juego.
  - `0x00023B7C` | `Asura_ResourceMgr_LoadArchive` | Alta | Función del motor Asura que carga y registra contenedores `.asr` en memoria.
  - `0x00023BEC` | `Asura_ResourceMgr_LoadCore` | Alta | Núcleo de apertura y gestión de refcount de contenedores.
  - `0x00024D70` | `Asura_ParseArchive` | Alta | Despachador de chunks FourCC de archivos `.asr`.
  - `0x00025158` | `Asura_ParseChunk_PTXT` | Alta | Parser de tablas de texto plano y cadenas UTF-16LE localizadas.
  - `0x00171F04` | `Weapons_GetHudArchiveName` | Alta | Resuelve el contenedor HUD de armas según la era de juego (`0x00317bdc`): Prequel (`0`) vs Classic (`1`).
  - `0x00171DA8` | `Weapons_SetupMenuAndHud` | Alta | Vincula menús de personalización y recursos HUD para armamento.
  - `0x00176098` | Bucle de carga de `text/*.asr` | Alta | Itera sobre la tabla `0x00317BE0` hasta encontrar terminador NULL.

- **Catálogo de Armamento Identificado (21 armas principales en `WEAPONNAMES.ASR`):**
  | ID | Identificador Interno | Inglés | Español |
  |---|---|---|---|
  | 00 | `WP_BLASTER_RIFLE` | Blaster Rifle | Fusil bláster |
  | 01 | `WP_BLASTER_PISTOL` | Blaster Pistol | Pistola bláster |
  | 02 | `WP_ARC_CASTER` | Arc Caster | Lanzaarcos voltaicos |
  | 03 | `WP_FUSION_CUTTER` | Fusion Cutter | Cortador de fusión |
  | 04 | `WP_CARBONITE_FREEZE_GUN` | Carbonite Freeze Gun | Arma de carbonita |
  | 05 | `WP_TRI_SHOT` | Tri Shot | Disparo triple |
  | 06 | `WP_INCINERATOR` | Incinerator | Incinerador |
  | 07 | `WP_WRIST_ROCKET` | Wrist Rocket | Cohete de muñeca |
  | 08 | `WP_SHOTGUN` | Shotgun | Escopeta |
  | 09 | `WP_EMP_LAUNCHER` | EMP Launcher | Lanzador de IEM |
  | 10 | `WP_SNIPER_RIFLE` | Sniper Rifle | Fusil de precisión |
  | 11 | `WP_EXPLOSIVE_BLASTER_PISTOL` | Explosive Blaster Pistol | Pistola bláster explosiva |
  | 12 | `WP_CHAINGUN` | Chaingun | Ametralladora rotatoria |
  | 13 | `WP_GRENADE_LAUNCHER` | Grenade Launcher | Lanzagranadas |
  | 14 | `WP_BOWCASTER` | Bowcaster | Ballesta láser |
  | 15 | `WP_GUIDED_ROCKET` | Guided Rocket | Cohete teledirigido |
  | 16 | `WP_ROCKET_LAUNCHER` | Rocket Launcher | Lanzador de IEM |
  | 17 | `WP_THERMAL_DETONATOR` | Thermal Detonator | Detonador térmico |
  | 18 | `WP_DETPACKS` | Detpacks | Paquetes explosivos |
  | 19 | `WP_MINES` | Mines | Minas |
  | 20 | `WP_CLUSTER_GRENADE` | Cluster Grenade | Granada racimo |

- **Estructuras de datos y tamaños:**
  - Formato contenedor Asura (`.asr`):
    - Magic 8 bytes: `"Asura   "` (sin comprimir) / `"AsuraCmp"` (comprimido).
    - Chunks secuenciales: FourCC (4 bytes), tamaño chunk (uint32).
    - Chunk `PTXT` (`0x54585450`): versión (5), flags, id de tabla (`0xe97f06ac`), conteo de cadenas (21), idioma (uint32), nombre de tabla alineado a 4 bytes, seguido de cadenas longitud (uint32) + payload UTF-16LE.
  - Tabla de Recursos de Texto (`0x00317BE0` en `.data`):
    - `0x00317BE0`: `"text/stdtext.asr"`
    - `0x00317BE4`: `"text/cpnames.asr"`
    - `0x00317BE8`: `"text/GalacticConquest.asr"`
    - `0x00317BEC`: `"text/objectnames.asr"`
    - `0x00317BF0`: `"text/missionobjectives.asr"`
    - `0x00317BF4`: `"text/vehiclenames.asr"`
    - `0x00317BF8`: `"text/weaponnames.asr"` (Nombres y descriptores de armas)
    - `0x00317BFC`: `"text/botnames_character.asr"`
    - `0x00317C00`: `"text/EndGame_Message.asr"`
    - `0x00317C04`: `"text/LoadingHints_ConquestGround.asr"`
    - `0x00317C08`: `"text/LoadingHints_ConquestSpace.asr"`
    - `0x00317C0C`: `"text/LoadingHints_CTF.asr"`
    - `0x00317C10`: `"text/LoadingHints_GC.asr"`
  - Recursos HUD de Armamento:
    - `"Graphics\HUD_Weapons_Prequel.asr"` (`0x002D82BC` / vaddr `0x002DC339`)
    - `"Graphics\HUD_Weapons_Classic.asr"` (`0x002D82E0` / vaddr `0x002DC35D`)
- **Implementación funcional C++:**
  - Lector de contenedores Asura: `include/core/asura_archive.h`, `src/core/asura_archive.cpp`.
  - Gestor de recursos: `include/core/resource_mgr.h`, `src/core/resource_mgr.cpp`.
  - Subsistema de armas: `include/systems/weapons.h`, `src/systems/weapons.cpp`.
  - Pruebas y validación cruzada: `tests/test_weapons_loader.cpp` (`make test`).
- **Dependencias con otros sistemas:** Sistema de físicas/colisiones, gestión de proyectiles, HUD/UI, perfiles de personalización de armas.
- **Estado (sin empezar / en análisis / decompilado / verificado):** Decompilado y verificado (carga de recursos y catálogo de armas).
- **Dudas abiertas:** Mapeo de valores de balance de combate por arma (daño, dispersión, sobrecalentamiento) en binario o tablas asociadas.
