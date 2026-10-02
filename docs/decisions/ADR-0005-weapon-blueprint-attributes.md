# ADR-0005: Arquitectura del Sistema de Blueprints y Mapeo de Atributos de Combate de Armas

## Estado
Aceptado

## Fecha
2026-10-02

## Contexto
En *Star Wars: Battlefront: Renegade Squadron*, las estadísticas de combate de las armas (cadencia de disparo `RateOfFire`, tiempo de recarga `ReloadTime`, capacidad de cargadores `AmmoPerClip` y `MaxClips`, calentamiento `OverheatPershot` / `OverheatDecay`, velocidad y daño a infantería y vehículos) no se encuentran codificadas de forma estática en las secciones `.data` o `.rodata` del ejecutable `EBOOT.BIN`.

El motor Asura v1.1 delega la definición de todas las entidades, armas, proyectiles, vehículos y efectos visuales al sistema de **Blueprints**, empaquetado en chunks FourCC `'BLUE'` (`0x45554C42`) dentro del contenedor global `orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR`.

Al decompilar las funciones del motor:
- `0x0001A07C` / `0x0001A0C8`: Despacho del chunk `'BLUE'`.
- `0x00018718`: Lector principal de la tabla de Blueprints.
- `0x00017BC0`: Parser de propiedades.
- `0x00017D90`: Deserializador de elementos primitivos (enteros, flotantes, booleanos y cadenas de caracteres con lectura en bloques de 4 bytes terminados en `\0`).
- `0x00029D88`: Lector de cadenas de texto alineado a 4 bytes.

Descubrimos que:
1. `COMMON.ASR` contiene 16 bloques de Blueprints (`Weapon`, `Projectile`, `LaserBolt`, `Tank`, `Turret`, `Flyer`, `Humanoid`, etc.).
2. El Blueprint `Weapon` define 212 plantillas. Los arquetipos base (`BlasterRifle`, `BlasterPistol`, `Shotgun`, `Sniper Rifle`, `Rocket_Launcher`, etc.) contienen los atributos numéricos de balance de combate.
3. Las armas de cada facción (`ALL_Blaster_Pistol`, `REP_Blaster_Rifle`, `EMP_Rocket_Launcher`, etc.) heredan de estos arquetipos, sobreescribiendo identificadores visuales, modelos 3D y partículas mientras conservan las propiedades mecánicas.
4. El daño, radio de explosión y velocidad se resuelven mediante el campo `Projectile`, enlazando dinámicamente con los Blueprints `LaserBolt` (armas de energía) o `Projectile` (misiles, cohetes, granadas y minas).

## Alternativas Evaluadas
1. **Hardcodear tablas numéricas estáticas en `src/systems/weapons.cpp`:**
   - Escribir arrays de constantes flotantes para las 21 armas.
   - *Desventaja:* Rompe la correspondencia con los archivos originales del juego de PSP, impide la carga de mods o variaciones de assets, y desconecta la arquitectura del motor Rebellion Asura.
2. **Implementar el deserializador nativo de Blueprints en C++ (`AsuraBlueprintArchive`):**
   - Reproducir fielmente el lector binario de chunks `'BLUE'`, resolviendo propiedades por nombre y herencia de arquetipo (`Base`).
   - Soportar búsqueda case-insensitive para tolerar diferencias de mayúsculas/minúsculas entre plantillas de armas y proyectiles (`"rocket"` vs `"Rocket"`).
   - Validar la carga y los atributos numéricos con pruebas unitarias automáticas (`make test`).

## Decisión
Se implementa la alternativa 2:
- `include/core/asura_blueprint.h` y `src/core/asura_blueprint.cpp`: Parser general de Blueprints de Asura Engine.
- `include/systems/weapons.h` y `src/systems/weapons.cpp`: Estructura `WeaponCombatStats`, función `Weapons_LoadBlueprints()` y resolución en tiempo real de estadísticas de combate enlazando `Weapon`, `LaserBolt` y `Projectile`.
- `tests/test_weapon_attributes.cpp`: Suite de pruebas que certifica 100% de los valores contra `COMMON.ASR`.

## Consecuencias
- Quedan completamente descifrados y accesibles todos los datos de balance del juego (daño a infantería/vehículos, dispersión, velocidad, calentamiento y munición).
- El sistema de IA de bots (`BotAIController`) puede consultar el rango efectivo y letalidad de cada arma para la toma de decisiones tácticas en combate.
- Se mantiene 100% de compatibilidad con los activos originales del disco UMD de PSP.
