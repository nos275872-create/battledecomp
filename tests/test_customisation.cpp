#include "core/asura_blueprint.h"
#include "systems/customisation.h"
#include <iostream>
#include <cassert>

using namespace battledecomp::core;
using namespace battledecomp::systems;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " running test_customisation" << std::endl;
    std::cout << "========================================" << std::endl;

    const std::string commonAsrPath = "orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR";

    AsuraBlueprintArchive archive;
    bool loaded = archive.LoadFromFile(commonAsrPath);
    assert(loaded && "Failed to load COMMON.ASR!");

    auto& mgr = CustomisationManager::Instance();
    mgr.Initialize(&archive);

    std::cout << "[+] Verifying Customisation Category Counts (0x002DBD0C)..." << std::endl;
    assert(mgr.GetCategoryItemCount(CustomisationCategory::PrimaryWeapon) == 10);
    assert(mgr.GetCategoryItemCount(CustomisationCategory::SecondaryWeapon) == 8);
    assert(mgr.GetCategoryItemCount(CustomisationCategory::Explosive) == 6);
    assert(mgr.GetCategoryItemCount(CustomisationCategory::Equipment) == 8);
    assert(mgr.GetCategoryItemCount(CustomisationCategory::PowerUp) == 6);
    assert(mgr.GetCategoryItemCount(CustomisationCategory::Health) == 4);
    assert(mgr.GetCategoryItemCount(CustomisationCategory::Speed) == 4);
    assert(mgr.GetCategoryItemCount(CustomisationCategory::Agility) == 4);
    std::cout << "    All 8 category item counts verified! [OK]" << std::endl;

    std::cout << "[+] Verifying Blueprint Credit Costs (0x002304EC & 0x002305F8)..." << std::endl;
    // Primary weapons
    assert(mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, 0) == 0);  // None
    assert(mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, 1) == 25); // BlasterRifle
    assert(mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, 2) == 30); // ArcCaster
    assert(mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, 3) == 15); // Carbonite_Gun
    assert(mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, 5) == 30); // Shotgun
    assert(mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, 6) == 25); // Sniper Rifle
    assert(mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, 7) == 40); // Chaingun
    assert(mgr.GetItemCost(CustomisationCategory::PrimaryWeapon, 9) == 25); // Rocket_Launcher

    // Secondary weapons
    assert(mgr.GetItemCost(CustomisationCategory::SecondaryWeapon, 0) == 0);  // BlasterPistol (Free)
    assert(mgr.GetItemCost(CustomisationCategory::SecondaryWeapon, 1) == 0);  // Fusion Cutter (Free)
    assert(mgr.GetItemCost(CustomisationCategory::SecondaryWeapon, 2) == 15); // TriShot
    assert(mgr.GetItemCost(CustomisationCategory::SecondaryWeapon, 3) == 25); // EMP Launcher

    // Explosives
    assert(mgr.GetItemCost(CustomisationCategory::Explosive, 1) == 10); // Thermal Detonator
    assert(mgr.GetItemCost(CustomisationCategory::Explosive, 2) == 10); // Detpacks
    assert(mgr.GetItemCost(CustomisationCategory::Explosive, 3) == 10); // Proximity Mines
    assert(mgr.GetItemCost(CustomisationCategory::Explosive, 4) == 15); // Cluster Grenades
    assert(mgr.GetItemCost(CustomisationCategory::Explosive, 5) == 20); // Wrist Rocket

    // Equipment
    assert(mgr.GetItemCost(CustomisationCategory::Equipment, 2) == 5);  // Auto Turret Droid
    assert(mgr.GetItemCost(CustomisationCategory::Equipment, 5) == 30); // Jetpack
    assert(mgr.GetItemCost(CustomisationCategory::Equipment, 6) == 20); // Jumppack
    assert(mgr.GetItemCost(CustomisationCategory::Equipment, 7) == 25); // Personal Shield

    // Stats (0x002DF13C - 0x002DF15C)
    assert(mgr.GetStatCost(CustomisationCategory::Health, 0) == 0);
    assert(mgr.GetStatCost(CustomisationCategory::Health, 1) == 10);
    assert(mgr.GetStatCost(CustomisationCategory::Health, 2) == 20);
    assert(mgr.GetStatCost(CustomisationCategory::Health, 3) == 30);

    assert(mgr.GetStatCost(CustomisationCategory::Speed, 0) == 0);
    assert(mgr.GetStatCost(CustomisationCategory::Speed, 1) == 10);
    assert(mgr.GetStatCost(CustomisationCategory::Speed, 2) == 20);
    assert(mgr.GetStatCost(CustomisationCategory::Speed, 3) == 30);

    assert(mgr.GetStatCost(CustomisationCategory::Agility, 0) == 0);
    assert(mgr.GetStatCost(CustomisationCategory::Agility, 1) == 15);
    assert(mgr.GetStatCost(CustomisationCategory::Agility, 2) == 25);
    assert(mgr.GetStatCost(CustomisationCategory::Agility, 3) == 35);
    std::cout << "    Credit costs verified against Blueprint data! [OK]" << std::endl;

    std::cout << "[+] Verifying SoldierLoadout Credit Calculations..." << std::endl;
    // Standard Trooper Loadout: BlasterRifle (25) + BlasterPistol (0) + Thermal Detonator (10)
    SoldierLoadout standardTrooper;
    standardTrooper.primaryWeaponIndex = 1;
    standardTrooper.secondaryWeaponIndex = 0;
    standardTrooper.explosiveIndex = 1;
    standardTrooper.equipmentIndex = 0;
    standardTrooper.powerUpIndex = 0;
    assert(standardTrooper.CalculateTotalCost() == 35);
    assert(standardTrooper.GetRemainingCredits() == 65);
    assert(standardTrooper.IsValid());
    std::cout << "    Standard Trooper Loadout (35 cr / 100 cr) [OK]" << std::endl;

    // Heavy Assault Loadout: Chaingun (40) + TriShot (15) + Wrist Rocket (20) + Personal Shield (25) = 100 cr
    SoldierLoadout heavyAssault;
    heavyAssault.primaryWeaponIndex = 7;   // Chaingun (40)
    heavyAssault.secondaryWeaponIndex = 2; // TriShot (15)
    heavyAssault.explosiveIndex = 5;       // Wrist Rocket (20)
    heavyAssault.equipmentIndex = 7;       // Personal Shield (25)
    assert(heavyAssault.CalculateTotalCost() == 100);
    assert(heavyAssault.GetRemainingCredits() == 0);
    assert(heavyAssault.IsValid());
    std::cout << "    Heavy Assault Loadout (100 cr / 100 cr, budget limit) [OK]" << std::endl;

    // Overbudget Loadout: Chaingun (40) + Jetpack (30) + Health Tier 3 (30) + Agility Tier 1 (15) = 115 cr
    SoldierLoadout overBudget;
    overBudget.primaryWeaponIndex = 7;   // Chaingun (40)
    overBudget.secondaryWeaponIndex = 0; // BlasterPistol (0)
    overBudget.explosiveIndex = 0;       // None (0)
    overBudget.equipmentIndex = 5;       // Jetpack (30)
    overBudget.powerUpIndex = 0;         // None (0)
    overBudget.healthTier = 3;           // Health Tier 3 (30)
    overBudget.speedTier = 0;
    overBudget.agilityTier = 1;          // Agility Tier 1 (15)
    assert(overBudget.CalculateTotalCost() == 115);
    assert(overBudget.GetRemainingCredits() == -15);
    assert(!overBudget.IsValid());
    std::cout << "    Overbudget Loadout (115 cr / 100 cr correctly rejected) [OK]" << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << " ALL CUSTOMISATION TESTS PASSED (100% OK)" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
