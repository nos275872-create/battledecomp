#pragma once

#include "core/asura_archive.h"
#include <string>
#include <vector>

namespace battledecomp::systems {

// Distinct weapons cataloged in WEAPONNAMES.ASR
enum class WeaponId : uint32_t {
    BlasterRifle           = 0,
    BlasterPistol          = 1,
    ArcCaster              = 2,
    FusionCutter           = 3,
    CarboniteFreezeGun     = 4,
    TriShot                = 5,
    Incinerator            = 6,
    WristRocket            = 7,
    Shotgun                = 8,
    EmpLauncher            = 9,
    SniperRifle            = 10,
    ExplosiveBlasterPistol = 11,
    Chaingun               = 12,
    GrenadeLauncher        = 13,
    Bowcaster              = 14,
    GuidedRocket           = 15,
    RocketLauncher         = 16,
    ThermalDetonator       = 17,
    Detpacks               = 18,
    Mines                  = 19,
    ClusterGrenade         = 20,
    Count                  = 21
};

// Campaign / Era setting controlling weapon HUD graphics (0x00317bdc)
enum class GameEra : uint32_t {
    Prequel = 0, // Clone Wars (Republic vs CIS)
    Classic = 1  // Galactic Civil War (Rebels vs Empire)
};

// Information container for a weapon definition
struct WeaponInfo {
    WeaponId id;
    const char* internalId;
    std::string localizedName;
};

// Reconstructed functions from EBOOT.BIN:
// Returns relative path to the HUD weapons texture archive for the era (0x00171F04)
const char* Weapons_GetHudArchiveName(GameEra era);

// Resolves weapon localized name using ResourceMgr
std::string Weapons_GetWeaponName(WeaponId id, core::LanguageId lang);

// Returns list of all 21 weapons with names localized in the requested language
std::vector<WeaponInfo> Weapons_GetAllWeapons(core::LanguageId lang);

// Converts WeaponId to standard internal canonical string
const char* Weapons_GetInternalId(WeaponId id);

} // namespace battledecomp::systems
