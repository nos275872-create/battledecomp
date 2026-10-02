#!/usr/bin/env python3
import struct

with open("orig/bin/EBOOT.BIN", "rb") as f:
    eboot = f.read()

def va_to_file(va):
    return va + 0x74

cats = [
    ("Category 0 (Primary Weapon)", 0x002d8668, 10),
    ("Category 1 (Secondary Weapon)", 0x002d8690, 8),
    ("Category 2 (Special / Explosives)", 0x002d86b0, 6),
    ("Category 3 (Equipment / Packs)", 0x002d86c8, 8),
    ("Category 4 (PowerUp / Trait)", 0x002d86e8, 6),
]

for name, va, count in cats:
    f_off = va_to_file(va)
    items = struct.unpack(f"<{count}i", eboot[f_off:f_off+count*4])
    print(f"\n=== {name} at VA 0x{va:08x} ===")
    for i, it in enumerate(items):
        print(f"  Item {i}: int={it} (0x{it & 0xffffffff:08x})")
