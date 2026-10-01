# ADR-0001: Estrategia de Descifrado de EBOOT.BIN de PSP

## Estado
Aceptado

## Contexto
El ejecutable principal del UMD oficial de *Star Wars: Battlefront: Renegade Squadron* (`PSP_GAME/SYSDIR/EBOOT.BIN`) se distribuye firmado y cifrado mediante el criptoprocesador hardware Kirk de la PSP (cabecera `~PSP`, etiqueta SCE `C0CB167C`). Para proceder con la ingeniería inversa, análisis estático y decompilación, es indispensable obtener el archivo ELF sin cifrar.

## Alternativas Consideradas
1. **Emulador PPSSPP con dump de memoria:** Configurar PPSSPP en modo desarrollador para volcar el EBOOT descifrado en RAM al arrancar el juego.
   - *Desventaja:* Requiere entorno gráfico o configuración de emulador interactivo, no fácilmente automatizable en pipelines sin interfaz.
2. **Volcado directo desde hardware real (PSP con Custom Firmware):** Utilizar homebrew como `PrxDecrypter` en una consola física.
   - *Desventaja:* Dependencia de hardware físico externo manual.
3. **Uso de BOOT.BIN del disco UMD:** Muchos juegos de PSP incluyen `PSP_GAME/SYSDIR/BOOT.BIN` sin cifrar para pruebas internas.
   - *Desventaja:* Algunos juegos sustituyen `BOOT.BIN` por un binario ficticio (*dummy*) o una versión obsoleta que difiere de la versión comercial final.
4. **Compilación de herramienta nativa `pspdecrypt` (John-K):** Utilidad en C++ con emulación de claves Kirk/AES/DES/SHA que procesa el contenedor `~PSP` de forma directa y repetible desde CLI.
   - *Ventaja:* Totalmente reproducible, integrable en scripts sin dependencias gráficas.

## Decisión
Se compila e instala localmente `pspdecrypt` en `tools/bin/pspdecrypt` mediante el script `scripts/setup/02-install-pspdecrypt.sh`. Se descifra `orig/bin/EBOOT.BIN.enc` generando `orig/bin/EBOOT.BIN.dec`.

Adicionalmente, se realizó una verificación cruzada calculando el hash SHA-256 de `BOOT.BIN` y `EBOOT.BIN.dec`:
- Ambos binarios son **100% idénticos** (`7e0938f26c677c49a20fff92dce3d5f9c9a553ca1d3885683ea0adf787184deb`).

## Consecuencias
- Disponemos del binario ELF32 Little-Endian original y verificado en `orig/bin/EBOOT.BIN`.
- El proceso es completamente desatendido, determinista y auditable en `scripts/setup/03-decrypt-eboot.sh`.
