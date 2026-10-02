# Sistema de Armas y Equipamiento (Weapons)

- **Qué hace en el juego:** Gestiona los tipos de armamento (blasters, lanzacohetes, granadas, sables de luz), mecánicas de disparo, cadencia, calentamiento/munición, dispersión, proyectiles y animaciones asociadas. Incluye máquinas de estado de ataque cuerpo a cuerpo para sables (`INCOMING_LIGHTSABER_THROW`, `PERFORMING_LIGHTSABER_ATTACK`).
- **Funciones y direcciones relevantes (dirección | nombre propuesto | confianza | notas):**
  - `0x0017603C` | `Text_InitResourceArchives` | Alta | Inicializa y carga en bucle todos los archivos `.asr` de texto del juego.
  - `0x00023B7C` | `Asura_ResourceMgr_LoadArchive` | Alta | Función del motor Asura que carga y registra contenedores `.asr` en memoria.
  - `0x00023BEC` | `Asura_ResourceMgr_LoadCore` | Alta | Núcleo de apertura y gestión de refcount de contenedores.
  - `0x00024D70` | `Asura_ParseArchive` | Alta | Despachador de chunks FourCC de archivos `.asr`.
  - `0x00025158` | `Asura_ParseChunk_PTXT` | Alta | Parser de tablas de texto plano y cadenas UTF-16LE localizadas.
  - `0x0001A07C` | `Asura_Blueprint_ChunkHandler` | Alta | Despachador de chunks FourCC `'BLUE'` (Blueprints).
  - `0x00018718` | `Asura_Blueprint_ReadTable` | Alta | Lee la tabla de Blueprints (conteo, ID, nombre y propiedades).
  - `0x00017BC0` | `Asura_Blueprint_ParseProperty` | Alta | Analiza y deserializa descriptores de propiedades y elementos.
  - `0x00017D90` | `Asura_Blueprint_DeserializeElement` | Alta | Lector de tipos elementales (enteros, floats, bools, strings con padding a 4 bytes).
  - `0x00171F04` | `Weapons_GetHudArchiveName` | Alta | Resuelve el contenedor HUD de armas según la era de juego (`0x00317bdc`): Prequel (`0`) vs Classic (`1`).
  - `0x00171DA8` | `Weapons_SetupMenuAndHud` | Alta | Vincula menús de personalización y recursos HUD para armamento.
  - `0x00176098` | Bucle de carga de `text/*.asr` | Alta | Itera sobre la tabla `0x00317BE0` hasta encontrar terminador NULL.

- **Catálogo de Armamento Identificado (21 armas principales en `WEAPONNAMES.ASR` y `COMMON.ASR`):**
  | ID | Identificador Interno | Plantilla Blueprint | Munición / Cargador | Cadencia (RoF) | Recarga | Calor/Disparo (Decay) | Proyectil | Daño Inf/Veh |
  |---|---|---|---|---|---|---|---|---|
  | 00 | `WP_BLASTER_RIFLE` | `BlasterRifle` | 25 / 5 | 0.30s | 2.25s | N/A (Munición) | `RifleLaser` | 15.0 / 6.0 |
  | 01 | `WP_BLASTER_PISTOL` | `BlasterPistol` | Calor (-1/-1) | 0.50s | N/A | +20.0 (-18.0/s) | `PistolLaser` | 10.0 / 5.0 |
  | 02 | `WP_ARC_CASTER` | `ArcCaster` | Calor (-1/-1) | 0.50s | N/A | +25.0 (-20.0/s) | `ShockCannon` | 30.0 / 15.0 |
  | 03 | `WP_FUSION_CUTTER` | `Fusion Cutter` | Calor (-1/-1) | 0.25s | N/A | +10.0 (-30.0/s) | `Fusion Cutter` | 5.0 / 25.0 |
  | 04 | `WP_CARBONITE_FREEZE_GUN` | `Carbonite_Gun` | Calor (-1/-1) | 0.40s | N/A | +15.0 (-15.0/s) | `Carbonite Beam` | 8.0 / 0.0 |
  | 05 | `WP_TRI_SHOT` | `TriShot` | 24 / 4 | 0.80s | 2.50s | N/A (Munición) | `PistolLaser` | 10.0 / 5.0 (x3) |
  | 06 | `WP_INCINERATOR` | `Flame Thrower` | Calor (-1/-1) | 0.10s | N/A | +5.0 (-25.0/s) | `FlameThrower` | 12.0 / 4.0 |
  | 07 | `WP_WRIST_ROCKET` | `Wrist_Rocket` | 4 / 0 | 1.25s | N/A | N/A (Cohetes) | `Wrist_Rocket` | 75.0 / 120.0 (R=6m) |
  | 08 | `WP_SHOTGUN` | `Shotgun` | 25 / 6 | 1.50s | 2.25s | N/A (Munición) | `ShotgunLaser` | 10.0 / 5.0 (x8) |
  | 09 | `WP_EMP_LAUNCHER` | `Emp_Launcher` | 3 / 3 | 1.00s | 3.00s | N/A (Munición) | `EMP_Ball` | 20.0 / 200.0 |
  | 10 | `WP_SNIPER_RIFLE` | `Sniper Rifle` | 5 / 5 | 2.00s | 0.50s | N/A (Munición) | `Hit Scan` | 120.0 / 15.0 |
  | 11 | `WP_EXPLOSIVE_BLASTER_PISTOL` | `Particle_Pistol` | 1 / 8 | 1.00s | 1.50s | N/A (Munición) | `Human_ParticleBeam` | 45.0 / 30.0 |
  | 12 | `WP_CHAINGUN` | `Chaingun` | Calor (-1/-1) | 0.12s | N/A | +4.0 (-22.0/s) | `ChaingunLaser` | 12.0 / 8.0 |
  | 13 | `WP_GRENADE_LAUNCHER` | `Grenade_Launcher_OnTime` | 5 / 2 | 0.80s | 2.80s | N/A (Munición) | `Launched_Grenade_OnTime` | 80.0 / 120.0 |
  | 14 | `WP_BOWCASTER` | `BowCaster` | Calor (-1/-1) | 0.60s | N/A | +18.0 (-16.0/s) | `BowcasterBolt` | 35.0 / 20.0 |
  | 15 | `WP_GUIDED_ROCKET` | `Guided_Rocket` | 1 / 4 | 2.00s | 5.00s | N/A (Munición) | `Guided_Rocket` | 140.0 / 250.0 |
  | 16 | `WP_ROCKET_LAUNCHER` | `Rocket_Launcher` | 1 / 7 | 1.00s | 4.40s | N/A (Munición) | `rocket` | 150.0 / 275.0 (R=8m) |
  | 17 | `WP_THERMAL_DETONATOR` | `REP_Grenade` | 4 / 0 | 1.00s | N/A | N/A (Granadas) | `Grenade` | 100.0 / 150.0 (R=8m) |
  | 18 | `WP_DETPACKS` | `Det_Pack` | 3 / 0 | 0.50s | N/A | N/A (C4) | `Det_Pack` | 200.0 / 350.0 (R=10m) |
  | 19 | `WP_MINES` | `Proximity_Mines` | 4 / 0 | 1.00s | N/A | N/A (Minas) | `Proximity_Mines` | 150.0 / 300.0 (R=7m) |
  | 20 | `WP_CLUSTER_GRENADE` | `Cluster_Grenades` | 5 / 0 | 1.00s | N/A | N/A (Granadas) | `Cluster_Grenade` | 60.0 / 80.0 (x5) |

- **Estructura del Sistema de Blueprints (`COMMON.ASR`):**
  - Chunks `'BLUE'` (`0x45554C42`):
    - Almacenan 16 categorías de Blueprints: `Weapon` (212 plantillas), `Projectile` (39 plantillas), `LaserBolt` (51 plantillas), `Humanoid` (48 plantillas), `Flyer` (41 plantillas), `Tank` (21 plantillas), `Turret` (48 plantillas), `Walker` (7 plantillas), etc.
    - Serialización binaria:
      - Despacho: `FUN_0001A07C` / `FUN_0001A0C8`.
      - Lector de tabla: `FUN_00018718` lee conteo de blueprints, ID uint32, conteo de propiedades y nombre textual.
      - Parser de propiedades: `FUN_00017BC0` lee versión (3), hash de propiedad uint32, flags y conteo de elementos.
      - Deserializador: `FUN_00017D90` procesa tipos primitivos:
        - `tag 0`: entero int32 / float32
        - `tag 1`: flotante float32
        - `tag 2`: booleano (1 byte con lectura directa)
        - `tag 3` y `tag 4`: cadena de texto o símbolo (bloques de 4 bytes hasta `\0`).
  - Jerarquía de plantillas:
    - Las armas faccionales (`ALL_Blaster_Rifle`, `REP_Blaster_Rifle`, `EMP_Blaster_Rifle`, `FED_Blaster_Rifle`) derivan de arquetipos base (`BlasterRifle`, `BlasterPistol`, etc.), heredando cadencia, daño, proyectil y tiempos de recarga mientras sobreescriben modelos 3D y partículas (`PFXMuzzleFlash`).

- **Implementación funcional C++:**
  - Lector de Blueprints: `include/core/asura_blueprint.h`, `src/core/asura_blueprint.cpp`.
  - Subsistema de armas: `include/systems/weapons.h`, `src/systems/weapons.cpp`.
  - Pruebas y validación cruzada:
    - `tests/test_weapons_loader.cpp` (21 armas en 5 idiomas a partir de `WEAPONNAMES.ASR`).
    - `tests/test_weapon_attributes.cpp` (validación de carga de Blueprints de `COMMON.ASR` y atributos de combate numéricos).
    - Ejecutables validados 100% con `make test`.

- **Estado:** Decompilado y verificado (carga de recursos, catálogo, blueprints y atributos de combate).
