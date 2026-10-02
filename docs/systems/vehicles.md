# Sistema de Vehículos y Torretas (Vehicles & Turrets)

- **Qué hace en el juego:**
  Gestiona todas las entidades vehiculares terrestres, aéreas y emplazamientos defensivos del juego:
  - Tanques repulsores y de orugas (`Tank`: AAT, IFTX, T4B, BARC, STAP...).
  - Torretas independientes y defensas fijas (`Turret`: Dish Turret, Round Turret, Auto-Turret...).
  - Cazas estelares, interceptores y bombarderos (`Flyer`: X-Wing, TIE Fighter, A-Wing, Millennium Falcon...).
  - Caminantes de asalto pesado (`Walker`: AT-AT, AT-TE, AT-ST, AT-RT, Spider Walker).
  Controla los atributos de blindaje y vida (`Health`), autoreparación pasiva (`Auto_Repair_Rate`), puntos de anclaje de armamento (`HardpointW1`, `HardpointW2`), torretas secundarias artilladas (`Turret0` a `Turret4`), tripulación y conductor (`Driver`, `Seat`, `Boardable`), y la máquina de estados de comportamiento para la IA vehicular (`VehicleAIController`).

---

## Funciones y Direcciones Relevantes

| Dirección | Nombre Propuesto | Confianza | Propósito / Notas |
|---|---|---|---|
| `0x0014CC58` | `VehicleAIController::VehicleAIController` | Alta | Constructor del controlador de IA vehicular. Inicializa la vtable `0x002F5068`, modo a `0` (`Unset`) y estado a `0`. |
| `0x0014CD68` | `VehicleAIController::Update` | Alta | Bucle principal de actualización: evalúa la cola de amenazas, transiciona entre modos y maneja la orientación hacia el objetivo. |
| `0x0014CDF0` | `VehicleAIController::ClearSubAction` | Alta | Aborta y resetea las sub-acciones activas de navegación y maniobra llamando al slot de vtable `+0x30` (`0x00290930`). |
| `0x0014CE38` | `VehicleAIController::GetTelemetryString` | Alta | Resuelve y formatea en pantalla la cadena descriptiva de depuración del modo actual indexando `0x0031773C`. |
| `0x001513F0` | `AIController::AIController` | Alta | Constructor base de controladores de IA del motor Asura. |
| `0x00234A28` | `Vehicle_ResetOrders` | Alta | Limpia y resetea los flags y registros de órdenes del vehículo. |
| `0x00234A84` | `Vehicle_OrderDispatcher` | Alta | Despachador de órdenes tácticas y eventos para vehículos en el campo de batalla. |
| `0x002F5068` | `vtable_VehicleAIController` | Alta | Tabla de métodos virtuales de `VehicleAIController` (13 slots mapeados). |
| `0x0031773C` | `g_VehicleBehaviorModeStrings` | Alta | Puntero a la tabla de 11 cadenas de modos de comportamiento en `.data`. |
| `0x002D6A34` | `g_VehicleNavSubactionStrings` | Alta | Cadenas de sub-acciones de navegación (`MOVING_TO_POSITION`, `POINTING_AT_THREAT`...). |
| `0x002D6B1C` | `g_VehicleManeuverSubactionStrings` | Alta | Cadenas de sub-acciones de maniobra (`MOVE_ABOUT`, `ATTACK`, `AVOID_COLLISION`...). |
| `0x002D9FD8` | `g_VehicleEventStrings` | Alta | Cadenas de eventos vehiculares (`vehicle created`, `vehicle destroyed`...). |

---

## Catálogo de Blueprints (`COMMON.ASR`)

El sistema de Blueprints define 117 plantillas divididas en 4 categorías:

### 1. Caminantes (`Walker`) — 7 Plantillas
| Plantilla | Vida Máx | Tamaño | Conductor | Armamento Integrado | Torretas Montadas |
|---|---|---|---|---|---|
| `AT_AT` | 3500 hp | Very Large | Stormtrooper | N/A | `Turret0`: `AT_AT_turret_01` |
| `AT_TE` | 2500 hp | Very Large | Clonetrooper | N/A | 5 Torretas (`AT_TE_turret_01` a `05`) |
| `Spider_walker` | 2500 hp | Large | battle_droid | `TankLasers_Spider` | `Turret0`: `spider_walker_turret_01` |
| `AT_ST` | 1000 hp | Medium | Stormtrooper | `Particle_Beam_ATST` | `Turret0`: `AT_ST_turret_02` |
| `AT_RT` | 400 hp | Small | Clonetrooper | `TankLasers_Medium`, `MortarLauncher_ATRT` | N/A (1 tripulante abierto) |

### 2. Tanques y Speederbikes (`Tank`) — 21 Plantillas
| Plantilla | Arquetipo Base | Vida Máx | Velocidad Máx | Armamento Integrado | Torretas |
|---|---|---|---|---|---|
| `IFTX_tank` | `HoverTankBase` | 800 hp | 25.0 m/s | `TankLasers_IFT`, `Concussion_Missile` | `IFTX_tank_turret_01_Base` |
| `IFTT_tank` | `HoverTankBase` | 800 hp | 25.0 m/s | `TankLasers_IFT`, `Concussion_Missile` | `IFTT_tank_turret_01_Base` |
| `AAT_Tank` | `TrackTankBase` | 1200 hp | 25.0 m/s | `TankLasers_AAT`, `Concussion_Missile` | `AAT_tank_turret_01` |
| `T4B_tank` | `TrackTankBase` | 1200 hp | 25.0 m/s | `TankLasers_T4B` | `T4B_tank_turret_01`, `02` |
| `A5_RX_Juggernaut` | `TrackTankBase` | 1200 hp | 25.0 m/s | `TankLasers_A5R` | 4 Torretas (`turret_01` a `04`) |
| `AAC1_speeder` | `HoverTankBase` | 800 hp | 25.0 m/s | `TankLasers_AAC1`, `Particle_Beam_Vehicle`| `AAC1_Speeder_turret_01` |
| `BARC_speeder` | `SpeederBikeBase` | 200 hp | 40.0 m/s | N/A | N/A |
| `STAP` | `SpeederBikeBase` | 200 hp | 40.0 m/s | `TankLasers_Speederbike_STAP` | N/A |

### 3. Cazas y Naves (`Flyer`) — 41 Plantillas
| Plantilla / Clase | Arquetipo Base | Vida Máx | Vel. Crucero / Top / Boost | Armamento Integrado |
|---|---|---|---|---|
| `X_Wing` | `Fighters` | 250 hp | 100 / 175 / 150 m/s | `SpaceLasers_X_Wing`, `Proton Torpedo Launcher` |
| `Tie_Fighter` | `Fighters` | 250 hp | 100 / 175 / 150 m/s | `SpaceLasers_Fighters`, `Proton Torpedo Launcher` |
| `A_Wing` | `Interceptors` | 200 hp | 125 / 225 / 175 m/s | `SpaceLasers_A_Wing`, `Homing Missiles` |
| `Tie_Interceptor` | `Interceptors` | 200 hp | 125 / 225 / 175 m/s | `SpaceLasers_TIE_Interceptor`, `Homing Missiles` |
| `Y_Wing` | `Bombers` | 400 hp | 75 / 125 / 50 m/s | `SpaceLasers_Y_Wing`, `Proton_Bomb_Launcher` |
| `Tie_Bomber` | `Bombers` | 400 hp | 75 / 125 / 50 m/s | `SpaceLasers_TIE_Bomber`, `Proton_Bomb_Launcher` |
| `Millennium_Falcon` | `Hero_Flyers` | 600 hp | 150 / 225 / 225 m/s | `SpaceLasers_MillenniumFalcon`, `Proton Torpedo Launcher` |
| `Slave_1` | `Hero_Flyers` | 600 hp | 150 / 225 / 225 m/s | `SpaceLasers_Fighters`, `Proton Torpedo Launcher` |

### 4. Torretas Defensivas (`Turret`) — 48 Plantillas
| Plantilla | Vida | Autoreparación | Abordable | Armamento Integrado |
|---|---|---|---|---|
| `TurretBase` | 200 hp | 5.0 hp/s | Sí | `TankLasers_Heavy` |
| `Dish_Turret` | 300 hp | N/A | Sí | `Particle_Beam_Turret` |
| `Round_turret` | 600 hp | N/A | Sí | `TurretLasers_Heavy` |
| `Small_Turret` | 300 hp | N/A | Sí | `TurretLasers_Medium` |
| `Big_Turret` | 500 hp | N/A | Sí | `Turret_Concussion_Missile` |
| `Hoth_Turret_Defence` | 500 hp | N/A | Sí | `TurretLasers_Medium` |
| `Auto_Turret` | 100 hp | N/A | No | `TurretLasers_Light` |
| `auto_turret_droid` | 50 hp | N/A | No | `TurretLasers_Light` |

---

## Máquina de Estados de la IA Vehicular (`VehicleAIController`)

### Modos de Comportamiento (`0x0031773C`)
```
[0] Vehicle - UNSET
[1] Vehicle - NONE
[2] Vehicle - TAKE_OFF
[3] Vehicle - GENERAL_COMBAT
[4] Vehicle - ATTACK_SHIP
[5] Vehicle - DEFEND_SHIP
[6] Vehicle - FOLLOW_SHIP
[7] Vehicle - CAPTURE_FLAG
[8] Vehicle - ATTACK_TARGET
[9] Vehicle - LAND_IN_HANGAR
[10] Vehicle - GO_TO_POSITION
```

### Sub-acciones de Navegación y Maniobra
- **Navegación:** `MOVING_TO_POSITION`, `ORIENTATING_TO_THREAT`, `ORIENTATING_TO_THREAT_NO_ROUTE`, `POINTING_AT_THREAT`, `POINTING_AT_THREAT_NO_ROUTE`, `FINISHED`.
- **Maniobra:** `MOVE_ABOUT`, `ATTACK`, `AVOID_COLLISION`, `FINISHED`.

### Eventos de Mando Vehicular (`0x002D9FD8`)
- `no event`, `vehicle created`, `vehicle destroyed`, `flag created`, `flag picked up`, `approach object created`, `approach object destroyed`, `approach object team changed`, `vehicle orders completed`, `vehicle orders aborted`.

---

## Implementación C++ y Pruebas

- **Cabecera:** [`include/systems/vehicles.h`](file:///home/jcgar2/battledecomp/include/systems/vehicles.h)
- **Implementación:** [`src/systems/vehicles.cpp`](file:///home/jcgar2/battledecomp/src/systems/vehicles.cpp)
- **Pruebas:** [`tests/test_vehicles.cpp`](file:///home/jcgar2/battledecomp/tests/test_vehicles.cpp)
- **Suite completa:** Ejecutable vía `make test`.
