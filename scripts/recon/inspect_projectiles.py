#!/usr/bin/env python3
import struct

class AsuraStream:
    def __init__(self, data):
        self.data = data
        self.pos = 0

    def read(self, n):
        res = self.data[self.pos : self.pos + n]
        self.pos += n
        return res

    def read_u32(self):
        return struct.unpack("<I", self.read(4))[0]

    def read_i32(self):
        return struct.unpack("<i", self.read(4))[0]

    def read_f32(self):
        return struct.unpack("<f", self.read(4))[0]

    def read_u8(self):
        return self.read(1)[0]

    def read_str(self):
        chars = bytearray()
        found_null = False
        while not found_null:
            block = self.read(4)
            if not block:
                break
            for b in block:
                if b == 0:
                    found_null = True
                    break
                chars.append(b)
        return chars.decode('utf-8', errors='replace')

def parse_chunk(chunk_data):
    stream = AsuraStream(chunk_data)
    v0, v1, v2, bp_count = stream.read_u32(), stream.read_u32(), stream.read_u32(), stream.read_u32()
    props_dict = {}
    for bp_i in range(bp_count):
        bp_id = stream.read_u32()
        prop_count = stream.read_u32()
        bp_name = stream.read_str()
        for p_i in range(prop_count):
            ver = stream.read_u32()
            p_id = stream.read_u32()
            p_flags = stream.read_u32()
            p_name = stream.read_str()
            elem_count = stream.read_u32()
            elems = {}
            for e_i in range(elem_count):
                e_id = stream.read_u32()
                e_name = stream.read_str()
                e_flags = stream.read_u32()
                if ver > 1: stream.read_u32()
                val_count = 1
                if ver > 2: val_count = stream.read_u32()
                vals = []
                for v_i in range(val_count):
                    t = stream.read_u32()
                    if t == 0: vals.append(stream.read_i32())
                    elif t == 1: vals.append(stream.read_f32())
                    elif t == 2: vals.append(stream.read_u8() != 0)
                    elif t in (3, 4):
                        flag = stream.read_u32()
                        s = stream.read_str() if flag != 0 else ""
                        vals.append(s)
                elems[e_name] = vals[0] if len(vals) == 1 else vals
            props_dict[p_name] = elems
    return props_dict

with open("orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR", "rb") as f:
    full_data = f.read()

pos = 8
while pos + 8 <= len(full_data):
    fourcc = full_data[pos:pos+4]
    size = struct.unpack("<I", full_data[pos+4:pos+8])[0]
    if fourcc == b'BLUE':
        chunk_data = full_data[pos+8 : pos+size]
        stream = AsuraStream(chunk_data)
        v0, v1, v2, bp_count = stream.read_u32(), stream.read_u32(), stream.read_u32(), stream.read_u32()
        bp_id = stream.read_u32()
        prop_count = stream.read_u32()
        bp_name = stream.read_str()
        if bp_name in ("Projectile", "LaserBolt"):
            p_dict = parse_chunk(chunk_data)
            print(f"\n=== BLUEPRINT: {bp_name} ({len(p_dict)} templates) ===")
            for t_name in sorted(p_dict.keys())[:15]:
                d = p_dict[t_name]
                dmg = d.get("Damage", d.get("damage", "N/A"))
                spd = d.get("Speed", d.get("speed", d.get("InitialSpeed", "N/A")))
                grav = d.get("Gravity", d.get("gravity", "N/A"))
                life = d.get("LifeTime", d.get("lifetime", "N/A"))
                print(f"  {t_name:<25} | Dmg: {dmg} | Spd: {spd} | Grav: {grav} | Life: {life}")
    pos += size
