#!/usr/bin/env python3
import struct

with open("orig/bin/EBOOT.BIN", "rb") as f:
    eboot = f.read()

# Let's search for functions in 0x00223000 to 0x00225000:
# We know func_00223a44, func_00223af4, func_00223b6c, func_00223bf4, func_00223d10, func_00223f3c are there.
# Let's grep functions.tsv for 00223
with open("ghidra/exports/functions.tsv") as f:
    for line in f:
        if "00223" in line or "00224" in line:
            print(line.strip())
