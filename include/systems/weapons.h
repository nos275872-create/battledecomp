#pragma once

#include "core/asura_archive.h"
#include "core/asura_blueprint.h"
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

// Combat parameters decoded from COMMON.ASR 'BLUE' (Blueprint) chunks
struct WeaponCombatStats {
    float rateOfFire{0.0f};           // Delay between shots in seconds (RateOfFire)
    float reloadTime{0.0f};           // Seconds required to reload (ReloadTime)
    int32_t ammoPerClip{0};           // Shots per magazine (-1 if overheat based)
    int32_t maxClips{0};              // Maximum reserve clips (-1 if overheat based)
    int32_t projectilesPerShot{1};    // Number of projectiles launched per trigger pull
    float overheatPerShot{0.0f};      // Heat added per discharge (OverheatPershot)
    float overheatDecay{0.0f};        // Heat cooled down per second (OverheatDecay)
    float overheatRecoveryTime{0.0f}; // Penalty cooldown delay when overheated (OverheatRecoveryTime)
    float damageInfantry{0.0f};       // Damage inflicted to humanoid targets (DamageToInfantry)
    float damageVehicle{0.0f};        // Damage inflicted to armor/vehicles (DamageToVehicle)
    float projectileSpeed{0.0f};      // Movement velocity in units/second (ProjectileSpeed)
    float explosionRadius{0.0f};      // Blast area-of-effect radius (ExplosionRadius)
    float maxRange{0.0f};             // Effective maximum engagement distance (Distance / range)
    bool isOverheatBased{false};      // True if weapon uses heat mechanics rather than ammo clips
    std::string projectileName;       // Linked projectile/laser bolt blueprint identifier
    std::string blueprintTemplate;    // Primary archetype blueprint property name in COMMON.ASR
};

// Information container for a weapon definition
struct WeaponInfo {
    WeaponId id;
    const char* internalId;
    std::string localizedName;
    WeaponCombatStats combatStats;
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

// Loads Blueprint data (COMMON.ASR) to populate live weapon stats
bool Weapons_LoadBlueprints(const std::string& commonAsrPath);

// Retrieves weapon combat attributes and stats
WeaponCombatStats Weapons_GetCombatStats(WeaponId id);

// Returns the underlying BlueprintArchive (or nullptr if not loaded)
const core::AsuraBlueprintArchive* Weapons_GetBlueprintArchive();

} // namespace battledecomp::systems
