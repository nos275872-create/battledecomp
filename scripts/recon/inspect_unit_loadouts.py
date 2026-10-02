#!/usr/bin/env python3
import sys, struct
sys.path.append("scripts/recon")
from inspect_projectiles import parse_chunk, AsuraStream

with open("orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR", "rb") as f:
    full_data = f.read()

pos = 8
while pos + 8 <= len(full_data):
    fourcc = full_data[pos:pos+4]
    size = struct.unpack("<I", full_data[pos+4:pos+8])[0]
    if fourcc == b"BLUE":
        chunk_data = full_data[pos+8 : pos+size]
        stream = AsuraStream(chunk_data)
        v0, v1, v2, bp_count = stream.read_u32(), stream.read_u32(), stream.read_u32(), stream.read_u32()
        bp_id, prop_count = stream.read_u32(), stream.read_u32()
        bp_name = stream.read_str()
        if bp_name == "Humanoid":
            d = parse_chunk(chunk_data)
            for unit in ("ALL_Unit", "EMP_Unit", "REP_Unit", "CIS_Unit", "Clonetrooper", "Stormtrooper"):
                if unit in d:
                    print(f"=== {unit} ===")
                    for k, v in d[unit].items():
                        print(f"  {k} = {v}")
    pos += size
