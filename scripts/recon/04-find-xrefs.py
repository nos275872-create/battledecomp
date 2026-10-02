#!/usr/bin/env python3
import struct
import sys
import re

def find_mips_xrefs(elf_path, target_vaddr):
    """
    Search .text for lui + addiu / ori / lw referencing target_vaddr
    """
    with open(elf_path, "rb") as f:
        data = f.read()

    # Read .text section info from ELF
    e_shoff = struct.unpack_from("<I", data, 32)[0]
    e_shentsize = struct.unpack_from("<H", data, 46)[0]
    e_shnum = struct.unpack_from("<H", data, 48)[0]
    e_shstrndx = struct.unpack_from("<H", data, 50)[0]

    shstr_hdr = e_shoff + e_shstrndx * e_shentsize
    shstr_offset = struct.unpack_from("<I", data, shstr_hdr + 16)[0]
    shstr_size = struct.unpack_from("<I", data, shstr_hdr + 20)[0]
    shstrtab = data[shstr_offset:shstr_offset + shstr_size]

    text_sec = None
    for i in range(e_shnum):
        s_hdr = e_shoff + i * e_shentsize
        sh_name_idx = struct.unpack_from("<I", data, s_hdr)[0]
        name_end = shstrtab.find(b"\x00", sh_name_idx)
        name = shstrtab[sh_name_idx:name_end].decode("ascii", errors="replace")
        if name == ".text":
            sh_addr = struct.unpack_from("<I", data, s_hdr + 12)[0]
            sh_offset = struct.unpack_from("<I", data, s_hdr + 16)[0]
            sh_size = struct.unpack_from("<I", data, s_hdr + 20)[0]
            text_sec = (sh_addr, sh_offset, sh_size)
            break

    if not text_sec:
        print("No .text found")
        return []

    text_addr, text_offset, text_size = text_sec
    text_bytes = data[text_offset:text_offset + text_size]

    # Calculate hi and lo for target_vaddr
    lo_signed = target_vaddr & 0xFFFF
    if lo_signed >= 0x8000:
        lo_signed -= 0x10000
    hi = (target_vaddr - lo_signed) >> 16

    lo_unsigned = target_vaddr & 0xFFFF
    hi_unsigned = target_vaddr >> 16

    results = []
    # Scan instructions in 4-byte steps
    # Keep track of last lui for each register (0..31)
    lui_map = {} # reg -> (hi_val, pc)

    for i in range(0, text_size - 4, 4):
        insn = struct.unpack_from("<I", text_bytes, i)[0]
        pc = text_addr + i
        opcode = (insn >> 26) & 0x3F
        rs = (insn >> 21) & 0x1F
        rt = (insn >> 16) & 0x1F
        imm = insn & 0xFFFF

        # lui rt, imm
        if opcode == 0x0F and rs == 0:
            lui_map[rt] = (imm, pc)
            continue

        # addiu rt, rs, imm
        if opcode == 0x09:
            if rs in lui_map:
                prev_hi, lui_pc = lui_map[rs]
                imm_signed = imm if imm < 0x8000 else imm - 0x10000
                resolved = (prev_hi << 16) + imm_signed
                if resolved == target_vaddr and (pc - lui_pc) < 128:
                    results.append((pc, f"lui+addiu at 0x{pc:08X} (lui at 0x{lui_pc:08X})"))

        # ori rt, rs, imm
        if opcode == 0x0D:
            if rs in lui_map:
                prev_hi, lui_pc = lui_map[rs]
                resolved = (prev_hi << 16) | imm
                if resolved == target_vaddr and (pc - lui_pc) < 128:
                    results.append((pc, f"lui+ori at 0x{pc:08X} (lui at 0x{lui_pc:08X})"))

        # lw rt, imm(rs)
        if opcode == 0x23:
            if rs in lui_map:
                prev_hi, lui_pc = lui_map[rs]
                imm_signed = imm if imm < 0x8000 else imm - 0x10000
                resolved = (prev_hi << 16) + imm_signed
                if resolved == target_vaddr and (pc - lui_pc) < 128:
                    results.append((pc, f"lui+lw at 0x{pc:08X} (lui at 0x{lui_pc:08X})"))

    return results

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: find_xrefs.py <elf> <vaddr_hex>")
        sys.exit(1)
    target = int(sys.argv[2], 16)
    hits = find_mips_xrefs(sys.argv[1], target)
    print(f"Xrefs to 0x{target:08X}: {len(hits)} encontrados")
    for pc, desc in hits:
        print(f"  0x{pc:08X}: {desc}")
