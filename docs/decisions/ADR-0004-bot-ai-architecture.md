# ADR-0004: Arquitectura del Controlador de IA de Bots, Modos de Comportamiento y Gestión de Amenazas

## Estado
Aceptado

## Fecha
2026-10-02

## Contexto
En *Star Wars: Battlefront: Renegade Squadron*, los soldados autónomos (bots de la República, CIS, Imperio y Rebelión) son gestionados por la clase `BotAIController` (vtable `0x002F6198`).
Al decompilar `0x00188A44` (constructor), `0x00188D58` (Update), `0x00188EF4` (despachador de telemetría y nombres de órdenes), `0x00188DB8` (filtro de amenazas) y `0x00189854` (eliminación y corrimiento de amenazas), descubrimos:
1. El comportamiento de alto nivel del bot se rige por un valor entero en el offset `+0x4C` con 9 modos documentados: `Hunter Seeker` (1008), `Attack : CP Variant` (1010 - por defecto), `Attack : CTF Variant` (1011), `Attack : CTF Variant-b` (1012), `Defend : Command Post` (1013), `Standard Attack` (1014), `Standard Defend` (1015), `Withdrawl` (1016) y `Hero` (1017).
2. Cada bot mantiene una lista acotada de hasta 5 amenazas simultáneas (`+0x98`), con un valor especial centinela `999` para slots vacíos o sin objetivo (`+0x3C` y `+0x98`).
3. El seguimiento de objetivos actualiza vector tridimensional de posición en `+0x58`, `+0x5C`, `+0x60` y ángulo de orientación en `+0x54`.

## Alternativas Evaluadas
1. **Representación abstracta sin coincidencia binaria:**
   - Crear una máquina de estados genérica desvinculada de los offsets de memoria del binario de PSP.
   - *Desventaja:* Dificulta el eventual matching de ensamblado de Allegrex y confunde la trazabilidad de los campos `+0x4C`, `+0x98` y `+0xAC`.
2. **Reconstrucción fiel C++17 orientada a objetos (Seleccionada):**
   - Modelar `BotAIController` reflejando los campos, capacidades y algoritmos del motor original (como el algoritmo de eliminación por corrimiento de `0x00189854` y filtro por salud de `0x00188DB8`).
   - Crear suite de pruebas unitarias (`tests/test_bot_ai.cpp`) que certifique el comportamiento contra los valores exactos encontrados en `EBOOT.BIN`.

## Decisión
Se implementa la alternativa 2 en `include/ai/bot_ai.h` y `src/ai/bot_ai.cpp`.

## Consecuencias
- La lógica de toma de decisiones de los bots queda aislada y comprobada de forma determinista.
- Queda listo el camino para conectar la selección de armas en combate con el subsistema de armas implementado en `src/systems/weapons.cpp`.
