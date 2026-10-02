#include "systems/customisation.h"
#include <algorithm>
#include <iostream>

namespace battledecomp::systems {

const char* CustomisationCategoryToString(CustomisationCategory cat) {
    switch (cat) {
        case CustomisationCategory::PrimaryWeapon:   return "PrimaryWeapon";
        case CustomisationCategory::SecondaryWeapon: return "SecondaryWeapon";
        case CustomisationCategory::Explosive:       return "Explosive";
        case CustomisationCategory::Equipment:       return "Equipment";
        case CustomisationCategory::PowerUp:         return "PowerUp";
        case CustomisationCategory::Health:          return "Health";
        case CustomisationCategory::Speed:           return "Speed";
        case CustomisationCategory::Agility:         return "Agility";
        default:                                     return "Unknown";
    }
}

int32_t SoldierLoadout::CalculateTotalCost() const {
    auto& mgr = CustomisationManager::Instance();
    int32_t total = 0;

    total += mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, primaryWeaponIndex);
    total += mgr.GetItemCost(CustomisationCategory::SecondaryWeapon, secondaryWeaponIndex);
    total += mgr.GetItemCost(CustomisationCategory::Explosive, explosiveIndex);
    total += mgr.GetItemCost(CustomisationCategory::Equipment, equipmentIndex);
    total += mgr.GetItemCost(CustomisationCategory::PowerUp, powerUpIndex);
    total += mgr.GetStatCost(CustomisationCategory::Health, healthTier);
    total += mgr.GetStatCost(CustomisationCategory::Speed, speedTier);
    total += mgr.GetStatCost(CustomisationCategory::Agility, agilityTier);

    return total;
}

int32_t SoldierLoadout::GetRemainingCredits(int32_t budget) const {
    return budget - CalculateTotalCost();
}

bool SoldierLoadout::IsValid(int32_t budget) const {
    return CalculateTotalCost() <= budget;
}

CustomisationManager& CustomisationManager::Instance() {
    static CustomisationManager s_instance;
    if (!s_instance.m_initialized) {
        s_instance.Initialize(nullptr);
    }
    return s_instance;
}

void CustomisationManager::Initialize(const core::AsuraBlueprintArchive* blueprintArchive) {
    for (size_t i = 0; i < static_cast<size_t>(CustomisationCategory::Count); ++i) {
        m_categories[i].clear();
    }

    const core::Blueprint* weaponBp = blueprintArchive ? blueprintArchive->FindBlueprint("Weapon") : nullptr;

    auto ResolveCost = [&](const std::string& name, int32_t defaultCost) -> int32_t {
        if (weaponBp) {
            const auto* prop = weaponBp->FindProperty(name);
            if (prop && prop->HasElement("Cost")) {
                return prop->GetInt("Cost", defaultCost);
            }
        }
        return defaultCost;
    };

    // Category 0: Primary Weapons (DAT_002d8668)
    m_categories[0] = {
        {0x00000000, "None",            0},
        {0xa6ec5d43, "BlasterRifle",     ResolveCost("BlasterRifle", 25)},
        {0x93b9a8fe, "ArcCaster",        ResolveCost("ArcCaster", 30)},
        {0x6379fa0e, "Carbonite_Gun",    ResolveCost("Carbonite_Gun", 15)},
        {0x38d0ed28, "ALL_Incinerator",  ResolveCost("Flame Thrower", 20)},
        {0x7b371c06, "Shotgun",          ResolveCost("Shotgun", 30)},
        {0x4691f857, "Sniper Rifle",     ResolveCost("Sniper Rifle", 25)},
        {0x65fa9041, "REP_Chaingun",     ResolveCost("Chaingun", 40)},
        {0x1b82399f, "ALL_Bow_Caster",   ResolveCost("BowCaster", 30)},
        {0x87d168ab, "Rocket_Launcher",  ResolveCost("Rocket_Launcher", 25)}
    };

    // Category 1: Secondary Weapons (DAT_002d8690)
    m_categories[1] = {
        {0x333ba25c, "BlasterPistol",             ResolveCost("BlasterPistol", 0)},
        {0xda0cae3b, "Fusion Cutter",             ResolveCost("Fusion Cutter", 0)},
        {0x285821ec, "ALL_Tri_Shot",              ResolveCost("ALL_Tri_shot", 15)},
        {0x001c1937, "Emp_Launcher",              ResolveCost("Emp_Launcher", 25)},
        {0x1155bd50, "Particle_Pistol",           ResolveCost("Particle_Pistol", 20)},
        {0x469279ba, "Grenade_Launcher_OnTime",   ResolveCost("Grenade_Launcher_OnTime", 20)},
        {0x4b53e4cf, "Guided_Missile",            ResolveCost("Guided_Missile", 25)},
        {0x8c52aedd, "Orbital Strike Gun",        ResolveCost("Orbital Strike Gun", 20)}
    };

    // Category 2: Special / Explosives (DAT_002d86b0)
    m_categories[2] = {
        {0x00000000, "None",              0},
        {0x2a3e7fac, "REP_Grenade",       ResolveCost("REP_Grenade", 10)},
        {0x3e2c2ac5, "Det_Pack",          ResolveCost("Det_Pack", 10)},
        {0x8b23bec0, "Proximity_Mines",   ResolveCost("Proximity_Mines", 10)},
        {0x98ba558a, "Cluster_Grenades",  ResolveCost("Cluster_Grenades", 15)},
        {0x2a945544, "Wrist_Rocket",      ResolveCost("Wrist_Rocket", 20)}
    };

    // Category 3: Equipment / Packs (DAT_002d86c8)
    m_categories[3] = {
        {0x00000000, "None",                       0},
        {0x5d1b8bbe, "ALL_HealthAmmo_Dispenser",    ResolveCost("ALL_HealthAmmo_Dispenser", 10)},
        {0xc56c8efd, "Auto_Turret_Droid",           ResolveCost("Auto_Turret_Droid", 5)},
        {0x72ab24ac, "Recon_Droid",                 ResolveCost("Recon_Droid", 10)},
        {0xfd9085c9, "Stealth_Suit",                ResolveCost("Stealth_Suit", 20)},
        {0x9a450832, "Jetpack",                     ResolveCost("Jetpack", 30)},
        {0xf0d48f47, "Jumppack",                    ResolveCost("Jumppack", 20)},
        {0x0ae26e48, "Personal_Shield",             ResolveCost("Personal_Shield", 25)}
    };

    // Category 4: PowerUp / Trait (DAT_002d86e8)
    m_categories[4] = {
        {0x00000000, "None",               0},
        {0x0674326a, "Rally",              ResolveCost("Rally", 10)},
        {0x00354b4d, "Rage",               ResolveCost("Rage", 15)},
        {0x8ee8d26f, "Stamina",            ResolveCost("Stamina", 5)},
        {0xb39c40a8, "Regenerate",         ResolveCost("Regenerate", 15)},
        {0xa90e4628, "VehicleAutoRepair",  ResolveCost("VehicleAutoRepair", 10)}
    };

    // Category 5: Health (DAT_002df13c)
    m_categories[5] = {
        {0, "Health Tier 0", 0},
        {1, "Health Tier 1", 10},
        {2, "Health Tier 2", 20},
        {3, "Health Tier 3", 30}
    };

    // Category 6: Speed (UNK_002df14c)
    m_categories[6] = {
        {0, "Speed Tier 0", 0},
        {1, "Speed Tier 1", 10},
        {2, "Speed Tier 2", 20},
        {3, "Speed Tier 3", 30}
    };

    // Category 7: Agility (UNK_002df15c)
    m_categories[7] = {
        {0, "Agility Tier 0", 0},
        {1, "Agility Tier 1", 15},
        {2, "Agility Tier 2", 25},
        {3, "Agility Tier 3", 35}
    };

    m_initialized = true;
}

size_t CustomisationManager::GetCategoryItemCount(CustomisationCategory cat) const {
    auto idx = static_cast<size_t>(cat);
    if (idx < static_cast<size_t>(CustomisationCategory::Count)) {
        return m_categories[idx].size();
    }
    return 0;
}

const CustomisationItem* CustomisationManager::GetItem(CustomisationCategory cat, size_t index) const {
    auto idx = static_cast<size_t>(cat);
    if (idx < static_cast<size_t>(CustomisationCategory::Count)) {
        if (index < m_categories[idx].size()) {
            return &m_categories[idx][index];
        }
    }
    return nullptr;
}

int32_t CustomisationManager::GetItemCost(CustomisationCategory cat, size_t index) const {
    const auto* item = GetItem(cat, index);
    return item ? item->cost : 0;
}

int32_t CustomisationManager::GetStatCost(CustomisationCategory cat, int32_t tier) const {
    if (tier < 0) tier = 0;
    if (tier > 3) tier = 3;
    return GetItemCost(cat, static_cast<size_t>(tier));
}

const std::vector<CustomisationItem>& CustomisationManager::GetCategoryItems(CustomisationCategory cat) const {
    static const std::vector<CustomisationItem> s_empty;
    auto idx = static_cast<size_t>(cat);
    if (idx < static_cast<size_t>(CustomisationCategory::Count)) {
        return m_categories[idx];
    }
    return s_empty;
}

} // namespace battledecomp::systems
