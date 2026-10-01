# Sistema de Armas y Equipamiento (Weapons)

- **Qué hace en el juego:** Gestiona los tipos de armamento (blasters, lanzacohetes, granadas, sables de luz), mecánicas de disparo, cadencia, calentamiento/munición, dispersión, proyectiles y animaciones asociadas. Incluye máquinas de estado de ataque cuerpo a cuerpo para sables (`INCOMING_LIGHTSABER_THROW`, `PERFORMING_LIGHTSABER_ATTACK`).
- **Funciones y direcciones relevantes (dirección | nombre propuesto | confianza | notas):**
  - *(En proceso de identificación y mapeo tras análisis Ghidra)*
- **Estructuras de datos y tamaños:**
  - `WeaponDef` / `WeaponInstance` (por identificar en `.rodata` / `.data`).
  - Recursos vinculados: `HUD_Weapons_Prequel.asr`, `HUD_Weapons_Classic.asr`, `text/weaponnames.asr`.
- **Dependencias con otros sistemas:** Sistema de físicas/colisiones, gestión de proyectiles, HUD/UI, perfiles de jugador y bots.
- **Estado (sin empezar / en análisis / decompilado / verificado):** En análisis.
- **Dudas abiertas:** Formato binario de los archivos `.asr` para tablas de atributos de armas (si las propiedades están en código o en datos empaquetados).
