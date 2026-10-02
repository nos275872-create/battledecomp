# Sistema de Embarque e Interacción Soldado-Vehículo (Boarding & Seating)

- **Qué hace en el juego:**
  Gestiona la interacción bidireccional entre la infantería (soldados humanos y droides) y las entidades motorizadas (tanques, cazas, caminantes y torretas):
  - **Asignación de Asientos:** Configura dinámicamente los puestos de tripulación según el Blueprint del vehículo:
    - `Driver` (Conductor / Piloto): controla la locomoción y las armas integradas (`HardpointW1`, `HardpointW2`).
    - `Gunner` (Artillero): opera una torreta montada específica (`Turret0` a `Turret4`), con giro independiente (yaw/pitch) y disparo.
    - `Passenger` (Pasajero): viaja en el habitáculo o bahía de tropa (transporte táctico de hasta 6 soldados en LAAT, Rebel Transport o AT-TE).
  - **Reglas de Embarque (`CanBoard`):** Valida distancia máxima de abordaje ($\le 4.0\text{ m}$), estado vital del vehículo (`IsAlive()`), bandera de abordabilidad (`Boardable`), disponibilidad de asientos y bloqueo frente a vehículos enemigos bajo control activo.
  - **Modelo de Protección y Daño (Cabina Cerrada vs Descubierta):**
    - Vehículos blindados cerrados (`Enclosed == true`): inmunidad total del soldado frente a impactos directos de bláster o metralla exterior.
    - Monturas y speeders abiertos (`Enclosed == false`): el jinete reproduce animaciones posturales (`AnimSit`, ej. `Human_STAP_sitpose`) y queda expuesto al fuego enemigo directo (metralla, rifles de francotirador y multiplicador de disparos a la cabeza).
  - **Toma de Decisiones de la IA (Embarque y Evacuación de Emergencia):**
    - Sub-acción táctica `USING_APPROACH_OBJECT` (`0x00317600`) para aproximarse a vehículos vacíos o puestos artillados.
    - Detección de daño crítico ($\le 15\%$ vida): el bot evacua inmediatamente el vehículo para no perecer en la explosión catastrófica (`DeathRate`).
  - **Siniestro Catastrófico:** Si el vehículo alcanza 0 HP, la detonación produce bajas fatales en todos los tripulantes no evacuados.

---

## Funciones y Estructuras Relevantes en el Binario

| Dirección | Nombre Propuesto | Confianza | Propósito / Notas |
|---|---|---|---|
| `0x00317600` | `g_SubAction_ApproachObject` | Alta | Tabla de sub-estados en `.data` que incluye `USING_APPROACH_OBJECT` y `LANDING`. |
| `0x003176B0` | `g_SubAction_BoardApproach` | Alta | Secuencia de aproximación y uso de objetos abordables en el terreno. |
| `0x003176E0` | `g_SubAction_HangarLanding` | Alta | Secuencia de aproximación y toma de tierra en hangares y naves capitales. |
| `0x002D5F70` | `s_USING_APPROACH_OBJECT` | Alta | Cadena descriptiva de la sub-acción en `.rodata`. |
| `0x002D9FE4` | `s_vehicle_created` | Alta | Evento de creación y puesta a punto de asientos de vehículo. |
| `0x002D9FF4` | `s_vehicle_destroyed` | Alta | Evento de destrucción vehicular y liquidación de ocupantes. |
| `0x0014CC58` | `VehicleAIController_ctor` | Alta | Enlace del controlador de locomoción al asumir el puesto de conductor. |

---

## Tipos de Asientos y Capacidades Típicas

| Arquetipo de Vehículo | Conductor | Artilleros | Pasajeros | Cabina Cerrada | Capacidad Total |
|---|---|---|---|---|---|
| **Caminante AT-TE** | 1 (Clon) | 5 (Torretas 0 a 4) | 4 | Sí | **10 plazas** |
| **Caminante AT-AT** | 1 (Stormtrooper) | 1 (Torreta principal) | 2 | Sí | **4 plazas** |
| **Tanque IFTX** | 1 (Clon) | 1 (Torreta superior) | 2 | Sí | **4 plazas** |
| **Transporte LAAT** | 1 (Piloto) | 0 | 6 (Bahía de tropa) | Sí | **7 plazas** |
| **Speeder STAP / BARC** | 1 (Jinete) | 0 | 0 | No (Expuesto) | **1 plaza** |
| **Torreta Fija (Small/Big)**| 1 (Artillero) | 0 | 0 | No (Expuesto) | **1 plaza** |

---

## Implementación C++ y Pruebas

- **Cabecera:** [`include/systems/boarding.h`](file:///home/jcgar2/battledecomp/include/systems/boarding.h)
- **Implementación:** [`src/systems/boarding.cpp`](file:///home/jcgar2/battledecomp/src/systems/boarding.cpp)
- **Pruebas Unitarias:** [`tests/test_boarding.cpp`](file:///home/jcgar2/battledecomp/tests/test_boarding.cpp)
- **Suite de Pruebas:** Ejecutable vía `make test`.
