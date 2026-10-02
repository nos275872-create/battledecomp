# ADR-0003: Arquitectura y Reconstrucción del Cargador de Recursos Asura y Catálogo de Armas

## Estado
Aceptado

## Fecha
2026-10-02

## Contexto
El motor de *Star Wars: Battlefront: Renegade Squadron* es el motor propietario Asura v1.1 de Rebellion Developments. Todos los recursos del juego (textos, modelos, texturas, HUD, scripts de misiones) están encapsulados en paquetes propietarios `.asr` ("Asura   " / "AsuraCmp").

Para poder avanzar en la decompilación funcional del subsistema de armamento y lógica de juego:
1. Necesitamos entender cómo el motor carga y despacha los recursos en memoria a partir de los puntos de entrada decompilados (`0x0017603C`, `0x00023B7C`, `0x00023BEC`, `0x00024D70`, `0x00025158`).
2. Necesitamos aislar las 21 armas base del juego contenidas en `WEAPONNAMES.ASR` y cómo se relacionan con las eras de campaña (Prequel Era / Classic Era en `0x00171F04`).

## Alternativas Evaluadas

1. **Decompilación aislada estricta C sin entorno de ejecución:**
   - Decompilar código C crudo de Ghidra y dejarlo como documentación sin compilar ni validar contra los ficheros originales extraídos.
   - *Desventaja:* No permite verificar si la interpretación de estructuras binarias (como `PTXT`, padding a 4 bytes, cadenas UTF-16LE) es exacta.

2. **Reconstrucción C++17 modular con banco de pruebas nativo (Seleccionada):**
   - Reconstruir las clases centrales `AsuraArchive` y `ResourceMgr` en C++17 moderno en `src/core/` e `include/core/`, manteniendo fidelidad con las funciones originales Allegrex.
   - Reconstruir el subsistema `Weapons` en `src/systems/weapons.cpp` con catálogo tipado (`WeaponId`) y mapeo de eras (`GameEra`).
   - Crear una suite de pruebas automatizada (`tests/test_weapons_loader.cpp`) integrada en el `Makefile` (`make test`) que lee directamente los ficheros de la ISO extraída (`WEAPONNAMES.ASR`) y valida la extracción en todos los idiomas (inglés, español, francés, alemán, italiano).

## Decisión
Se implementa la alternativa 2:
- Formato `.asr` documentado: Magic 8 bytes (`"Asura   "`), encabezados de chunk FourCC (`PTXT`, `FNFO`, `RSFL`, `TTXT`, `MTRL`, etc.), tamaños de chunk Little-Endian de 32 bits, encabezado `PTXT` con ID de tabla, idioma y conteo, y cadenas codificadas en UTF-16LE.
- `ResourceMgr` maneja la carga, resolución insensible a mayúsculas/minúsculas para compatibilidad con sistemas de archivos Linux, y caché de tablas de cadenas.
- `Weapons` expone `Weapons_GetHudArchiveName` (reconstrucción directa de `0x00171F04`) y `Weapons_GetWeaponName`.

## Consecuencias
- Disponemos de una herramienta funcional y verificable para inspeccionar cualquier contenedor `.asr` de texto del juego.
- Las 21 armas base quedan catalogadas formalmente en el proyecto.
- Se sienta la base para cargar otros contenedores esenciales (`stdtext.asr`, `missionobjectives.asr`, etc.).
