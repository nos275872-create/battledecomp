#!/usr/bin/env python3
import struct

with open("orig/bin/EBOOT.BIN", "rb") as f:
    eboot = f.read()

def va_to_file(va): return va + 0x74

for name, va in [("Health Costs", 0x002df13c), ("Speed Costs", 0x002df14c), ("Agility Costs", 0x002df15c)]:
    off = va_to_file(va)
    costs = struct.unpack("<4i", eboot[off:off+16])
    print(f"{name} at VA 0x{va:08x}: {costs}")
