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

with open("orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR", "rb") as f:
    full_data = f.read()

size = struct.unpack("<I", full_data[0x64:0x68])[0]
chunk_data = full_data[0x68 : 0x60 + size]

stream = AsuraStream(chunk_data)
v0 = stream.read_u32()
v1 = stream.read_u32()
v2 = stream.read_u32()
bp_count = stream.read_u32()

weapons_data = {}

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
        
        elem_dict = {}
        for e_i in range(elem_count):
            e_id = stream.read_u32()
            e_name = stream.read_str()
            e_flags = stream.read_u32()
            if ver > 1:
                sub_hash = stream.read_u32()
            val_count = 1
            if ver > 2:
                val_count = stream.read_u32()
                
            vals = []
            for v_i in range(val_count):
                type_tag = stream.read_u32()
                if type_tag in (0,):
                    val = stream.read_i32()
                    vals.append(val)
                elif type_tag == 1:
                    val = stream.read_f32()
                    vals.append(val)
                elif type_tag == 2:
                    val = (stream.read_u8() != 0)
                    vals.append(val)
                elif type_tag in (3, 4):
                    s_flag = stream.read_u32()
                    s_val = ""
                    if s_flag != 0:
                        s_val = stream.read_str()
                    vals.append(s_val)
            elem_dict[e_name] = vals[0] if len(vals) == 1 else vals
        weapons_data[p_name] = elem_dict

print(f"Total weapon templates/variants in 'Weapon' blueprint: {len(weapons_data)}")
for name in sorted(weapons_data.keys()):
    if any(k in name for k in ("Pistol", "Rifle", "Blaster", "Rocket", "Sniper", "Shotgun", "Grenade", "Bowcaster", "Cutter", "Disruptor")):
        d = weapons_data[name]
        rof = d.get("RateOfFire", "N/A")
        reload_t = d.get("ReloadTime", "N/A")
        ammo = d.get("AmmoPerClip", "N/A")
        clips = d.get("MaxClips", "N/A")
        heat_shot = d.get("OverheatPershot", "N/A")
        decay = d.get("OverheatDecay", "N/A")
        proj = d.get("Projectile", "N/A")
        print(f"{name:<28} | RoF: {rof} | Reload: {reload_t} | Ammo: {ammo}/{clips} | Heat: +{heat_shot}/-{decay} | Proj: {proj}")
