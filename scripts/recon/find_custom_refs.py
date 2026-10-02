#!/usr/bin/env python3
import struct

with open("orig/bin/EBOOT.BIN", "rb") as f:
    eboot = f.read()

# Let's search for references to strings:
target_addrs = [
    (0x002db728, "FE_Customisation_OnInitMenu"),
    (0x002db760, "FE_Customisation_OnDeInitMenu"),
    (0x002db79c, "FE_Customisation_Cancel"),
    (0x002db7cc, "FE_Customisation_ConfirmCancel"),
    (0x002db80c, "FE_Customisation_Prompt"),
    (0x002db83c, "FE_Customisation_Credits"),
    (0x002db870, "FE_Customisation_ListCategoryNames"),
    (0x002db8b8, "FE_Customisation_ListCategoryIcons"),
    (0x002db900, "FE_Customisation_ListCategoryTotals"),
    (0x002db948, "FE_Customisation_AcceptCategoryItem"),
    (0x002db990, "FE_Customisation_ListCarouselNames"),
    (0x002db9d8, "FE_Customisation_ListCarouselAnims"),
    (0x002dba20, "FE_Customisation_ListCarouselCosts"),
    (0x002dba68, "FE_Customisation_ListCarouselIcons"),
    (0x002dbab0, "FE_Customisation_AcceptCarouselItem"),
    (0x002dbaf8, "FE_Customisation_SellCarouselItem"),
]

for addr, name in target_addrs:
    pattern = struct.pack("<I", addr)
    idx = 0
    while True:
        idx = eboot.find(pattern, idx)
        if idx == -1: break
        # check surrounding words
        surrounding = struct.unpack("<4I", eboot[idx-8:idx+8])
        print(f"Ref to {name} (0x{addr:08x}) at 0x{idx:08x}: {[hex(x) for x in surrounding]}")
        idx += 4
