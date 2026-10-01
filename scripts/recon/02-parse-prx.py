#!/usr/bin/env python3
import struct
import sys
import os

def parse_prx(elf_path, out_file):
    with open(elf_path, "rb") as f:
        data = f.read()

    # ELF Header
    magic = data[:4]
    if magic != b"\x7fELF":
        print(f"Error: Not an ELF file: {magic}")
        return

    # Section Headers
    e_shoff = struct.unpack_from("<I", data, 32)[0]
    e_shentsize = struct.unpack_from("<H", data, 46)[0]
    e_shnum = struct.unpack_from("<H", data, 48)[0]
    e_shstrndx = struct.unpack_from("<H", data, 50)[0]

    # Read section name string table
    shstr_hdr = e_shoff + e_shstrndx * e_shentsize
    shstr_offset = struct.unpack_from("<I", data, shstr_hdr + 16)[0]
    shstr_size = struct.unpack_from("<I", data, shstr_hdr + 20)[0]
    shstrtab = data[shstr_offset:shstr_offset + shstr_size]

    sections = {}
    for i in range(e_shnum):
        s_hdr = e_shoff + i * e_shentsize
        sh_name_idx = struct.unpack_from("<I", data, s_hdr)[0]
        sh_type = struct.unpack_from("<I", data, s_hdr + 4)[0]
        sh_flags = struct.unpack_from("<I", data, s_hdr + 8)[0]
        sh_addr = struct.unpack_from("<I", data, s_hdr + 12)[0]
        sh_offset = struct.unpack_from("<I", data, s_hdr + 16)[0]
        sh_size = struct.unpack_from("<I", data, s_hdr + 20)[0]

        name_end = shstrtab.find(b"\x00", sh_name_idx)
        name = shstrtab[sh_name_idx:name_end].decode("ascii", errors="replace")
        sections[name] = {
            "index": i,
            "type": sh_type,
            "flags": sh_flags,
            "addr": sh_addr,
            "offset": sh_offset,
            "size": sh_size,
        }

    with open(out_file, "w") as out:
        out.write("# Análisis de Módulos e Importaciones PRX\n\n")
        
        # Module Info
        if ".rodata.sceModuleInfo" in sections:
            s = sections[".rodata.sceModuleInfo"]
            mod_data = data[s["offset"]:s["offset"] + s["size"]]
            mod_attr, mod_ver = struct.unpack_from("<HH", mod_data, 0)
            mod_name = mod_data[4:32].split(b"\x00")[0].decode("ascii", errors="replace")
            gp = struct.unpack_from("<I", mod_data, 32)[0]
            lib_ent = struct.unpack_from("<I", mod_data, 36)[0]
            lib_ent_end = struct.unpack_from("<I", mod_data, 40)[0]
            lib_stub = struct.unpack_from("<I", mod_data, 44)[0]
            lib_stub_end = struct.unpack_from("<I", mod_data, 48)[0]

            out.write("## 1. Información del Módulo (sceModuleInfo)\n")
            out.write(f"- **Nombre del módulo:** `{mod_name}`\n")
            out.write(f"- **Atributos:** `0x{mod_attr:04X}`\n")
            out.write(f"- **Versión:** `{mod_ver >> 8}.{mod_ver & 0xFF}`\n")
            out.write(f"- **GP Register:** `0x{gp:08X}`\n")
            out.write(f"- **Entradas (Exportaciones):** `0x{lib_ent:08X}` .. `0x{lib_ent_end:08X}`\n")
            out.write(f"- **Stubs (Importaciones):** `0x{lib_stub:08X}` .. `0x{lib_stub_end:08X}`\n\n")

        # Parse Stubs (.lib.stub)
        if ".lib.stub" in sections:
            s_stub = sections[".lib.stub"]
            stub_bytes = data[s_stub["offset"]:s_stub["offset"] + s_stub["size"]]
            # Each PspLibStubEntry is 20 (0x14) or 28 (0x1C) bytes in PSP SDK
            # struct PspLibStubEntry:
            #   char *name (4 bytes)
            #   u16 version (2 bytes)
            #   u16 flags (2 bytes)
            #   u8 size (1 byte)
            #   u8 numVars (1 byte)
            #   u16 numFuncs (2 bytes)
            #   u32 *nidTable (4 bytes)
            #   void *stubTable (4 bytes)
            out.write("## 2. Bibliotecas Importadas y NIDs\n\n")
            out.write("| Biblioteca | Versión | Funciones | Variables | Dirección Stubs | Tabla NIDs |\n")
            out.write("|------------|---------|-----------|-----------|-----------------|------------|\n")

            offset = 0
            stub_count = 0
            while offset + 20 <= len(stub_bytes):
                mod_name_ptr, ver, flags, size_val, num_vars, num_funcs, nid_tbl, stub_tbl = struct.unpack_from("<IHHBBHII", stub_bytes, offset)
                if size_val == 0:
                    size_val = 5
                
                # Resolve module name from virtual address
                # Convert mod_name_ptr to file offset
                lib_name = f"ptr_0x{mod_name_ptr:08X}"
                for s_name, sec in sections.items():
                    if sec["addr"] <= mod_name_ptr < sec["addr"] + sec["size"]:
                        name_off = sec["offset"] + (mod_name_ptr - sec["addr"])
                        end_zero = data.find(b"\x00", name_off)
                        lib_name = data[name_off:end_zero].decode("ascii", errors="replace")
                        break

                out.write(f"| `{lib_name}` | `0x{ver:04X}` | {num_funcs} | {num_vars} | `0x{stub_tbl:08X}` | `0x{nid_tbl:08X}` |\n")
                stub_count += 1
                offset += size_val * 4

            out.write(f"\n*Total de bibliotecas importadas: {stub_count}*\n\n")

        # Section Summary
        out.write("## 3. Resumen de Secciones Principales\n\n")
        out.write("| Sección | Dirección Virt | Tamaño | Tipo | Flags |\n")
        out.write("|---------|----------------|--------|------|-------|\n")
        for s_name, sec in sections.items():
            if sec["size"] > 0 and not s_name.startswith(".rel."):
                flags_str = ""
                if sec["flags"] & 1: flags_str += "W"
                if sec["flags"] & 2: flags_str += "A"
                if sec["flags"] & 4: flags_str += "X"
                out.write(f"| `{s_name}` | `0x{sec['addr']:08X}` | {sec['size']} B (`0x{sec['size']:X}`) | {sec['type']} | `{flags_str}` |\n")

    print(f"Análisis guardado en {out_file}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: parse_prx.py <elf> <output_md>")
        sys.exit(1)
    parse_prx(sys.argv[1], sys.argv[2])
