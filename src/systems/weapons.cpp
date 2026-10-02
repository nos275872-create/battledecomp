#include "systems/weapons.h"
#include "core/resource_mgr.h"
#include "core/asura_blueprint.h"

#include <iostream>
#include <memory>
#include <unordered_map>

namespace battledecomp::systems {

namespace {

static const char* const kWeaponInternalIds[static_cast<size_t>(WeaponId::Count)] = {
    "WP_BLASTER_RIFLE",
    "WP_BLASTER_PISTOL",
    "WP_ARC_CASTER",
    "WP_FUSION_CUTTER",
    "WP_CARBONITE_FREEZE_GUN",
    "WP_TRI_SHOT",
    "WP_INCINERATOR",
    "WP_WRIST_ROCKET",
    "WP_SHOTGUN",
    "WP_EMP_LAUNCHER",
    "WP_SNIPER_RIFLE",
    "WP_EXPLOSIVE_BLASTER_PISTOL",
    "WP_CHAINGUN",
    "WP_GRENADE_LAUNCHER",
    "WP_BOWCASTER",
    "WP_GUIDED_ROCKET",
    "WP_ROCKET_LAUNCHER",
    "WP_THERMAL_DETONATOR",
    "WP_DETPACKS",
    "WP_MINES",
    "WP_CLUSTER_GRENADE"
};

// Maps WeaponId to blueprint template name in COMMON.ASR
const char* GetBlueprintTemplateName(WeaponId id) {
    switch (id) {
        case WeaponId::BlasterRifle:           return "BlasterRifle";
        case WeaponId::BlasterPistol:          return "BlasterPistol";
        case WeaponId::ArcCaster:              return "ArcCaster";
        case WeaponId::FusionCutter:           return "Fusion Cutter";
        case WeaponId::CarboniteFreezeGun:     return "Carbonite_Gun";
        case WeaponId::TriShot:                return "TriShot";
        case WeaponId::Incinerator:            return "Flame Thrower";
        case WeaponId::WristRocket:            return "Wrist_Rocket";
        case WeaponId::Shotgun:                return "Shotgun";
        case WeaponId::EmpLauncher:            return "Emp_Launcher";
        case WeaponId::SniperRifle:            return "Sniper Rifle";
        case WeaponId::ExplosiveBlasterPistol: return "Particle_Pistol";
        case WeaponId::Chaingun:               return "Chaingun";
        case WeaponId::GrenadeLauncher:        return "Grenade_Launcher_OnTime";
        case WeaponId::Bowcaster:              return "BowCaster";
        case WeaponId::GuidedRocket:           return "Guided_Rocket";
        case WeaponId::RocketLauncher:         return "Rocket_Launcher";
        case WeaponId::ThermalDetonator:       return "REP_Grenade";
        case WeaponId::Detpacks:               return "Det_Pack";
        case WeaponId::Mines:                  return "Proximity_Mines";
        case WeaponId::ClusterGrenade:         return "Cluster_Grenades";
        default:                               return "Base";
    }
}

std::unique_ptr<core::AsuraBlueprintArchive> g_blueprintArchive;

} // anonymous namespace

const char* Weapons_GetHudArchiveName(GameEra era) {
    if (era == GameEra::Prequel) {
        return "Graphics\\HUD_Weapons_Prequel.asr";
    }
    if (era == GameEra::Classic) {
        return "Graphics\\HUD_Weapons_Classic.asr";
    }
    return nullptr;
}

const char* Weapons_GetInternalId(WeaponId id) {
    auto idx = static_cast<size_t>(id);
    if (idx < static_cast<size_t>(WeaponId::Count)) {
        return kWeaponInternalIds[idx];
    }
    return "WP_UNKNOWN";
}

std::string Weapons_GetWeaponName(WeaponId id, core::LanguageId lang) {
    auto idx = static_cast<size_t>(id);
    return core::ResourceMgr::Instance().GetText("WeaponNames", lang, idx);
}

std::vector<WeaponInfo> Weapons_GetAllWeapons(core::LanguageId lang) {
    std::vector<WeaponInfo> list;
    list.reserve(static_cast<size_t>(WeaponId::Count));

    for (size_t i = 0; i < static_cast<size_t>(WeaponId::Count); ++i) {
        WeaponId id = static_cast<WeaponId>(i);
        WeaponInfo info;
        info.id = id;
        info.internalId = Weapons_GetInternalId(id);
        info.localizedName = Weapons_GetWeaponName(id, lang);
        info.combatStats = Weapons_GetCombatStats(id);
        list.push_back(std::move(info));
    }

    return list;
}

bool Weapons_LoadBlueprints(const std::string& commonAsrPath) {
    auto archive = std::make_unique<core::AsuraBlueprintArchive>();
    if (!archive->LoadFromFile(commonAsrPath)) {
        std::cerr << "[Weapons] Failed to load blueprints from " << commonAsrPath << std::endl;
        return false;
    }
    g_blueprintArchive = std::move(archive);
    return true;
}

const core::AsuraBlueprintArchive* Weapons_GetBlueprintArchive() {
    return g_blueprintArchive.get();
}

WeaponCombatStats Weapons_GetCombatStats(WeaponId id) {
    WeaponCombatStats stats;
    const char* tmplName = GetBlueprintTemplateName(id);
    stats.blueprintTemplate = tmplName;

    const core::Blueprint* weaponBp = nullptr;
    const core::Blueprint* projBp = nullptr;
    const core::Blueprint* laserBp = nullptr;

    if (g_blueprintArchive) {
        weaponBp = g_blueprintArchive->FindBlueprint("Weapon");
        projBp = g_blueprintArchive->FindBlueprint("Projectile");
        laserBp = g_blueprintArchive->FindBlueprint("LaserBolt");
    }

    const core::BlueprintProperty* baseProp = weaponBp ? weaponBp->FindProperty("Base") : nullptr;
    const core::BlueprintProperty* tmplProp = weaponBp ? weaponBp->FindProperty(tmplName) : nullptr;

    auto GetFloatVal = [&](const std::string& key, float def) -> float {
        if (tmplProp && tmplProp->HasElement(key)) return tmplProp->GetFloat(key, def);
        if (baseProp && baseProp->HasElement(key)) return baseProp->GetFloat(key, def);
        return def;
    };

    auto GetIntVal = [&](const std::string& key, int32_t def) -> int32_t {
        if (tmplProp && tmplProp->HasElement(key)) return tmplProp->GetInt(key, def);
        if (baseProp && baseProp->HasElement(key)) return baseProp->GetInt(key, def);
        return def;
    };

    auto GetStringVal = [&](const std::string& key, const std::string& def) -> std::string {
        if (tmplProp && tmplProp->HasElement(key)) return tmplProp->GetString(key, def);
        if (baseProp && baseProp->HasElement(key)) return baseProp->GetString(key, def);
        return def;
    };

    stats.rateOfFire = GetFloatVal("RateOfFire", 0.5f);
    stats.reloadTime = GetFloatVal("ReloadTime", 0.0f);
    stats.ammoPerClip = GetIntVal("AmmoPerClip", -1);
    stats.maxClips = GetIntVal("MaxClips", -1);
    stats.projectilesPerShot = GetIntVal("ProjectilesPerShot", 1);
    stats.overheatPerShot = GetFloatVal("OverheatPershot", 0.0f);
    stats.overheatDecay = GetFloatVal("OverheatDecay", 0.0f);
    stats.overheatRecoveryTime = GetFloatVal("OverheatRecoveryTime", 0.0f);
    stats.maxRange = GetFloatVal("Distance", 750.0f);
    stats.projectileName = GetStringVal("Projectile", "");

    if (stats.ammoPerClip == -1 && stats.overheatPerShot > 0.0f) {
        stats.isOverheatBased = true;
    }

    // Resolve projectile or laser bolt stats
    if (!stats.projectileName.empty()) {
        const core::BlueprintProperty* boltProp = laserBp ? laserBp->FindProperty(stats.projectileName) : nullptr;
        if (boltProp) {
            const core::BlueprintProperty* boltBase = laserBp ? laserBp->FindProperty("base") : nullptr;
            auto GetBoltFloat = [&](const std::string& key, float def) -> float {
                if (boltProp->HasElement(key)) return boltProp->GetFloat(key, def);
                if (boltBase && boltBase->HasElement(key)) return boltBase->GetFloat(key, def);
                return def;
            };
            stats.damageInfantry = GetBoltFloat("DamageToInfantry", 10.0f);
            stats.damageVehicle = GetBoltFloat("DamageToVehicle", 10.0f);
            stats.projectileSpeed = GetBoltFloat("ProjectileSpeed", 100.0f);
            stats.maxRange = GetBoltFloat("range", stats.maxRange);
        } else {
            const core::BlueprintProperty* pProp = projBp ? projBp->FindProperty(stats.projectileName) : nullptr;
            if (pProp) {
                const core::BlueprintProperty* pBase = projBp ? projBp->FindProperty("Base") : nullptr;
                auto GetProjFloat = [&](const std::string& key, float def) -> float {
                    if (pProp->HasElement(key)) return pProp->GetFloat(key, def);
                    if (pBase && pBase->HasElement(key)) return pBase->GetFloat(key, def);
                    return def;
                };
                stats.damageInfantry = GetProjFloat("DamageToInfantry", 60.0f);
                stats.damageVehicle = GetProjFloat("DamageToVehicle", 40.0f);
                stats.projectileSpeed = GetProjFloat("ProjectileSpeed", 10.0f);
                stats.explosionRadius = GetProjFloat("ExplosionRadius", 0.0f);
                stats.maxRange = GetProjFloat("MaxRange", stats.maxRange);
            }
        }
    }

    // Default fallbacks if blueprints aren't loaded from disk yet
    if (!g_blueprintArchive) {
        switch (id) {
            case WeaponId::BlasterRifle:
                stats.rateOfFire = 0.3f;
                stats.reloadTime = 2.25f;
                stats.ammoPerClip = 25;
                stats.maxClips = 5;
                stats.damageInfantry = 15.0f;
                stats.damageVehicle = 6.0f;
                stats.projectileSpeed = 100.0f;
                stats.projectileName = "RifleLaser";
                break;
            case WeaponId::BlasterPistol:
                stats.rateOfFire = 0.5f;
                stats.ammoPerClip = -1;
                stats.maxClips = -1;
                stats.overheatPerShot = 20.0f;
                stats.overheatDecay = 18.0f;
                stats.damageInfantry = 10.0f;
                stats.damageVehicle = 5.0f;
                stats.projectileSpeed = 100.0f;
                stats.isOverheatBased = true;
                stats.projectileName = "PistolLaser";
                break;
            case WeaponId::Shotgun:
                stats.rateOfFire = 1.5f;
                stats.reloadTime = 2.25f;
                stats.ammoPerClip = 25;
                stats.maxClips = 6;
                stats.projectilesPerShot = 8;
                stats.projectileName = "ShotgunLaser";
                break;
            case WeaponId::SniperRifle:
                stats.rateOfFire = 2.0f;
                stats.reloadTime = 0.5f;
                stats.ammoPerClip = 5;
                stats.maxClips = 5;
                stats.projectileName = "Hit Scan";
                break;
            case WeaponId::RocketLauncher:
                stats.reloadTime = 4.4f;
                stats.ammoPerClip = 1;
                stats.maxClips = 7;
                stats.damageInfantry = 150.0f;
                stats.damageVehicle = 275.0f;
                stats.explosionRadius = 8.0f;
                stats.projectileSpeed = 20.0f;
                stats.projectileName = "rocket";
                break;
            case WeaponId::ThermalDetonator:
                stats.damageInfantry = 100.0f;
                stats.damageVehicle = 150.0f;
                stats.explosionRadius = 8.0f;
                stats.projectileSpeed = 10.0f;
                stats.projectileName = "Grenade";
                break;
            case WeaponId::WristRocket:
                stats.rateOfFire = 1.25f;
                stats.ammoPerClip = 4;
                stats.maxClips = 0;
                stats.damageInfantry = 75.0f;
                stats.damageVehicle = 120.0f;
                stats.explosionRadius = 6.0f;
                stats.projectileName = "Wrist_Rocket";
                break;
            default:
                break;
        }
    }

    return stats;
}

} // namespace battledecomp::systems
