#include "systems/boarding.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace battledecomp::systems {

BoardingManager& BoardingManager::GetInstance() {
    static BoardingManager s_instance;
    return s_instance;
}

void BoardingManager::Clear() {
    m_vehicleSeats.clear();
    m_soldierStates.clear();
}

void BoardingManager::SetupVehicleSeats(uint32_t vehicleId, const VehicleBlueprintStats& stats) {
    auto& seats = m_vehicleSeats[vehicleId];
    seats.clear();

    if (!stats.boardable) {
        return;
    }

    uint32_t seatIdx = 0;

    // Seat 0: Driver / Pilot
    VehicleSeat driverSeat;
    driverSeat.seatIndex = seatIdx++;
    driverSeat.type = VehicleSeatType::Driver;
    driverSeat.turretIndex = -1;
    driverSeat.occupantId = 0;
    driverSeat.seatName = "driver";
    seats.push_back(driverSeat);

    // Gunner seats: one per mounted turret
    for (size_t t = 0; t < stats.turrets.size(); ++t) {
        VehicleSeat gunnerSeat;
        gunnerSeat.seatIndex = seatIdx++;
        gunnerSeat.type = VehicleSeatType::Gunner;
        gunnerSeat.turretIndex = static_cast<int32_t>(t);
        gunnerSeat.occupantId = 0;
        gunnerSeat.seatName = "gunner_" + std::to_string(t);
        seats.push_back(gunnerSeat);
    }

    // Passenger seats based on vehicle size & archetype
    uint32_t passengerCount = 0;
    if (stats.name.find("Transport") != std::string::npos ||
        stats.name.find("LAAT") != std::string::npos ||
        stats.name == "Rebel_Transport" ||
        stats.name == "Imperial_Troop_Transporter") {
        passengerCount = 6;
    } else if (stats.name == "AT_TE" || stats.name.find("Juggernaut") != std::string::npos) {
        passengerCount = 4;
    } else if (stats.size == VehicleSize::Large || stats.size == VehicleSize::VeryLarge) {
        passengerCount = 2;
    }

    for (uint32_t p = 0; p < passengerCount; ++p) {
        VehicleSeat passSeat;
        passSeat.seatIndex = seatIdx++;
        passSeat.type = VehicleSeatType::Passenger;
        passSeat.turretIndex = -1;
        passSeat.occupantId = 0;
        passSeat.seatName = "passenger_" + std::to_string(p);
        seats.push_back(passSeat);
    }
}

void BoardingManager::RemoveVehicleSeats(uint32_t vehicleId) {
    // Eject all occupants first
    auto it = m_vehicleSeats.find(vehicleId);
    if (it != m_vehicleSeats.end()) {
        for (const auto& seat : it->second) {
            if (seat.occupantId != 0) {
                m_soldierStates.erase(seat.occupantId);
            }
        }
        m_vehicleSeats.erase(it);
    }
}

const std::vector<VehicleSeat>* BoardingManager::GetVehicleSeats(uint32_t vehicleId) const {
    auto it = m_vehicleSeats.find(vehicleId);
    if (it != m_vehicleSeats.end()) return &it->second;
    return nullptr;
}

std::vector<VehicleSeat>* BoardingManager::GetVehicleSeats(uint32_t vehicleId) {
    auto it = m_vehicleSeats.find(vehicleId);
    if (it != m_vehicleSeats.end()) return &it->second;
    return nullptr;
}

BoardingResult BoardingManager::CanBoard(uint32_t soldierId, uint32_t vehicleId,
                                        VehicleFaction soldierFaction, float distance) const {
    if (IsSoldierInVehicle(soldierId)) {
        return BoardingResult::NoSeatsAvailable;
    }

    auto vehicle = VehicleManager::GetInstance().GetVehicle(vehicleId);
    if (!vehicle) {
        return BoardingResult::VehicleNotFound;
    }

    if (vehicle->IsDestroyed()) {
        return BoardingResult::VehicleDestroyed;
    }

    const auto& stats = vehicle->GetStats();
    if (!stats.boardable) {
        return BoardingResult::NotBoardable;
    }

    if (distance > MAX_BOARDING_DISTANCE) {
        return BoardingResult::TooFar;
    }

    // Check faction hostility
    uint32_t currentDriver = GetVehicleDriver(vehicleId);
    if (currentDriver != 0) {
        // If controlled by a soldier of hostile faction, board rejected
        auto driverState = GetSoldierState(currentDriver);
        if (driverState) {
            // Unoccupied vehicles can be commandeered, occupied ones check friendly
            if (stats.faction != VehicleFaction::Neutral && stats.faction != soldierFaction) {
                return BoardingResult::FactionHostile;
            }
        }
    }

    // Check available seats
    const auto* seats = GetVehicleSeats(vehicleId);
    if (!seats || seats->empty()) {
        return BoardingResult::NoSeatsAvailable;
    }

    bool hasFreeSeat = false;
    for (const auto& s : *seats) {
        if (s.IsFree()) {
            hasFreeSeat = true;
            break;
        }
    }

    if (!hasFreeSeat) {
        return BoardingResult::NoSeatsAvailable;
    }

    return BoardingResult::Success;
}

BoardingResult BoardingManager::BoardVehicle(uint32_t soldierId, uint32_t vehicleId,
                                            VehicleSeatType preferredSeat,
                                            VehicleFaction soldierFaction, float distance) {
    BoardingResult check = CanBoard(soldierId, vehicleId, soldierFaction, distance);
    if (check != BoardingResult::Success) {
        return check;
    }

    auto* seats = GetVehicleSeats(vehicleId);
    if (!seats) {
        return BoardingResult::NoSeatsAvailable;
    }

    auto vehicle = VehicleManager::GetInstance().GetVehicle(vehicleId);
    if (!vehicle) {
        return BoardingResult::VehicleNotFound;
    }

    // Find requested seat, or fallback to Driver -> Gunner -> Passenger
    VehicleSeat* targetSeat = nullptr;
    for (auto& s : *seats) {
        if (s.type == preferredSeat && s.IsFree()) {
            targetSeat = &s;
            break;
        }
    }

    if (!targetSeat) {
        // Priority fallback: Driver first
        for (auto& s : *seats) {
            if (s.type == VehicleSeatType::Driver && s.IsFree()) {
                targetSeat = &s;
                break;
            }
        }
    }
    if (!targetSeat) {
        // Gunner second
        for (auto& s : *seats) {
            if (s.type == VehicleSeatType::Gunner && s.IsFree()) {
                targetSeat = &s;
                break;
            }
        }
    }
    if (!targetSeat) {
        // Passenger third
        for (auto& s : *seats) {
            if (s.IsFree()) {
                targetSeat = &s;
                break;
            }
        }
    }

    if (!targetSeat) {
        return BoardingResult::NoSeatsAvailable;
    }

    // Assign occupant to physical seat
    targetSeat->occupantId = soldierId;

    if (targetSeat->type == VehicleSeatType::Driver) {
        vehicle->SetDriver(soldierId);
    } else if (targetSeat->type == VehicleSeatType::Gunner && targetSeat->turretIndex >= 0) {
        auto* turret = vehicle->GetTurret(targetSeat->turretIndex);
        if (turret) turret->active = true;
    }

    // Track soldier state
    SoldierVehicleState sState;
    sState.soldierId = soldierId;
    sState.isInVehicle = true;
    sState.vehicleId = vehicleId;
    sState.seatIndex = targetSeat->seatIndex;
    sState.seatType = targetSeat->type;
    sState.isProtected = vehicle->GetStats().enclosed;
    sState.sittingAnim = vehicle->GetStats().enclosed ? "" : "Human_STAP_sitpose";

    m_soldierStates[soldierId] = sState;

    return BoardingResult::Success;
}

bool BoardingManager::ExitVehicle(uint32_t soldierId, EgressReason /*reason*/) {
    auto it = m_soldierStates.find(soldierId);
    if (it == m_soldierStates.end() || !it->second.isInVehicle) {
        return false;
    }

    uint32_t vId = it->second.vehicleId;
    uint32_t sIdx = it->second.seatIndex;

    auto* seats = GetVehicleSeats(vId);
    if (seats && sIdx < seats->size()) {
        auto& seat = (*seats)[sIdx];
        if (seat.occupantId == soldierId) {
            seat.occupantId = 0;
            if (seat.type == VehicleSeatType::Driver) {
                auto v = VehicleManager::GetInstance().GetVehicle(vId);
                if (v) v->RemoveDriver();
            }
        }
    }

    m_soldierStates.erase(it);
    return true;
}

bool BoardingManager::IsSoldierInVehicle(uint32_t soldierId) const {
    auto it = m_soldierStates.find(soldierId);
    return it != m_soldierStates.end() && it->second.isInVehicle;
}

const SoldierVehicleState* BoardingManager::GetSoldierState(uint32_t soldierId) const {
    auto it = m_soldierStates.find(soldierId);
    if (it != m_soldierStates.end()) return &it->second;
    return nullptr;
}

std::vector<uint32_t> BoardingManager::GetVehicleOccupants(uint32_t vehicleId) const {
    std::vector<uint32_t> occs;
    const auto* seats = GetVehicleSeats(vehicleId);
    if (seats) {
        for (const auto& s : *seats) {
            if (!s.IsFree()) {
                occs.push_back(s.occupantId);
            }
        }
    }
    return occs;
}

uint32_t BoardingManager::GetVehicleDriver(uint32_t vehicleId) const {
    const auto* seats = GetVehicleSeats(vehicleId);
    if (seats) {
        for (const auto& s : *seats) {
            if (s.type == VehicleSeatType::Driver && !s.IsFree()) {
                return s.occupantId;
            }
        }
    }
    return 0;
}

void BoardingManager::ProcessVehicleDamage(uint32_t vehicleId, float damageAmount,
                                         std::vector<std::pair<uint32_t, float>>& outOccupantDamage) {
    outOccupantDamage.clear();
    auto vehicle = VehicleManager::GetInstance().GetVehicle(vehicleId);
    if (!vehicle) return;

    bool enclosed = vehicle->GetStats().enclosed;
    const auto* seats = GetVehicleSeats(vehicleId);
    if (!seats) return;

    for (const auto& s : *seats) {
        if (!s.IsFree()) {
            if (!enclosed) {
                // Exposed occupant takes splash/penetration damage
                float occDmg = damageAmount * 0.25f;
                outOccupantDamage.emplace_back(s.occupantId, occDmg);
            }
        }
    }
}

void BoardingManager::ProcessVehicleDestruction(uint32_t vehicleId, std::vector<uint32_t>& outFatalOccupants) {
    outFatalOccupants.clear();
    const auto* seats = GetVehicleSeats(vehicleId);
    if (!seats) return;

    for (const auto& s : *seats) {
        if (!s.IsFree()) {
            outFatalOccupants.push_back(s.occupantId);
            m_soldierStates.erase(s.occupantId);
        }
    }

    m_vehicleSeats.erase(vehicleId);
}

uint32_t BoardingManager::EvaluateBotBoarding(uint32_t botId, VehicleFaction botFaction,
                                             float botX, float botY, float botZ,
                                             float searchRadius) const {
    if (IsSoldierInVehicle(botId)) return 0;

    auto& vm = VehicleManager::GetInstance();
    uint32_t bestVehicleId = 0;
    float closestDistSq = searchRadius * searchRadius;


    // Inspect active vehicles
    for (uint32_t candidateId = 1000; candidateId < 2000; ++candidateId) {
        auto v = vm.GetVehicle(candidateId);
        if (!v || !v->IsAlive() || !v->GetStats().boardable) continue;

        // Faction check: same faction or neutral
        if (v->GetStats().faction != VehicleFaction::Neutral &&
            v->GetStats().faction != botFaction) {
            continue;
        }

        // Check if there is an empty seat
        const auto* seats = GetVehicleSeats(candidateId);
        if (!seats) continue;

        bool hasFree = false;
        for (const auto& s : *seats) {
            if (s.IsFree()) {
                hasFree = true;
                break;
            }
        }
        if (!hasFree) continue;

        // Simple distance proxy calculation
        float dx = botX - 0.0f; // Mock vehicle world pos
        float dy = botY - 0.0f;
        float dz = botZ - 0.0f;
        float distSq = dx*dx + dy*dy + dz*dz;

        if (distSq <= closestDistSq) {
            closestDistSq = distSq;
            bestVehicleId = candidateId;
        }
    }

    return bestVehicleId;
}

bool BoardingManager::EvaluateBotEgress(uint32_t botId) const {
    const auto* state = GetSoldierState(botId);
    if (!state || !state->isInVehicle) return false;

    auto v = VehicleManager::GetInstance().GetVehicle(state->vehicleId);
    if (!v || v->IsDestroyed()) return true;

    float ratio = v->GetCurrentHealth() / v->GetMaxHealth();
    return ratio <= CRITICAL_HEALTH_THRESHOLD;
}

} // namespace battledecomp::systems
