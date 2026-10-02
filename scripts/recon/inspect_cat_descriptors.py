#!/usr/bin/env python3
import struct

with open("orig/bin/EBOOT.BIN", "rb") as f:
    eboot = f.read()

def va_to_file(va):
    # For .text / .rodata / .data:
    # Notice .text VA 0x40 -> file 0xb4 (+0x74)
    # .rodata VA 0x2cf800 -> file 0x2cf874 (+0x74)
    return va + 0x74

addr = va_to_file(0x002dbd04)
print(f"=== Category Descriptors at VA 0x002dbd04 (File 0x{addr:08x}) ===")
for cat_i in range(8):
    cat_addr = addr + cat_i * 0xc
    w0, count, w2 = struct.unpack("<III", eboot[cat_addr:cat_addr+12])
    print(f"Category [{cat_i}]: w0=0x{w0:08x}, count={count}, w2=0x{w2:08x}")
    if w0 != 0:
        w0_file = va_to_file(w0)
        if w0_file < len(eboot):
            items = struct.unpack(f"<{min(count, 30)}I", eboot[w0_file:w0_file+min(count, 30)*4])
            print(f"  Item IDs / Hashes ({len(items)}): {[hex(x) for x in items]}")
