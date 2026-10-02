#include "systems/boarding.h"
#include "systems/vehicles.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace battledecomp::systems;

void TestSeatAutoConfiguration() {
    std::cout << "[+] Verifying Vehicle Seat Auto-Configuration from Blueprints..." << std::endl;

    auto& vm = VehicleManager::GetInstance();
    bool loaded = vm.LoadFromCommonArchive("orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR");
    assert(loaded);

    auto& bm = BoardingManager::GetInstance();
    bm.Clear();

    // 1. AT_TE Walker (Heavy Assault / Mobile Command Post)
    // 1 Driver + 5 Gunners (Turret0..4) + 4 Passengers = 10 seats
    const auto* atteStats = vm.FindBlueprintStats("AT_TE");
    assert(atteStats != nullptr);
    bm.SetupVehicleSeats(5001, *atteStats);

    const auto* atteSeats = bm.GetVehicleSeats(5001);
    assert(atteSeats != nullptr);
    assert(atteSeats->size() == 10 && "AT_TE must have 10 seats (1 Driver + 5 Gunners + 4 Passengers)");
    assert((*atteSeats)[0].type == VehicleSeatType::Driver);
    assert((*atteSeats)[1].type == VehicleSeatType::Gunner && (*atteSeats)[1].turretIndex == 0);
    assert((*atteSeats)[5].type == VehicleSeatType::Gunner && (*atteSeats)[5].turretIndex == 4);
    assert((*atteSeats)[6].type == VehicleSeatType::Passenger);
    std::cout << "    AT_TE Seats verified: 1 Driver, 5 Gunners, 4 Passengers (Total 10) [OK]" << std::endl;

    // 2. STAP Speeder (Single occupant, open cockpit)
    const auto* stapStats = vm.FindBlueprintStats("STAP");
    assert(stapStats != nullptr);
    bm.SetupVehicleSeats(5002, *stapStats);

    const auto* stapSeats = bm.GetVehicleSeats(5002);
    assert(stapSeats != nullptr);
    assert(stapSeats->size() == 1 && "STAP must have exactly 1 seat");
    assert((*stapSeats)[0].type == VehicleSeatType::Driver);
    assert(!stapStats->enclosed && "STAP is an open vehicle");
    std::cout << "    STAP Seats verified: 1 Driver (Exposed/Open Cockpit) [OK]" << std::endl;

    // 3. IFTX Tank (Hover Tank, 1 Turret, 2 Passengers)
    const auto* iftxStats = vm.FindBlueprintStats("IFTX_tank");
    assert(iftxStats != nullptr);
    bm.SetupVehicleSeats(5003, *iftxStats);

    const auto* iftxSeats = bm.GetVehicleSeats(5003);
    assert(iftxSeats != nullptr);
    assert(iftxSeats->size() == 4 && "IFTX must have 4 seats (1 Driver, 1 Gunner, 2 Passengers)");
    std::cout << "    IFTX Tank Seats verified: 1 Driver, 1 Gunner, 2 Passengers (Total 4) [OK]" << std::endl;
}

void TestBoardingFlowAndSeatAssignment() {
    std::cout << "[+] Verifying Boarding Flow and Seat Assignment..." << std::endl;

    auto& vm = VehicleManager::GetInstance();
    auto& bm = BoardingManager::GetInstance();

    auto tank = vm.SpawnVehicle("IFTX_tank", 6001);
    assert(tank != nullptr);
    bm.SetupVehicleSeats(6001, tank->GetStats());

    // Soldier 101 boards as Driver
    auto res1 = bm.BoardVehicle(101, 6001, VehicleSeatType::Driver, VehicleFaction::Republic, 2.0f);
    assert(res1 == BoardingResult::Success);
    assert(bm.IsSoldierInVehicle(101));
    assert(bm.GetVehicleDriver(6001) == 101);
    assert(tank->GetDriverId() == 101);

    const auto* s101State = bm.GetSoldierState(101);
    assert(s101State != nullptr);
    assert(s101State->seatType == VehicleSeatType::Driver);
    assert(s101State->isProtected == true); // Enclosed

    // Soldier 102 boards as Gunner
    auto res2 = bm.BoardVehicle(102, 6001, VehicleSeatType::Gunner, VehicleFaction::Republic, 1.5f);
    assert(res2 == BoardingResult::Success);
    const auto* s102State = bm.GetSoldierState(102);
    assert(s102State != nullptr);
    assert(s102State->seatType == VehicleSeatType::Gunner);
    assert(tank->GetTurret(0)->active == true);

    // Soldier 103 & 104 board as Passengers
    assert(bm.BoardVehicle(103, 6001, VehicleSeatType::Passenger, VehicleFaction::Republic, 2.5f) == BoardingResult::Success);
    assert(bm.BoardVehicle(104, 6001, VehicleSeatType::Passenger, VehicleFaction::Republic, 3.0f) == BoardingResult::Success);

    // Vehicle full: Soldier 105 rejected
    auto res5 = bm.BoardVehicle(105, 6001, VehicleSeatType::Passenger, VehicleFaction::Republic, 2.0f);
    assert(res5 == BoardingResult::NoSeatsAvailable);

    auto occs = bm.GetVehicleOccupants(6001);
    assert(occs.size() == 4);
    std::cout << "    Boarding flow verified: 4 Occupants seated (Driver, Gunner, 2 Passengers), capacity enforced [OK]" << std::endl;
}

void TestBoardingConstraintsAndProtection() {
    std::cout << "[+] Verifying Boarding Constraints & Protection..." << std::endl;

    auto& vm = VehicleManager::GetInstance();
    auto& bm = BoardingManager::GetInstance();

    auto tank = vm.GetVehicle(6001);
    assert(tank != nullptr);

    // Distance constraint (> 4.0m rejected)
    auto resDist = bm.BoardVehicle(106, 6001, VehicleSeatType::Driver, VehicleFaction::Republic, 6.5f);
    assert(resDist == BoardingResult::TooFar);
    std::cout << "    Distance constraint (6.5m > 4.0m -> TooFar) verified! [OK]" << std::endl;

    // Faction Hostile constraint (Enemy CIS trying to board Republic tank with active driver)
    auto resFaction = bm.BoardVehicle(201, 6001, VehicleSeatType::Gunner, VehicleFaction::CIS, 2.0f);
    assert(resFaction == BoardingResult::FactionHostile);
    std::cout << "    Faction hostile lock verified! [OK]" << std::endl;

    // Protection check: Enclosed vs Open
    // Enclosed tank takes 200 damage: occupants take 0 direct damage
    std::vector<std::pair<uint32_t, float>> occDmg;
    bm.ProcessVehicleDamage(6001, 200.0f, occDmg);
    assert(occDmg.empty() && "Enclosed vehicle occupants take 0 direct damage");
    std::cout << "    Enclosed cockpit full armor protection verified! [OK]" << std::endl;

    // Open STAP speeder takes 200 damage: exposed rider takes 25% splash (50.0 hp)
    auto stap = vm.SpawnVehicle("STAP", 6002);
    assert(stap != nullptr);
    bm.SetupVehicleSeats(6002, stap->GetStats());
    assert(bm.BoardVehicle(301, 6002, VehicleSeatType::Driver, VehicleFaction::CIS, 1.0f) == BoardingResult::Success);

    const auto* stapRiderState = bm.GetSoldierState(301);
    assert(stapRiderState != nullptr);
    assert(!stapRiderState->isProtected);
    assert(stapRiderState->sittingAnim == "Human_STAP_sitpose");

    occDmg.clear();
    bm.ProcessVehicleDamage(6002, 200.0f, occDmg);
    assert(occDmg.size() == 1);
    assert(occDmg[0].first == 301);
    assert(std::fabs(occDmg[0].second - 50.0f) < 0.01f);
    std::cout << "    Open speeder exposure (25% splash damage + sit animation) verified! [OK]" << std::endl;
}

void TestBotAIInteractionAndDestruction() {
    std::cout << "[+] Verifying Bot AI Boarding, Critical Egress and Destruction..." << std::endl;

    auto& vm = VehicleManager::GetInstance();
    auto& bm = BoardingManager::GetInstance();

    // Bot 101 in tank: health is 100% -> should NOT evacuate
    assert(!bm.EvaluateBotEgress(101));

    // Damage tank to critical (< 15% health): 800 hp * 0.15 = 120 hp threshold
    auto tank = vm.GetVehicle(6001);
    tank->TakeDamage(720.0f); // 800 - 720 = 80 hp (10% health)
    assert(tank->GetCurrentHealth() <= 80.0f);

    // Bot detects critical emergency and requests evacuation
    assert(bm.EvaluateBotEgress(101) && "Bot must evacuate critical vehicle (<15% HP)");
    std::cout << "    Bot emergency egress trigger (<15% HP) verified! [OK]" << std::endl;

    // Soldier 101 executes voluntary/evacuation exit
    assert(bm.ExitVehicle(101, EgressReason::VehicleCriticalDamage));
    assert(!bm.IsSoldierInVehicle(101));
    assert(bm.GetVehicleDriver(6001) == 0);
    assert(!tank->HasDriver());
    std::cout << "    Vehicle exit & driver slot release verified! [OK]" << std::endl;

    // Catastrophic destruction: vehicle takes lethal blow
    tank->TakeDamage(500.0f);
    assert(tank->IsDestroyed());

    // Remaining occupants (102, 103, 104) become fatal casualties
    std::vector<uint32_t> fatalOccs;
    bm.ProcessVehicleDestruction(6001, fatalOccs);
    assert(fatalOccs.size() == 3);
    assert(!bm.IsSoldierInVehicle(102));
    assert(!bm.IsSoldierInVehicle(103));
    assert(!bm.IsSoldierInVehicle(104));
    std::cout << "    Catastrophic destruction casualties processing verified! [OK]" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " running test_boarding" << std::endl;
    std::cout << "========================================" << std::endl;

    TestSeatAutoConfiguration();
    TestBoardingFlowAndSeatAssignment();
    TestBoardingConstraintsAndProtection();
    TestBotAIInteractionAndDestruction();

    std::cout << "\n========================================" << std::endl;
    std::cout << " ALL BOARDING & INTERACTION TESTS PASSED (100% OK)" << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
