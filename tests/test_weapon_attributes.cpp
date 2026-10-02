#include "core/asura_blueprint.h"
#include "systems/weapons.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace battledecomp::core;
using namespace battledecomp::systems;

bool NearlyEqual(float a, float b, float epsilon = 0.05f) {
    return std::fabs(a - b) <= epsilon;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " running test_weapon_attributes" << std::endl;
    std::cout << "========================================" << std::endl;

    const std::string commonAsrPath = "orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR";

    // 1. Direct Blueprint Archive loading
    AsuraBlueprintArchive bpArchive;
    bool loaded = bpArchive.LoadFromFile(commonAsrPath);
    assert(loaded && "Failed to load COMMON.ASR blueprints!");
    std::cout << "[PASS] Successfully loaded COMMON.ASR (blueprints count: "
              << bpArchive.GetBlueprintCount() << ")" << std::endl;

    assert(bpArchive.FindBlueprint("Weapon") != nullptr);
    assert(bpArchive.FindBlueprint("Projectile") != nullptr);
    assert(bpArchive.FindBlueprint("LaserBolt") != nullptr);
    assert(bpArchive.FindBlueprint("Humanoid") != nullptr);
    assert(bpArchive.FindBlueprint("Flyer") != nullptr);
    assert(bpArchive.FindBlueprint("Tank") != nullptr);
    assert(bpArchive.FindBlueprint("Turret") != nullptr);
    std::cout << "[PASS] Verified core engine blueprints exist ('Weapon', 'Projectile', 'LaserBolt', 'Humanoid', 'Flyer', 'Tank', 'Turret')" << std::endl;

    // 2. Integration with Weapons subsystem
    bool sysLoaded = Weapons_LoadBlueprints(commonAsrPath);
    assert(sysLoaded && "Weapons_LoadBlueprints failed!");
    std::cout << "[PASS] Weapons_LoadBlueprints initialized" << std::endl;

    // Test Blaster Rifle
    {
        WeaponCombatStats rifle = Weapons_GetCombatStats(WeaponId::BlasterRifle);
        std::cout << "BlasterRifle: RoF=" << rifle.rateOfFire
                  << ", Reload=" << rifle.reloadTime
                  << ", Ammo=" << rifle.ammoPerClip << "/" << rifle.maxClips
                  << ", DmgInf=" << rifle.damageInfantry
                  << ", DmgVeh=" << rifle.damageVehicle
                  << ", Proj=" << rifle.projectileName << std::endl;
        assert(NearlyEqual(rifle.rateOfFire, 0.3f));
        assert(NearlyEqual(rifle.reloadTime, 2.25f));
        assert(rifle.ammoPerClip == 25);
        assert(rifle.maxClips == 5);
        assert(rifle.projectileName == "RifleLaser");
        assert(NearlyEqual(rifle.damageInfantry, 15.0f));
        assert(NearlyEqual(rifle.damageVehicle, 6.0f));
        assert(!rifle.isOverheatBased);
        std::cout << "[PASS] BlasterRifle combat stats validated" << std::endl;
    }

    // Test Blaster Pistol
    {
        WeaponCombatStats pistol = Weapons_GetCombatStats(WeaponId::BlasterPistol);
        std::cout << "BlasterPistol: RoF=" << pistol.rateOfFire
                  << ", HeatPerShot=" << pistol.overheatPerShot
                  << ", Decay=" << pistol.overheatDecay
                  << ", DmgInf=" << pistol.damageInfantry
                  << ", DmgVeh=" << pistol.damageVehicle
                  << ", Proj=" << pistol.projectileName << std::endl;
        assert(NearlyEqual(pistol.rateOfFire, 0.5f));
        assert(NearlyEqual(pistol.overheatPerShot, 20.0f));
        assert(NearlyEqual(pistol.overheatDecay, 18.0f));
        assert(pistol.isOverheatBased);
        assert(pistol.projectileName == "PistolLaser");
        assert(NearlyEqual(pistol.damageInfantry, 10.0f));
        assert(NearlyEqual(pistol.damageVehicle, 5.0f));
        std::cout << "[PASS] BlasterPistol combat stats validated" << std::endl;
    }

    // Test Shotgun
    {
        WeaponCombatStats shotgun = Weapons_GetCombatStats(WeaponId::Shotgun);
        std::cout << "Shotgun: RoF=" << shotgun.rateOfFire
                  << ", Reload=" << shotgun.reloadTime
                  << ", Ammo=" << shotgun.ammoPerClip << "/" << shotgun.maxClips
                  << ", Proj=" << shotgun.projectileName << std::endl;
        assert(NearlyEqual(shotgun.rateOfFire, 1.5f));
        assert(NearlyEqual(shotgun.reloadTime, 2.25f));
        assert(shotgun.ammoPerClip == 25);
        assert(shotgun.maxClips == 6);
        assert(shotgun.projectileName == "ShotgunLaser");
        std::cout << "[PASS] Shotgun combat stats validated" << std::endl;
    }

    // Test Sniper Rifle
    {
        WeaponCombatStats sniper = Weapons_GetCombatStats(WeaponId::SniperRifle);
        std::cout << "SniperRifle: RoF=" << sniper.rateOfFire
                  << ", Reload=" << sniper.reloadTime
                  << ", Ammo=" << sniper.ammoPerClip << "/" << sniper.maxClips
                  << ", Proj=" << sniper.projectileName << std::endl;
        assert(NearlyEqual(sniper.rateOfFire, 2.0f));
        assert(NearlyEqual(sniper.reloadTime, 0.5f));
        assert(sniper.ammoPerClip == 5);
        assert(sniper.maxClips == 5);
        assert(sniper.projectileName == "Hit Scan");
        std::cout << "[PASS] SniperRifle combat stats validated" << std::endl;
    }

    // Test Rocket Launcher
    {
        WeaponCombatStats rocket = Weapons_GetCombatStats(WeaponId::RocketLauncher);
        std::cout << "RocketLauncher: Reload=" << rocket.reloadTime
                  << ", Ammo=" << rocket.ammoPerClip << "/" << rocket.maxClips
                  << ", DmgInf=" << rocket.damageInfantry
                  << ", DmgVeh=" << rocket.damageVehicle
                  << ", Radius=" << rocket.explosionRadius
                  << ", Speed=" << rocket.projectileSpeed
                  << ", Proj=" << rocket.projectileName << std::endl;
        assert(NearlyEqual(rocket.reloadTime, 4.4f));
        assert(rocket.ammoPerClip == 1);
        assert(rocket.maxClips == 7);
        assert(rocket.projectileName == "rocket");
        assert(NearlyEqual(rocket.damageInfantry, 150.0f));
        assert(NearlyEqual(rocket.damageVehicle, 275.0f));
        assert(NearlyEqual(rocket.explosionRadius, 8.0f));
        assert(NearlyEqual(rocket.projectileSpeed, 20.0f));
        std::cout << "[PASS] RocketLauncher combat stats validated" << std::endl;
    }

    // Test Thermal Detonator (Grenade)
    {
        WeaponCombatStats grenade = Weapons_GetCombatStats(WeaponId::ThermalDetonator);
        std::cout << "ThermalDetonator: DmgInf=" << grenade.damageInfantry
                  << ", DmgVeh=" << grenade.damageVehicle
                  << ", Radius=" << grenade.explosionRadius
                  << ", Speed=" << grenade.projectileSpeed
                  << ", Proj=" << grenade.projectileName << std::endl;
        assert(grenade.projectileName == "Grenade");
        assert(NearlyEqual(grenade.damageInfantry, 100.0f));
        assert(NearlyEqual(grenade.damageVehicle, 150.0f));
        assert(NearlyEqual(grenade.explosionRadius, 8.0f));
        assert(NearlyEqual(grenade.projectileSpeed, 10.0f));
        std::cout << "[PASS] ThermalDetonator combat stats validated" << std::endl;
    }

    // Test Wrist Rocket
    {
        WeaponCombatStats wrist = Weapons_GetCombatStats(WeaponId::WristRocket);
        std::cout << "WristRocket: RoF=" << wrist.rateOfFire
                  << ", Ammo=" << wrist.ammoPerClip << "/" << wrist.maxClips
                  << ", DmgInf=" << wrist.damageInfantry
                  << ", DmgVeh=" << wrist.damageVehicle
                  << ", Radius=" << wrist.explosionRadius
                  << ", Proj=" << wrist.projectileName << std::endl;
        assert(NearlyEqual(wrist.rateOfFire, 1.25f));
        assert(wrist.ammoPerClip == 4);
        assert(wrist.maxClips == 0);
        assert(wrist.projectileName == "Wrist_Rocket");
        assert(NearlyEqual(wrist.damageInfantry, 75.0f));
        assert(NearlyEqual(wrist.damageVehicle, 120.0f));
        assert(NearlyEqual(wrist.explosionRadius, 6.0f));
        std::cout << "[PASS] WristRocket combat stats validated" << std::endl;
    }

    // 3. Test all 21 weapons mapping
    for (size_t i = 0; i < static_cast<size_t>(WeaponId::Count); ++i) {
        WeaponId id = static_cast<WeaponId>(i);
        WeaponCombatStats s = Weapons_GetCombatStats(id);
        const char* internalId = Weapons_GetInternalId(id);
        assert(internalId != nullptr && std::string(internalId).find("WP_") == 0);
        assert(!s.blueprintTemplate.empty());
    }
    std::cout << "[PASS] Verified all 21 cataloged weapons have valid blueprint mappings" << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << " ALL 8 TEST SUITES PASSED (100% OK)" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
