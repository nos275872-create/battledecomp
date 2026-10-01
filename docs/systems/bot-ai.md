# Sistema de IA de Bots y Soldados (Bot AI)

- **Qué hace en el juego:** Controla el comportamiento autónomo de soldados y droides en combates individuales y multijugador local (Adhoc) e infraestructura. Maneja estados de navegación, toma de puestos de mando (Command Posts), selección de objetivos, auto-balance de jugadores/bots y niveles de dificultad (Normal, Élite).
- **Funciones y direcciones relevantes (dirección | nombre propuesto | confianza | notas):**
  - *(En proceso de identificación y mapeo tras análisis Ghidra)*
- **Estructuras de datos y tamaños:**
  - Parámetros de dificultad (`Difficulty level of the MP servers bots [0=Normal, 1=Elite]`).
  - Lógica de auto-balance de número mínimo de jugadores (`Sets the minimum number of players in a singleplayer game`).
  - Ficheros de nombres de bots por facción: `text/botnames_character.asr`, `text/botnames_cis.asr`, `text/botnames_republic.asr`, `text/botnames_alliance.asr`.
- **Dependencias con otros sistemas:** Sistema de armas, sistema de navegación/pathfinding, máquina de estados de vehículos (`Vehicle - ATTACK_TARGET`, `Vehicle - FOLLOW_SHIP`), sincronización de red.
- **Estado (sin empezar / en análisis / decompilado / verificado):** En análisis.
- **Dudas abiertas:** Algoritmo de pathfinding utilizado en el Asura Engine (navmesh, waypoints o rejilla de navegación).
