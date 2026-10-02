# ADR-0007: Arquitectura del Subsistema de Vehículos, Torretas e IA Vehicular

## Estado
Aceptado

## Fecha
2026-10-02

## Contexto
El combate vehicular y espacial es un componente central de *Star Wars: Battlefront: Renegade Squadron*. El juego presenta una diversidad sustancial de unidades mecanizadas:
- Caminantes terrestres pesados (`Walker`: AT-AT, AT-TE, AT-ST, AT-RT, Spider Walker).
- Tanques repulsores y de orugas (`Tank`: AAT, IFTX, T4B, Juggernaut, speeder bikes).
- Naves y cazas espaciales/atmosféricos (`Flyer`: X-Wing, TIE Fighter, A-Wing, Millennium Falcon...).
- Emplazamientos artillados y defensivos (`Turret`: Dish Turret, Round Turret, defensas de Hoth...).

Al analizar el binario `orig/bin/EBOOT.BIN` y los recursos globales de `COMMON.ASR`:
1. Identificamos 117 plantillas de Blueprints correspondientes a `Tank` (21), `Turret` (48), `Flyer` (41) y `Walker` (7).
2. Los vehículos combinan estadísticas de blindaje (`Health`, `Auto_Repair_Rate`, `DeathRate`), dinámicas cinemáticas (`CruisingSpeed`, `TopSpeed`, `BoostAcc`, `MaxTurnYaw`), puntos de anclaje de armamento primario y secundario (`HardpointW1`, `HardpointW2`), y torretas hijas montadas (`Turret0` a `Turret4`).
3. En el binario Allegrex localizamos la máquina de estados de IA vehicular:
   - `VehicleAIController`: Constructor en `0x0014CC58`, vtable en `0x002F5068`, update loop en `0x0014CD68`, y descarte de sub-acciones en `0x0014CDF0`.
   - Modos de comportamiento en `0x0031773C` (11 modos: `Vehicle - UNSET` a `Vehicle - GO_TO_POSITION`).
   - Sub-acciones de navegación en `0x002D6A34` y maniobras en `0x002D6B1C`.
   - Despacho de órdenes y eventos de mando de vehículos en `0x00234A84` y `0x002D9FD8`.

## Alternativas Evaluadas
1. **Entidades hardcodeadas sin herencia:**
   - Crear clases separadas en C++ para cada vehículo individual (`class XWing`, `class ATAT`).
   - *Desventaja:* Inflexible, duplica cientos de líneas de código y no aprovecha la estructura de plantillas heredadas del motor Asura.
2. **Modelo unificado basado en Blueprints con Controlador Polimórfico:**
   - Parsear dinámicamente las 117 plantillas de `COMMON.ASR` con soporte de herencia arquetípica (`Fighters`, `Interceptors`, `Bombers`, `HoverTankBase`, `TrackTankBase`, `SpeederBikeBase`, `TurretBase`).
   - Implementar `VehicleInstance` con soporte de tripulación (`Driver`), salud/daño, autorreparación y torretas montadas.
   - Reconstruir `VehicleAIController` con correspondencia 1:1 a los 11 modos de comportamiento y sub-acciones observados en el desensamblado.
   - Integrar un despachador de eventos `VehicleEvent` para la coordinación a nivel de escuadrón/mando.

## Decisión
Se adopta la **Alternativa 2**:
- `include/systems/vehicles.h` y `src/systems/vehicles.cpp`: Implementan `VehicleManager`, `VehicleInstance`, `VehicleAIController` y las estructuras asociadas.
- Soporte para las 117 plantillas de `COMMON.ASR` con fallback integrado.
- Suite de pruebas en `tests/test_vehicles.cpp` integrada en `Makefile` bajo `make test`.

## Consecuencias
- Queda totalmente mapeada y funcional la capa de vehículos, torretas y cazas estelares del juego.
- Se logra interoperabilidad directa con el sistema de armas ya decompilado (`HardpointW1`, `HardpointW2` se vinculan con las armas de `COMMON.ASR`).
- Se provee a la IA la capacidad de tripular vehículos, cambiar de modo de vuelo/combate y gestionar torretas defensivas.
