# Sistema de Menús e Interfaz de Usuario (Menus & UI)

- **Qué hace en el juego:** Gestiona el flujo de pantallas principales, selección de perfil de jugador, configuración de controles (eje Y invertido para vehículos), salas de espera multijugador, diálogos del sistema PSP (`sceUtility`) y renderizado del HUD en pantalla con la GPU de la PSP (`sceGe_user`).
- **Funciones y direcciones relevantes (dirección | nombre propuesto | confianza | notas):**
  - *(En proceso de identificación y mapeo tras análisis Ghidra)*
- **Estructuras de datos y tamaños:**
  - Estructuras de configuración del perfil del jugador (`Player Profile - Is Y-Axis inverted for Vehicle controls`).
  - Diálogos SCE: `sceUtilityMsgDialogInitStart`, `sceUtilitySavedataInitStart`.
- **Dependencias con otros sistemas:** `sceCtrl` (lectura de botones), `sceGe_user` (listas de comandos del procesador gráfico), motor de audio (efectos de menú).
- **Estado (sin empezar / en análisis / decompilado / verificado):** En análisis.
- **Dudas abiertas:** Cómo se renderizan los menús (si usan fuentes vectoriales, mapa de bits en texturas o listas de renderizado 2D directas de PSP GE).
