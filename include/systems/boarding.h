#pragma once

#include "systems/vehicles.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace battledecomp::systems {

enum class VehicleSeatType {
    Driver,
    Gunner,
    Passenger
};

enum class BoardingResult {
    Success,
    VehicleNotFound,
    SoldierNotFound,
    VehicleDestroyed,
    NotBoardable,
    FactionHostile,
    NoSeatsAvailable,
    TooFar
};

enum class EgressReason {
    Voluntary,
    VehicleCriticalDamage,
    VehicleDestroyed,
    ObjectiveReached,
    Evacuate
};

// Represents a physical seat inside or on a vehicle
struct VehicleSeat {
    uint32_t seatIndex{0};
    VehicleSeatType type{VehicleSeatType::Passenger};
    int32_t turretIndex{-1}; // Associated turret slot index (for Gunner), or -1
    uint32_t occupantId{0};   // Soldier ID, or 0 if free
    std::string seatName;

    bool IsFree() const { return occupantId == 0; }
};

// State of an individual soldier regarding vehicles
struct SoldierVehicleState {
    uint32_t soldierId{0};
    bool isInVehicle{false};
    uint32_t vehicleId{0};
    uint32_t seatIndex{0};
    VehicleSeatType seatType{VehicleSeatType::Passenger};
    bool isProtected{true};   // True if enclosed cockpit/hull, false if exposed (open speeder/turret)
    std::string sittingAnim;   // e.g. "Human_STAP_sitpose"
};

// Manager for Soldier-Vehicle Boarding, Seating and Interaction
class BoardingManager {
public:
    static BoardingManager& GetInstance();

    // Seat registration & configuration
    void SetupVehicleSeats(uint32_t vehicleId, const VehicleBlueprintStats& stats);
    void RemoveVehicleSeats(uint32_t vehicleId);

    const std::vector<VehicleSeat>* GetVehicleSeats(uint32_t vehicleId) const;
    std::vector<VehicleSeat>* GetVehicleSeats(uint32_t vehicleId);

    // Boarding queries and operations
    BoardingResult CanBoard(uint32_t soldierId, uint32_t vehicleId, VehicleFaction soldierFaction, float distance) const;
    BoardingResult BoardVehicle(uint32_t soldierId, uint32_t vehicleId, VehicleSeatType preferredSeat,
                               VehicleFaction soldierFaction, float distance);

    bool ExitVehicle(uint32_t soldierId, EgressReason reason);

    // Queries for occupants and soldier status
    bool IsSoldierInVehicle(uint32_t soldierId) const;
    const SoldierVehicleState* GetSoldierState(uint32_t soldierId) const;
    std::vector<uint32_t> GetVehicleOccupants(uint32_t vehicleId) const;
    uint32_t GetVehicleDriver(uint32_t vehicleId) const;

    // Damage & life events handling
    void ProcessVehicleDamage(uint32_t vehicleId, float damageAmount, std::vector<std::pair<uint32_t, float>>& outOccupantDamage);
    void ProcessVehicleDestruction(uint32_t vehicleId, std::vector<uint32_t>& outFatalOccupants);

    // Bot AI vehicle evaluation
    uint32_t EvaluateBotBoarding(uint32_t botId, VehicleFaction botFaction, float botX, float botY, float botZ,
                                 float searchRadius = 35.0f) const;
    bool EvaluateBotEgress(uint32_t botId) const;

    // Maximum distance allowed for boarding an entity (in meters)
    static constexpr float MAX_BOARDING_DISTANCE = 4.0f;
    // Critical health threshold percentage triggering automatic bot evacuation (15%)
    static constexpr float CRITICAL_HEALTH_THRESHOLD = 0.15f;

    void Clear();

private:
    BoardingManager() = default;

    // Map: vehicleId -> vector of seats
    std::unordered_map<uint32_t, std::vector<VehicleSeat>> m_vehicleSeats;
    // Map: soldierId -> soldier boarding state
    std::unordered_map<uint32_t, SoldierVehicleState> m_soldierStates;
};

} // namespace battledecomp::systems
