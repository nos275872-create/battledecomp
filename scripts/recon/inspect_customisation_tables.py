#!/usr/bin/env python3
import struct

with open("orig/bin/EBOOT.BIN", "rb") as f:
    eboot = f.read()

def dump_ptrs(name, addr, count):
    print(f"=== {name} at 0x{addr:08x} ===")
    ptrs = struct.unpack(f"<{count}I", eboot[addr:addr+count*4])
    for i, p in enumerate(ptrs):
        # check if it is string or int or hash
        val_str = ""
        if 0 < p < len(eboot):
            # maybe string
            sub = eboot[p:p+32]
            if all(32 <= b < 127 or b == 0 for b in sub[:8]) and b"\x00" in sub:
                val_str = sub.split(b"\x00")[0].decode('ascii', errors='ignore')
        print(f"  [{i}] 0x{p:08x} ({p}) {val_str}")

dump_ptrs("Categories DAT_002dbc94", 0x002dbc94, 8)
dump_ptrs("Category labels UNK_002dbc74", 0x002dbc74, 8)
dump_ptrs("Category 0 DAT_002d8668", 0x002d8668, 10)
dump_ptrs("Category 1 DAT_002d8690", 0x002d8690, 8)
dump_ptrs("Category 2 DAT_002d86b0", 0x002d86b0, 6)
dump_ptrs("Category 3 DAT_002d86c8", 0x002d86c8, 8)
dump_ptrs("Category 4 DAT_002d86e8", 0x002d86e8, 6)
