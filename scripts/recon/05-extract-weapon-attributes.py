#!/usr/bin/env python3
"""
Extract weapon blueprint attributes from COMMON.ASR
"""

import struct
import sys

def read_str_aligned(data, pos):
    start = pos
    while pos < len(data) and data[pos] != 0:
        pos += 1
    s = data[start:pos].decode('utf-8', errors='replace')
    pos += 1 # skip null
    # align to 4 bytes
    padded = (pos + 3) & ~3
    return s, padded

def parse_blue_chunks(asr_path):
    with open(asr_path, "rb") as f:
        data = f.read()

    print(f"Loaded {asr_path}, size = {len(data)} bytes")
    # All BLUE chunks
    pos = 8 # skip "Asura   "
    blueprints = {}
    
    while pos + 8 <= len(data):
        fourcc = data[pos:pos+4]
        size = struct.unpack("<I", data[pos+4:pos+8])[0]
        if size == 0:
            break
        
        if fourcc == b'BLUE':
            # Chunk payload starts at pos + 8
            # In Asura engine, BLUE chunk header has 16 extra bytes or 12 bytes
            # Let's inspect pos+8 to pos+size
            chunk_data = data[pos+8:pos+size]
            # Look at header
            # u32[0]=0, u32[1]=0, u32[2]=0, u32[3]=1 (blueprint count)
            # or pos in chunk_data:
            cpos = 0
            if len(chunk_data) >= 16:
                v0, v1, v2, bp_count = struct.unpack("<4I", chunk_data[cpos:cpos+16])
                cpos += 16
                for bp_idx in range(bp_count):
                    if cpos + 8 > len(chunk_data):
                        break
                    bp_id, prop_count = struct.unpack("<II", chunk_data[cpos:cpos+8])
                    cpos += 8
                    bp_name, cpos = read_str_aligned(chunk_data, cpos)
                    props = {}
                    for prop_idx in range(prop_count):
                        if cpos + 12 > len(chunk_data):
                            break
                        ver, p_id, p_flags = struct.unpack("<III", chunk_data[cpos:cpos+12])
                        cpos += 12
                        p_name, cpos = read_str_aligned(chunk_data, cpos)
                        if cpos + 4 > len(chunk_data):
                            break
                        elem_count = struct.unpack("<I", chunk_data[cpos:cpos+4])[0]
                        cpos += 4
                        
                        elems = []
                        for elem_idx in range(elem_count):
                            if cpos + 8 > len(chunk_data):
                                break
                            e_id = struct.unpack("<I", chunk_data[cpos:cpos+4])[0]
                            cpos += 4
                            e_name, cpos = read_str_aligned(chunk_data, cpos)
                            if cpos + 4 > len(chunk_data):
                                break
                            e_u32 = struct.unpack("<I", chunk_data[cpos:cpos+4])[0]
                            cpos += 4
                            if ver > 1:
                                cpos += 4
                            val_count = 1
                            if ver > 2:
                                if cpos + 4 <= len(chunk_data):
                                    val_count = struct.unpack("<I", chunk_data[cpos:cpos+4])[0]
                                    cpos += 4
                            
                            vals = []
                            for v_idx in range(val_count):
                                if cpos + 4 > len(chunk_data):
                                    break
                                type_tag = struct.unpack("<I", chunk_data[cpos:cpos+4])[0]
                                cpos += 4
                                if type_tag in (0,):
                                    # float or int
                                    val = struct.unpack("<f", chunk_data[cpos:cpos+4])[0]
                                    ival = struct.unpack("<i", chunk_data[cpos:cpos+4])[0]
                                    cpos += 4
                                    vals.append({"tag": type_tag, "float": val, "int": ival})
                                elif type_tag == 1:
                                    val = struct.unpack("<f", chunk_data[cpos:cpos+4])[0]
                                    cpos += 4
                                    vals.append({"tag": type_tag, "float": val})
                                elif type_tag == 2:
                                    bval = chunk_data[cpos] != 0
                                    cpos += 1
                                    # 4-byte align if needed
                                    vals.append({"tag": type_tag, "bool": bval})
                                elif type_tag == 3:
                                    s_flag = struct.unpack("<I", chunk_data[cpos:cpos+4])[0]
                                    cpos += 4
                                    s_val = ""
                                    if s_flag != 0:
                                        s_val, cpos = read_str_aligned(chunk_data, cpos)
                                    vals.append({"tag": type_tag, "str": s_val})
                                elif type_tag == 4:
                                    s_flag = struct.unpack("<I", chunk_data[cpos:cpos+4])[0]
                                    cpos += 4
                                    s_val = ""
                                    if s_flag != 0:
                                        s_val, cpos = read_str_aligned(chunk_data, cpos)
                                    vals.append({"tag": type_tag, "str": s_val})
                                else:
                                    cpos += 4
                            elems.append({"id": e_id, "name": e_name, "vals": vals})
                        props[p_name] = {"id": p_id, "elems": elems}
                    blueprints[bp_name] = {"id": bp_id, "props": props}
        pos += size
    print(f"Total blueprints parsed: {len(blueprints)}")
    return blueprints

if __name__ == "__main__":
    bps = parse_blue_chunks("orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR")
    for name in sorted(bps.keys()):
        print(f"Blueprint: {name}")
