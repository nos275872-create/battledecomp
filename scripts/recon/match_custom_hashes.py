#!/usr/bin/env python3
import sys, struct
sys.path.append("scripts/recon")
from inspect_projectiles import parse_chunk, AsuraStream

with open("orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR", "rb") as f:
    full_data = f.read()

# Let's read all blueprint property IDs in Weapon blueprint
size = struct.unpack("<I", full_data[0x64:0x68])[0]
chunk_data = full_data[0x68 : 0x60 + size]

stream = AsuraStream(chunk_data)
v0, v1, v2, bp_count = stream.read_u32(), stream.read_u32(), stream.read_u32(), stream.read_u32()
bp_id = stream.read_u32()
prop_count = stream.read_u32()
bp_name = stream.read_str()

hash_to_name = {}
for p_i in range(prop_count):
    ver = stream.read_u32()
    p_id = stream.read_u32()
    p_flags = stream.read_u32()
    p_name = stream.read_str()
    elem_count = stream.read_u32()
    hash_to_name[p_id] = p_name
    for e_i in range(elem_count):
        e_id = stream.read_u32()
        e_name = stream.read_str()
        e_flags = stream.read_u32()
        if ver > 1: stream.read_u32()
        val_count = stream.read_u32() if ver > 2 else 1
        for v_i in range(val_count):
            t = stream.read_u32()
            if t == 0: stream.read_i32()
            elif t == 1: stream.read_f32()
            elif t == 2: stream.read_u8()
            elif t in (3, 4):
                f = stream.read_u32()
                if f != 0: stream.read_str()

print(f"Total Weapon template hashes: {len(hash_to_name)}")

from inspect_cat_items import cats
import inspect_cat_items

# Check each item hash in our category tables
with open("orig/bin/EBOOT.BIN", "rb") as f:
    eboot = f.read()

for name, va, count in cats:
    f_off = va + 0x74
    items = struct.unpack(f"<{count}I", eboot[f_off:f_off+count*4])
    print(f"\n=== {name} ===")
    for i, it in enumerate(items):
        mapped = hash_to_name.get(it, "NOT_FOUND")
        print(f"  Item {i}: 0x{it:08x} -> {mapped}")
