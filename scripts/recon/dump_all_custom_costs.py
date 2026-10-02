#!/usr/bin/env python3
import sys, struct
sys.path.append("scripts/recon")
from match_custom_hashes import cats, hash_to_name
from extract_weapon_stats import weapons_data

with open("orig/bin/EBOOT.BIN", "rb") as f:
    eboot = f.read()

def va_to_file(va): return va + 0x74

print("=================================================================")
print("  RENEGADE SQUADRON — FULL LOADOUT & CUSTOMISATION COSTS TABLE")
print("=================================================================")

for cat_name, va, count in cats:
    f_off = va_to_file(va)
    items = struct.unpack(f"<{count}I", eboot[f_off:f_off+count*4])
    print(f"\n--- {cat_name} (Max items: {count}) ---")
    for i, it in enumerate(items):
        name = hash_to_name.get(it, "None" if it == 0 else f"Unknown_0x{it:08x}")
        # Look up Cost in weapons_data
        cost = 0
        if name in weapons_data:
            cost = weapons_data[name].get("Cost", 0)
        print(f"  [{i}] {name:<30} -> Cost: {cost} credits")

# Also print stat upgrade costs
print("\n--- Stat Attribute Costs (4 Tiers each: None, Level 1, Level 2, Level 3) ---")
for s_name, va in [("Health", 0x002df13c), ("Speed", 0x002df14c), ("Agility", 0x002df15c)]:
    off = va_to_file(va)
    costs = struct.unpack("<4i", eboot[off:off+16])
    print(f"  {s_name:<10}: Tier 0={costs[0]}cr, Tier 1={costs[1]}cr, Tier 2={costs[2]}cr, Tier 3={costs[3]}cr")
