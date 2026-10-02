# ADR-0006: Arquitectura del Sistema de Personalización de Soldados y Economía de Créditos

## Estado
Aceptado

## Fecha
2026-10-02

## Contexto
Una de las características definitorias más relevantes de *Star Wars: Battlefront: Renegade Squadron* (y exclusiva frente a entregas anteriores de la saga) es la personalización libre de soldados y clases (*Customisation*). En lugar de seleccionar clases fijas e inmutables, el jugador y los bots configuran su rol a través de un presupuesto cerrado de créditos (100 créditos por defecto).

Al realizar ingeniería inversa del binario `orig/bin/EBOOT.BIN` en MIPS Allegrex:
- Localizamos los callbacks de interfaz `FE_Customisation_*` registrados por `Customisation_InitManager` (`0x00224878`), incluyendo `FE_Customisation_OnInitMenu` (`0x0022379C`), `FE_Customisation_Credits` (`0x00223A44`), `FE_Customisation_ListCarouselNames` (`0x00224148`) y `FE_Customisation_ListCarouselCosts` (`0x0022466C`).
- Identificamos la tabla de descriptores de categorías en `0x002DBD0C` (`g_CustomisationCategoryTable`), que agrupa 8 categorías:
  1. `PrimaryWeapon` (10 opciones)
  2. `SecondaryWeapon` (8 opciones)
  3. `Explosive` (6 opciones)
  4. `Equipment` (8 opciones)
  5. `PowerUp` (6 opciones)
  6. `Health` (4 niveles)
  7. `Speed` (4 niveles)
  8. `Agility` (4 niveles)
- En `0x002304EC` (`Customisation_GetItemCost`) y `0x002305F8` (`Customisation_GetStatCost`), descubrimos que:
  - Los costes de las estadísticas físicas (`Health`, `Speed`, `Agility`) se resuelven a través de tablas estáticas en `.rodata` (`0x002DF13C`, `0x002DF14C`, `0x002DF15C`).
  - Los costes del armamento y dispositivos se leen en tiempo de ejecución extrayendo la propiedad de FourCC/Hash `0x002EAFCD` (nombre `"Cost"`) de las plantillas de Blueprint del archivo `COMMON.ASR` cargado por el motor Asura.

## Alternativas Evaluadas
1. **Modelar únicamente un enumerador de clases predefinidas:**
   - Crear clases rígidas tipo Soldado, Pesado, Francotirador e Ingeniero con armas fijas.
   - *Desventaja:* Incompatible con la arquitectura del juego, imposibilita recrear la pantalla de personalización y no refleja la asignación real de equipamiento en las plantillas de unidades del motor Asura.
2. **Hardcodear costes y omitir la consulta a Blueprints de `COMMON.ASR`:**
   - Tabular los costes en C++ sin interactuar con el sistema de Blueprints ya decompilado.
   - *Desventaja:* Desconecta la personalización del cargador de recursos del motor Rebellion Asura (`COMMON.ASR`). Si se modifica o actualiza un archivo `.asr`, los costes quedarían desfasados.
3. **Arquitectura dual integrada (Resolución Dinámica con Fallback Exacto):**
   - Definir `CustomisationManager` con capacidad de consultar el Blueprint nativo de `COMMON.ASR` mediante la propiedad `"Cost"` (`0x002EAFCD`), disponiendo a la vez de las tablas exactas de respaldo extraídas de la sección `.rodata` del ejecutable de PSP.
   - Proveer la estructura `SoldierLoadout` con cálculo dinámico de costes, créditos restantes y validación estricta de presupuesto (≤ 100 créditos).

## Decisión
Se adopta la **Alternativa 3**:
- Se implementan `include/systems/customisation.h` y `src/systems/customisation.cpp`.
- Se mapean las 8 categorías, sus 46 ítems totales y sus costes correspondientes.
- Se implementa `CustomisationManager::ResolveItemCost()` con soporte bidireccional (tablas estáticas de `.rodata` y extracción de Blueprints de `COMMON.ASR`).
- Se implementa `SoldierLoadout` para verificación de presupuestos y composición de clases.
- Se añade la suite de pruebas unitarias `tests/test_customisation.cpp` y se integra en `Makefile` bajo `make test`.

## Consecuencias
- Queda completamente reconstruido el sistema de personalización de clases e inventario de *Renegade Squadron*.
- El sistema de IA de bots (`BotAIController`) y el generador de escuadrones pueden instanciar loadouts válidos y calcular su equipamiento táctico respetando las reglas del juego.
- Se preserva la fidelidad con el binario ejecutable original de PSP y los contenedores de recursos Asura.
