#include "systems/vehicles.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace battledecomp::systems;

void TestVehicleBlueprintsAndCatalog() {
    std::cout << "[+] Verifying Vehicle & Turret Blueprints (COMMON.ASR)..." << std::endl;

    VehicleManager& mgr = VehicleManager::GetInstance();
    bool loaded = mgr.LoadFromCommonArchive("orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR");
    assert(loaded && "Must load COMMON.ASR successfully");

    const auto& allBps = mgr.GetAllBlueprints();
    assert(allBps.size() == 117 && "Must load 117 vehicle/turret blueprints");

    size_t tanks = 0, turrets = 0, flyers = 0, walkers = 0;
    for (const auto& stats : allBps) {
        if (stats.category == VehicleCategory::Tank) tanks++;
        else if (stats.category == VehicleCategory::Turret) turrets++;
        else if (stats.category == VehicleCategory::Flyer) flyers++;
        else if (stats.category == VehicleCategory::Walker) walkers++;
    }

    assert(tanks == 21 && "Tanks count must be 21");
    assert(turrets == 48 && "Turrets count must be 48");
    assert(flyers == 41 && "Flyers count must be 41");
    assert(walkers == 7 && "Walkers count must be 7");

    std::cout << "    Verified template counts: 21 Tanks, 48 Turrets, 41 Flyers, 7 Walkers (117 total) [OK]" << std::endl;

    // Verify iconic Walker: AT_AT
    const auto* atat = mgr.FindBlueprintStats("AT_AT");
    assert(atat != nullptr);
    assert(atat->category == VehicleCategory::Walker);
    assert(atat->maxHealth == 3500.0f);
    assert(atat->size == VehicleSize::VeryLarge);
    assert(atat->turrets.size() == 1);
    assert(atat->turrets[0].turretBlueprint == "AT_AT_turret_01");
    std::cout << "    AT_AT Walker verified: Health=3500, Size=VeryLarge, Turret=AT_AT_turret_01 [OK]" << std::endl;

    // Verify multi-turret Walker: AT_TE (5 turrets)
    const auto* atte = mgr.FindBlueprintStats("AT_TE");
    assert(atte != nullptr);
    assert(atte->category == VehicleCategory::Walker);
    assert(atte->maxHealth == 2500.0f);
    assert(atte->driverBlueprint == "Clonetrooper");
    assert(atte->turrets.size() == 5);
    assert(atte->turrets[0].turretBlueprint == "AT_TE_turret_01");
    assert(atte->turrets[4].turretBlueprint == "AT_TE_turret_05");
    std::cout << "    AT_TE Walker verified: Health=2500, Driver=Clonetrooper, 5 Turrets [OK]" << std::endl;

    // Verify Tank: IFTX_tank
    const auto* iftx = mgr.FindBlueprintStats("IFTX_tank");
    assert(iftx != nullptr);
    assert(iftx->category == VehicleCategory::Tank);
    assert(iftx->maxHealth == 800.0f); // Inherited from HoverTankBase
    assert(iftx->hardpoints.size() >= 2);
    assert(iftx->hardpoints[0].weaponBlueprint == "TankLasers_IFT");
    assert(iftx->hardpoints[1].weaponBlueprint == "Concussion_Missile");
    assert(iftx->turrets.size() == 1);
    assert(iftx->turrets[0].turretBlueprint == "IFTX_tank_turret_01_Base");
    std::cout << "    IFTX Tank verified: Health=800, Weapons=(TankLasers_IFT, Concussion_Missile), Turret [OK]" << std::endl;

    // Verify Flyer: X_Wing
    const auto* xwing = mgr.FindBlueprintStats("X_Wing");
    assert(xwing != nullptr);
    assert(xwing->category == VehicleCategory::Flyer);
    assert(xwing->maxHealth == 250.0f); // Inherited from Fighters
    assert(xwing->topSpeed == 175.0f);
    assert(xwing->cruisingSpeed == 100.0f);
    assert(xwing->hardpoints.size() >= 1);
    assert(xwing->hardpoints[0].weaponBlueprint == "SpaceLasers_X_Wing");
    std::cout << "    X_Wing Flyer verified: Health=250, TopSpeed=175, Weapon=SpaceLasers_X_Wing [OK]" << std::endl;

    // Verify Turret: Dish_Turret
    const auto* dish = mgr.FindBlueprintStats("Dish_Turret");
    assert(dish != nullptr);
    assert(dish->category == VehicleCategory::Turret);
    assert(dish->maxHealth == 300.0f);
    assert(dish->hardpoints.size() >= 1);
    assert(dish->hardpoints[0].weaponBlueprint == "Particle_Beam_Turret");
    std::cout << "    Dish_Turret verified: Health=300, Weapon=Particle_Beam_Turret [OK]" << std::endl;
}

void TestVehicleAIControllerAndModes() {
    std::cout << "[+] Verifying Vehicle AI Controller & Behavior Modes (0x0031773C)..." << std::endl;

    VehicleAIController ai;
    assert(ai.GetBehaviorMode() == VehicleBehaviorMode::Unset);
    assert(std::string(ai.GetBehaviorModeString()) == "Vehicle - UNSET");

    // Test all 11 mode strings exactly matching 0x0031773C
    const char* expectedModes[] = {
        "Vehicle - UNSET",
        "Vehicle - NONE",
        "Vehicle - TAKE_OFF",
        "Vehicle - GENERAL_COMBAT",
        "Vehicle - ATTACK_SHIP",
        "Vehicle - DEFEND_SHIP",
        "Vehicle - FOLLOW_SHIP",
        "Vehicle - CAPTURE_FLAG",
        "Vehicle - ATTACK_TARGET",
        "Vehicle - LAND_IN_HANGAR",
        "Vehicle - GO_TO_POSITION"
    };

    for (uint32_t i = 0; i < 11; ++i) {
        auto mode = static_cast<VehicleBehaviorMode>(i);
        ai.SetBehaviorMode(mode);
        assert(ai.GetBehaviorMode() == mode);
        assert(std::string(ai.GetBehaviorModeString()) == expectedModes[i]);
    }
    std::cout << "    All 11 Vehicle AI behavior modes verified! [OK]" << std::endl;

    // Test Vehicle Events (0x002D9FD8)
    const char* expectedEvents[] = {
        "no event",
        "vehicle created",
        "vehicle destroyed",
        "flag created",
        "flag picked up",
        "approach object created",
        "approach object destroyed",
        "approach object team changed",
        "vehicle orders completed",
        "vehicle orders aborted"
    };

    for (uint32_t i = 0; i < 10; ++i) {
        auto ev = static_cast<VehicleEvent>(i);
        assert(std::string(VehicleManager::GetEventString(ev)) == expectedEvents[i]);
    }
    std::cout << "    All 10 Vehicle Commander Events verified! [OK]" << std::endl;

    // Test Threat engagement transition
    ai.SetBehaviorMode(VehicleBehaviorMode::GeneralCombat);
    ai.SetTargetThreat(501);
    ai.Update(0.1f);
    assert(ai.GetManeuverAction() == VehicleManeuverAction::Attack);
    assert(ai.GetNavigationAction() == VehicleNavigationAction::PointingAtThreat);
    std::cout << "    Combat threat tracking & maneuver actions verified! [OK]" << std::endl;
}

void TestVehicleRuntimeSimulation() {
    std::cout << "[+] Verifying Runtime Vehicle Instance & Damage/Repair Simulation..." << std::endl;

    VehicleManager& mgr = VehicleManager::GetInstance();
    mgr.ClearEvents();

    auto tank = mgr.SpawnVehicle("IFTX_tank", 2001);
    assert(tank != nullptr);
    assert(tank->GetId() == 2001);
    assert(tank->GetName() == "IFTX_tank");
    assert(tank->GetMaxHealth() == 800.0f);
    assert(tank->GetCurrentHealth() == 800.0f);
    assert(tank->IsAlive());

    // Verify creation event
    const auto& events = mgr.GetRecentEvents();
    assert(!events.empty());
    assert(events[0].first == VehicleEvent::VehicleCreated);
    assert(events[0].second == 2001);

    // Board driver
    assert(!tank->HasDriver());
    assert(tank->SetDriver(101));
    assert(tank->HasDriver());
    assert(tank->GetDriverId() == 101);

    // Damage vehicle
    tank->TakeDamage(300.0f);
    assert(tank->GetCurrentHealth() == 500.0f);
    assert(tank->IsAlive());

    // Auto-repair simulation (5.0 hp/s * 10s = +50 hp -> 550 hp)
    tank->Update(10.0f);
    assert(std::fabs(tank->GetCurrentHealth() - 550.0f) < 0.01f);

    // Fatal damage & destruction
    tank->TakeDamage(1000.0f);
    assert(tank->GetCurrentHealth() == 0.0f);
    assert(tank->IsDestroyed());

    mgr.DestroyVehicle(2001);
    assert(mgr.GetVehicle(2001) == nullptr);
    assert(events.back().first == VehicleEvent::VehicleDestroyed);
    std::cout << "    Runtime vehicle lifecycle (Spawn -> Driver -> Damage -> AutoRepair -> Destroy) verified! [OK]" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " running test_vehicles" << std::endl;
    std::cout << "========================================" << std::endl;

    TestVehicleBlueprintsAndCatalog();
    TestVehicleAIControllerAndModes();
    TestVehicleRuntimeSimulation();

    std::cout << "\n========================================" << std::endl;
    std::cout << " ALL VEHICLE & TURRET TESTS PASSED (100% OK)" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
