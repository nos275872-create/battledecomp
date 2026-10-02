# ADR-0008: Arquitectura del Sistema de Embarque e Interacción Soldado-Vehículo

## Estado
Aceptado

## Fecha
2026-10-02

## Contexto
En *Star Wars: Battlefront: Renegade Squadron*, la transición fluida entre el combate a pie como infantería y el combate mecanizado en vehículos terrestres, torretas o cazas estelares es fundamental para el bucle de juego.
Para dar soporte tanto al jugador como a los bots controlados por IA:
1. Las entidades vehiculares deben exponer asientos dinámicos acordes a sus plantillas de Blueprints en `COMMON.ASR`:
   - Conductor/Piloto (`Driver`), Artilleros de torretas (`Gunner`), y Pasajeros (`Passenger`).
2. Se debe modelar con precisión la protección balística:
   - Cabinas blindadas cerradas (`Enclosed == true`): protección del 100% de la infantería interior.
   - Puestos abiertos (`Enclosed == false`): exposición a disparos a la cabeza y metralla, con animación postural (`AnimSit`).
3. En el desensamblado de MIPS Allegrex en `EBOOT.BIN`:
   - La IA de los bots utiliza las sub-acciones tácticas de `USING_APPROACH_OBJECT` (`0x00317600`, `0x003176B0`) para aproximarse al vehículo e interactuar con él.
   - Se debe disponer de lógica de evacuación automática en caso de daño crítico del vehículo ($\le 15\%$ vida) para evitar que los bots perezcan en la explosión de destrucción (`0x002D9FF4`).

## Alternativas Evaluadas
1. **Delegar el estado de embarque exclusivamente a un flag booleano en el soldado:**
   - Añadir únicamente `bool inVehicle` y un puntero de vehículo en la entidad soldado.
   - *Desventaja:* No permite gestionar puestos múltiples (artilleros de torretas vs pasajeros), no soporta vehículos multitripulados (como el AT-TE con 10 plazas o el LAAT con 7) y dificulta el cálculo de daño diferencial y destrucción catastrófica.
2. **Sistema Centralizado de Asientos y Abordaje (`BoardingManager`):**
   - Registrar una topología de asientos para cada vehículo en función de sus armas y torretas montadas.
   - Gestionar el ciclo de vida del abordaje (`CanBoard`, `BoardVehicle`, `ExitVehicle`), control de facciones enemigas y cálculo de daño según el tipo de cabina.
   - Proveer heurísticas de decisión para la IA de bots (`EvaluateBotBoarding`, `EvaluateBotEgress`).

## Decisión
Se adopta la **Alternativa 2**:
- Implementada en `include/systems/boarding.h` y `src/systems/boarding.cpp`.
- Integrada con `VehicleManager` y `BotAIController`.
- Validada exhaustivamente en `tests/test_boarding.cpp` con 100% de aserciones en `make test`.

## Consecuencias
- Queda totalmente completada la interoperabilidad entre infantería, vehículos, torretas secundarias y sistemas de armas.
- Se reproducen fielmente las reglas de combate del juego original de PSP (inmunidad en cabinas cerradas, exposición en speeders abiertos, y límites de plazas).
