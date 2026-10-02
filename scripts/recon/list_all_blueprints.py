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

pos = 8
chunk_idx = 0
while pos + 8 <= len(full_data):
    fourcc = full_data[pos:pos+4]
    size = struct.unpack("<I", full_data[pos+4:pos+8])[0]
    if size == 0: break
    if fourcc == b'BLUE':
        chunk_data = full_data[pos+8 : pos+size]
        stream = AsuraStream(chunk_data)
        v0, v1, v2, bp_count = stream.read_u32(), stream.read_u32(), stream.read_u32(), stream.read_u32()
        for bp_i in range(bp_count):
            bp_id = stream.read_u32()
            prop_count = stream.read_u32()
            bp_name = stream.read_str()
            print(f"Chunk #{chunk_idx} (offset 0x{pos:x}, size {size}): Blueprint '{bp_name}' (props={prop_count})")
    pos += size
    chunk_idx += 1
