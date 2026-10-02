#pragma once

#include "core/asura_blueprint.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace battledecomp::systems {

enum class VehicleCategory {
    Tank,
    Turret,
    Flyer,
    Walker,
    Unknown
};

enum class VehicleSize {
    Small,
    Medium,
    Large,
    VeryLarge,
    Unknown
};

enum class VehicleFaction {
    Republic = 0,
    CIS = 1,
    Alliance = 2,
    Empire = 3,
    Neutral = 4
};

// Vehicle AI behavior modes from table 0x0031773C
enum class VehicleBehaviorMode : uint32_t {
    Unset = 0,
    None = 1,
    TakeOff = 2,
    GeneralCombat = 3,
    AttackShip = 4,
    DefendShip = 5,
    FollowShip = 6,
    CaptureFlag = 7,
    AttackTarget = 8,
    LandInHangar = 9,
    GoToPosition = 10,
    Count = 11
};

// Navigation subactions (0x002D6A34)
enum class VehicleNavigationAction : uint32_t {
    Unset = 0,
    MovingToPosition = 1,
    OrientatingToThreat = 2,
    OrientatingToThreatNoRoute = 3,
    PointingAtThreat = 4,
    PointingAtThreatNoRoute = 5,
    Finished = 6
};

// Maneuvering subactions (0x002D6B1C)
enum class VehicleManeuverAction : uint32_t {
    Unset = 0,
    MoveAbout = 1,
    Attack = 2,
    AvoidCollision = 3,
    Finished = 4
};

// Vehicle Order / Commander Events (0x002D9FD8)
enum class VehicleEvent : uint32_t {
    NoEvent = 0,
    VehicleCreated = 1,
    VehicleDestroyed = 2,
    FlagCreated = 3,
    FlagPickedUp = 4,
    ApproachObjectCreated = 5,
    ApproachObjectDestroyed = 6,
    ApproachObjectTeamChanged = 7,
    VehicleOrdersCompleted = 8,
    VehicleOrdersAborted = 9
};

// Represents an integrated weapon mount (HardpointW1, HardpointW2, etc.)
struct VehicleHardpoint {
    std::string slotName;          // e.g. "HardpointW1", "HardpointW2"
    std::string weaponBlueprint;   // e.g. "SpaceLasers_X_Wing", "Concussion_Missile"
    std::vector<std::string> markers; // e.g. "muz01", "muz02", "torp"
    std::vector<float> fireTimes;  // Time interval offsets
};

// Represents a child turret mount (Turret0, Turret1, etc.)
struct MountedTurret {
    std::string slotName;          // e.g. "Turret0", "Turret1"
    std::string turretBlueprint;   // e.g. "AT_TE_turret_01", "IFTX_tank_turret_01_Base"
    std::string markerName;        // e.g. "tur", "tur01"
    float currentHealth{0.0f};
    float maxHealth{0.0f};
    float yaw{0.0f};
    float pitch{0.0f};
    bool active{true};
};

// Static/blueprint stats extracted from COMMON.ASR
struct VehicleBlueprintStats {
    std::string name;
    VehicleCategory category{VehicleCategory::Unknown};
    VehicleSize size{VehicleSize::Medium};
    VehicleFaction faction{VehicleFaction::Neutral};
    std::string driverBlueprint;

    float maxHealth{100.0f};
    float autoRepairRate{0.0f};
    float deathRate{0.0f};

    // Movement & flight dynamics
    float cruisingSpeed{0.0f};
    float topSpeed{0.0f};
    float maxAcc{0.0f};
    float boostAcc{0.0f};
    float stallSpeed{0.0f};

    // Maneuvering rates
    float maxTurnYaw{0.0f};
    float maxTurnPitch{0.0f};
    float maxTurnRoll{0.0f};

    // Configuration flags
    bool boardable{true};
    bool enclosed{true};
    bool carriesCommandPost{false};
    std::string cmdPostName;

    // Attached equipment
    std::vector<VehicleHardpoint> hardpoints;
    std::vector<MountedTurret> turrets;
};

// Vehicle AI Controller (reconstructing 0x0014CC58, 0x0014CD68, 0x0014CDF0, 0x0014CE38, vtable 0x002F5068)
class VehicleAIController {
public:
    VehicleAIController();
    ~VehicleAIController() = default;

    void Reset();
    void SetBehaviorMode(VehicleBehaviorMode mode);
    VehicleBehaviorMode GetBehaviorMode() const { return m_mode; }
    const char* GetBehaviorModeString() const;

    void SetNavigationAction(VehicleNavigationAction action) { m_navAction = action; }
    VehicleNavigationAction GetNavigationAction() const { return m_navAction; }

    void SetManeuverAction(VehicleManeuverAction action) { m_maneuverAction = action; }
    VehicleManeuverAction GetManeuverAction() const { return m_maneuverAction; }

    void ClearSubAction(); // FUN_0014cdf0
    void Update(float deltaTime); // FUN_0014cd68

    // Threat tracking
    void SetTargetThreat(uint32_t threatId) { m_targetThreatId = threatId; }
    uint32_t GetTargetThreat() const { return m_targetThreatId; }

private:
    VehicleBehaviorMode m_mode{VehicleBehaviorMode::Unset};       // 0x1C in EBOOT.BIN
    VehicleNavigationAction m_navAction{VehicleNavigationAction::Unset}; // 0x20 in EBOOT.BIN
    VehicleManeuverAction m_maneuverAction{VehicleManeuverAction::Unset};
    uint8_t m_flags{0};                                           // 0x24 in EBOOT.BIN
    uint32_t m_targetThreatId{0};
};

// Runtime vehicle entity instance
class VehicleInstance {
public:
    VehicleInstance(uint32_t id, const VehicleBlueprintStats& stats);

    uint32_t GetId() const { return m_id; }
    const VehicleBlueprintStats& GetStats() const { return m_stats; }
    const std::string& GetName() const { return m_stats.name; }
    VehicleCategory GetCategory() const { return m_stats.category; }

    float GetCurrentHealth() const { return m_currentHealth; }
    float GetMaxHealth() const { return m_stats.maxHealth; }
    bool IsAlive() const { return m_currentHealth > 0.0f; }
    bool IsDestroyed() const { return m_currentHealth <= 0.0f; }

    void TakeDamage(float amount);
    void Repair(float amount);
    void Update(float deltaTime);

    // Crew management
    bool HasDriver() const { return m_driverId != 0; }
    uint32_t GetDriverId() const { return m_driverId; }
    bool SetDriver(uint32_t actorId);
    void RemoveDriver();

    VehicleAIController& GetAIController() { return m_aiController; }
    const VehicleAIController& GetAIController() const { return m_aiController; }

    const std::vector<MountedTurret>& GetTurrets() const { return m_turrets; }
    MountedTurret* GetTurret(size_t index);

private:
    uint32_t m_id{0};
    VehicleBlueprintStats m_stats;
    float m_currentHealth{0.0f};
    uint32_t m_driverId{0};
    std::vector<MountedTurret> m_turrets;
    VehicleAIController m_aiController;
};

// Manager for Vehicle & Turret Subsystem
class VehicleManager {
public:
    static VehicleManager& GetInstance();

    bool LoadFromBlueprintArchive(const core::AsuraBlueprintArchive& archive);
    bool LoadFromCommonArchive(const std::string& commonAsrPath = "orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR");

    const VehicleBlueprintStats* FindBlueprintStats(const std::string& name, VehicleCategory cat = VehicleCategory::Unknown) const;
    const std::vector<VehicleBlueprintStats>& GetAllBlueprints() const {
        return m_allBlueprints;
    }

    // Instantiation and entity management
    std::shared_ptr<VehicleInstance> SpawnVehicle(const std::string& templateName, uint32_t entityId = 0);
    std::shared_ptr<VehicleInstance> GetVehicle(uint32_t entityId);
    bool DestroyVehicle(uint32_t entityId);
    void ClearVehicles();

    void Update(float deltaTime);

    // Event system
    void DispatchEvent(VehicleEvent event, uint32_t vehicleId);
    const std::vector<std::pair<VehicleEvent, uint32_t>>& GetRecentEvents() const {
        return m_eventLog;
    }
    void ClearEvents() { m_eventLog.clear(); }

    static const char* GetBehaviorModeString(VehicleBehaviorMode mode);
    static const char* GetEventString(VehicleEvent event);

private:
    VehicleManager() = default;

    void RegisterBuiltinFallbacks();
    void ParseCategoryBlueprints(const core::Blueprint* bp, VehicleCategory cat);

    std::vector<VehicleBlueprintStats> m_allBlueprints;
    std::unordered_map<std::string, VehicleBlueprintStats> m_blueprintStats;
    std::unordered_map<uint32_t, std::shared_ptr<VehicleInstance>> m_activeVehicles;
    std::vector<std::pair<VehicleEvent, uint32_t>> m_eventLog;
    uint32_t m_nextEntityId{1000};
};

} // namespace battledecomp::systems
