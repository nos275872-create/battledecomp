#include "systems/vehicles.h"
#include <iostream>
#include <algorithm>

namespace battledecomp::systems {

namespace {

const char* s_VehicleBehaviorModeStrings[] = {
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

const char* s_VehicleEventStrings[] = {
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

VehicleSize ParseVehicleSize(const std::string& sz) {
    if (sz == "small") return VehicleSize::Small;
    if (sz == "medium") return VehicleSize::Medium;
    if (sz == "large") return VehicleSize::Large;
    if (sz == "very_large" || sz == "huge") return VehicleSize::VeryLarge;
    return VehicleSize::Medium;
}

} // anonymous namespace

// -------------------------------------------------------------
// VehicleAIController
// -------------------------------------------------------------

VehicleAIController::VehicleAIController() {
    Reset();
}

void VehicleAIController::Reset() {
    m_mode = VehicleBehaviorMode::Unset;
    m_navAction = VehicleNavigationAction::Unset;
    m_maneuverAction = VehicleManeuverAction::Unset;
    m_flags = 0;
    m_targetThreatId = 0;
}

void VehicleAIController::SetBehaviorMode(VehicleBehaviorMode mode) {
    m_mode = mode;
    ClearSubAction();
}

const char* VehicleAIController::GetBehaviorModeString() const {
    return VehicleManager::GetBehaviorModeString(m_mode);
}

void VehicleAIController::ClearSubAction() {
    // Reconstructs FUN_0014cdf0: if subaction != 0, abort and clear
    m_navAction = VehicleNavigationAction::Unset;
    m_maneuverAction = VehicleManeuverAction::Unset;
}

void VehicleAIController::Update(float /*deltaTime*/) {
    // Reconstructs FUN_0014cd68: threat verification and state transition
    if (m_targetThreatId != 0) {
        // If target is engaged, proceed with attack maneuver
        if (m_mode == VehicleBehaviorMode::GeneralCombat ||
            m_mode == VehicleBehaviorMode::AttackShip ||
            m_mode == VehicleBehaviorMode::AttackTarget) {
            m_maneuverAction = VehicleManeuverAction::Attack;
            m_navAction = VehicleNavigationAction::PointingAtThreat;
        }
    }
}

// -------------------------------------------------------------
// VehicleInstance
// -------------------------------------------------------------

VehicleInstance::VehicleInstance(uint32_t id, const VehicleBlueprintStats& stats)
    : m_id(id), m_stats(stats), m_currentHealth(stats.maxHealth), m_turrets(stats.turrets) {
}

void VehicleInstance::TakeDamage(float amount) {
    if (amount <= 0.0f || !IsAlive()) return;
    m_currentHealth = std::max(0.0f, m_currentHealth - amount);
}

void VehicleInstance::Repair(float amount) {
    if (amount <= 0.0f || !IsAlive()) return;
    m_currentHealth = std::min(m_stats.maxHealth, m_currentHealth + amount);
}

void VehicleInstance::Update(float deltaTime) {
    if (!IsAlive()) return;

    // Apply auto-repair rate if damaged
    if (m_stats.autoRepairRate > 0.0f && m_currentHealth < m_stats.maxHealth) {
        Repair(m_stats.autoRepairRate * deltaTime);
    }

    m_aiController.Update(deltaTime);
}

bool VehicleInstance::SetDriver(uint32_t actorId) {
    if (!m_stats.boardable || actorId == 0) return false;
    m_driverId = actorId;
    return true;
}

void VehicleInstance::RemoveDriver() {
    m_driverId = 0;
}

MountedTurret* VehicleInstance::GetTurret(size_t index) {
    if (index < m_turrets.size()) {
        return &m_turrets[index];
    }
    return nullptr;
}

// -------------------------------------------------------------
// VehicleManager
// -------------------------------------------------------------

VehicleManager& VehicleManager::GetInstance() {
    static VehicleManager s_instance;
    return s_instance;
}

const char* VehicleManager::GetBehaviorModeString(VehicleBehaviorMode mode) {
    uint32_t idx = static_cast<uint32_t>(mode);
    if (idx < static_cast<uint32_t>(VehicleBehaviorMode::Count)) {
        return s_VehicleBehaviorModeStrings[idx];
    }
    return "Unknown";
}

const char* VehicleManager::GetEventString(VehicleEvent event) {
    uint32_t idx = static_cast<uint32_t>(event);
    if (idx < sizeof(s_VehicleEventStrings) / sizeof(s_VehicleEventStrings[0])) {
        return s_VehicleEventStrings[idx];
    }
    return "unknown event";
}

bool VehicleManager::LoadFromCommonArchive(const std::string& commonAsrPath) {
    core::AsuraBlueprintArchive archive;
    if (!archive.LoadFromFile(commonAsrPath)) {
        RegisterBuiltinFallbacks();
        return false;
    }
    return LoadFromBlueprintArchive(archive);
}

bool VehicleManager::LoadFromBlueprintArchive(const core::AsuraBlueprintArchive& archive) {
    m_blueprintStats.clear();
    m_allBlueprints.clear();

    const core::Blueprint* bpTank = archive.FindBlueprint("Tank");
    const core::Blueprint* bpTurret = archive.FindBlueprint("Turret");
    const core::Blueprint* bpFlyer = archive.FindBlueprint("Flyer");
    const core::Blueprint* bpWalker = archive.FindBlueprint("Walker");

    if (bpTank) ParseCategoryBlueprints(bpTank, VehicleCategory::Tank);
    if (bpTurret) ParseCategoryBlueprints(bpTurret, VehicleCategory::Turret);
    if (bpFlyer) ParseCategoryBlueprints(bpFlyer, VehicleCategory::Flyer);
    if (bpWalker) ParseCategoryBlueprints(bpWalker, VehicleCategory::Walker);

    // Apply template inheritance fallback for stats where child templates don't specify health/speeds
    for (auto& stats : m_allBlueprints) {
        if (stats.maxHealth <= 1.0f) {
            std::string parent;
            if (stats.category == VehicleCategory::Flyer) {
                if (stats.name.find("Fighter") != std::string::npos || stats.name == "X_Wing" || stats.name == "Tie_Fighter") parent = "Fighters";
                else if (stats.name.find("Interceptor") != std::string::npos || stats.name == "A_Wing") parent = "Interceptors";
                else if (stats.name.find("Bomber") != std::string::npos || stats.name == "Y_Wing") parent = "Bombers";
                else if (stats.name.find("Transport") != std::string::npos || stats.name == "LAAT_REP_and_ALL") parent = "Transports";
                else parent = "Flyer:Base";
            } else if (stats.category == VehicleCategory::Tank) {
                if (stats.name.find("speeder") != std::string::npos || stats.name.find("STAP") != std::string::npos || stats.name.find("Speeder") != std::string::npos) {
                    parent = "SpeederBikeBase";
                } else if (stats.name.find("tank") != std::string::npos || stats.name.find("Tank") != std::string::npos || stats.name.find("Juggernaut") != std::string::npos) {
                    parent = (stats.name == "T4B_tank" || stats.name == "AAT_Tank") ? "TrackTankBase" : "HoverTankBase";
                }
            } else if (stats.category == VehicleCategory::Turret) {
                parent = "TurretBase";
            }

            if (!parent.empty()) {
                const auto* pStats = FindBlueprintStats(parent);
                if (pStats) {
                    if (stats.maxHealth <= 1.0f) stats.maxHealth = pStats->maxHealth;
                    if (stats.autoRepairRate <= 0.0f) stats.autoRepairRate = pStats->autoRepairRate;
                    if (stats.topSpeed <= 0.0f) stats.topSpeed = pStats->topSpeed;
                    if (stats.cruisingSpeed <= 0.0f) stats.cruisingSpeed = pStats->cruisingSpeed;
                    if (stats.boostAcc <= 0.0f) stats.boostAcc = pStats->boostAcc;
                    if (stats.maxAcc <= 0.0f) stats.maxAcc = pStats->maxAcc;
                }
            }
        }
    }

    // Refresh m_blueprintStats with updated inherited values
    for (const auto& stats : m_allBlueprints) {
        std::string catPrefix;
        switch (stats.category) {
            case VehicleCategory::Tank: catPrefix = "Tank:"; break;
            case VehicleCategory::Turret: catPrefix = "Turret:"; break;
            case VehicleCategory::Flyer: catPrefix = "Flyer:"; break;
            case VehicleCategory::Walker: catPrefix = "Walker:"; break;
            default: break;
        }
        m_blueprintStats[catPrefix + stats.name] = stats;
        m_blueprintStats[stats.name] = stats;
    }

    return !m_allBlueprints.empty();
}

void VehicleManager::ParseCategoryBlueprints(const core::Blueprint* bp, VehicleCategory cat) {
    if (!bp) return;

    for (const auto& [propName, prop] : bp->properties) {
        VehicleBlueprintStats stats;
        stats.name = propName;
        stats.category = cat;

        stats.maxHealth = prop.GetFloat("Health", 0.0f);
        stats.autoRepairRate = prop.GetFloat("Auto_Repair_Rate", 0.0f);
        stats.deathRate = prop.GetFloat("DeathRate", 0.0f);

        stats.cruisingSpeed = prop.GetFloat("CruisingSpeed", 0.0f);
        stats.topSpeed = prop.GetFloat("TopSpeed", 0.0f);
        stats.maxAcc = prop.GetFloat("MaxAcc", 0.0f);
        stats.boostAcc = prop.GetFloat("BoostAcc", 0.0f);
        stats.stallSpeed = prop.GetFloat("StallSpeed", 0.0f);

        stats.maxTurnYaw = prop.GetFloat("MaxTurnYaw", 0.0f);
        stats.maxTurnPitch = prop.GetFloat("MaxTurnPitch", 0.0f);
        stats.maxTurnRoll = prop.GetFloat("MaxTurnRoll", 0.0f);

        stats.boardable = prop.GetBool("Boardable", true);
        stats.enclosed = prop.GetBool("Enclosed", true);
        stats.carriesCommandPost = prop.GetBool("CarriesCommandPost", false);
        stats.cmdPostName = prop.GetString("CmdPostName", "");

        stats.driverBlueprint = prop.GetString("Driver", "");
        stats.size = ParseVehicleSize(prop.GetString("vehicle_size", "medium"));
        stats.faction = static_cast<VehicleFaction>(prop.GetInt("Faction", 4));

        // Parse weapon hardpoints (HardpointW1, HardpointW2)
        for (const auto& slot : {"HardpointW1", "HardpointW2"}) {
            std::string weaponName = prop.GetString(slot, "");
            if (!weaponName.empty()) {
                VehicleHardpoint hp;
                hp.slotName = slot;
                hp.weaponBlueprint = weaponName;

                for (int m = 1; m <= 8; ++m) {
                    std::string mKey = std::string(slot) + "Marker" + std::to_string(m);
                    std::string marker = prop.GetString(mKey, "");
                    if (!marker.empty()) hp.markers.push_back(marker);

                    std::string tKey = std::string(slot) + "Time" + std::to_string(m);
                    const auto* tEl = prop.FindElement(tKey);
                    if (tEl && !tEl->values.empty()) {
                        hp.fireTimes.push_back(tEl->values[0].AsFloat());
                    }
                }
                stats.hardpoints.push_back(hp);
            }
        }

        // Parse mounted turrets (Turret0, Turret1, Turret2, Turret3, Turret4)
        for (int t = 0; t <= 4; ++t) {
            std::string tKey = "Turret" + std::to_string(t);
            std::string turretName = prop.GetString(tKey, "");
            if (!turretName.empty()) {
                MountedTurret mt;
                mt.slotName = tKey;
                mt.turretBlueprint = turretName;
                mt.markerName = prop.GetString("TurretMarker" + std::to_string(t), "tur");
                stats.turrets.push_back(mt);
            }
        }

        m_allBlueprints.push_back(stats);

        std::string catPrefix;
        switch (cat) {
            case VehicleCategory::Tank: catPrefix = "Tank:"; break;
            case VehicleCategory::Turret: catPrefix = "Turret:"; break;
            case VehicleCategory::Flyer: catPrefix = "Flyer:"; break;
            case VehicleCategory::Walker: catPrefix = "Walker:"; break;
            default: break;
        }
        m_blueprintStats[catPrefix + propName] = stats;
        if (m_blueprintStats.find(propName) == m_blueprintStats.end()) {
            m_blueprintStats[propName] = stats;
        }
    }
}

void VehicleManager::RegisterBuiltinFallbacks() {
    m_blueprintStats.clear();
    m_allBlueprints.clear();

    // Fallback baseline for AT_AT
    VehicleBlueprintStats atat;
    atat.name = "AT_AT";
    atat.category = VehicleCategory::Walker;
    atat.maxHealth = 3500.0f;
    atat.size = VehicleSize::VeryLarge;
    m_allBlueprints.push_back(atat);
    m_blueprintStats[atat.name] = atat;
    m_blueprintStats["Walker:AT_AT"] = atat;

    // Fallback baseline for AT_TE
    VehicleBlueprintStats atte;
    atte.name = "AT_TE";
    atte.category = VehicleCategory::Walker;
    atte.maxHealth = 2500.0f;
    atte.size = VehicleSize::VeryLarge;
    m_allBlueprints.push_back(atte);
    m_blueprintStats[atte.name] = atte;
    m_blueprintStats["Walker:AT_TE"] = atte;

    // Fallback baseline for X_Wing
    VehicleBlueprintStats xwing;
    xwing.name = "X_Wing";
    xwing.category = VehicleCategory::Flyer;
    xwing.maxHealth = 250.0f;
    xwing.topSpeed = 175.0f;
    xwing.cruisingSpeed = 100.0f;
    m_allBlueprints.push_back(xwing);
    m_blueprintStats[xwing.name] = xwing;
    m_blueprintStats["Flyer:X_Wing"] = xwing;

    // Fallback baseline for IFTX_tank
    VehicleBlueprintStats iftx;
    iftx.name = "IFTX_tank";
    iftx.category = VehicleCategory::Tank;
    iftx.maxHealth = 800.0f;
    iftx.topSpeed = 25.0f;
    m_allBlueprints.push_back(iftx);
    m_blueprintStats[iftx.name] = iftx;
    m_blueprintStats["Tank:IFTX_tank"] = iftx;
}

const VehicleBlueprintStats* VehicleManager::FindBlueprintStats(const std::string& name, VehicleCategory cat) const {
    if (cat != VehicleCategory::Unknown) {
        std::string catPrefix;
        switch (cat) {
            case VehicleCategory::Tank: catPrefix = "Tank:"; break;
            case VehicleCategory::Turret: catPrefix = "Turret:"; break;
            case VehicleCategory::Flyer: catPrefix = "Flyer:"; break;
            case VehicleCategory::Walker: catPrefix = "Walker:"; break;
            default: break;
        }
        auto it = m_blueprintStats.find(catPrefix + name);
        if (it != m_blueprintStats.end()) return &it->second;
    }

    auto it = m_blueprintStats.find(name);
    if (it != m_blueprintStats.end()) return &it->second;
    return nullptr;
}

std::shared_ptr<VehicleInstance> VehicleManager::SpawnVehicle(const std::string& templateName, uint32_t entityId) {
    const auto* stats = FindBlueprintStats(templateName);
    if (!stats) return nullptr;

    uint32_t id = entityId != 0 ? entityId : m_nextEntityId++;
    auto vehicle = std::make_shared<VehicleInstance>(id, *stats);
    m_activeVehicles[id] = vehicle;

    DispatchEvent(VehicleEvent::VehicleCreated, id);
    return vehicle;
}

std::shared_ptr<VehicleInstance> VehicleManager::GetVehicle(uint32_t entityId) {
    auto it = m_activeVehicles.find(entityId);
    if (it != m_activeVehicles.end()) return it->second;
    return nullptr;
}

bool VehicleManager::DestroyVehicle(uint32_t entityId) {
    auto it = m_activeVehicles.find(entityId);
    if (it != m_activeVehicles.end()) {
        it->second->TakeDamage(it->second->GetMaxHealth() * 2.0f);
        DispatchEvent(VehicleEvent::VehicleDestroyed, entityId);
        m_activeVehicles.erase(it);
        return true;
    }
    return false;
}

void VehicleManager::ClearVehicles() {
    m_activeVehicles.clear();
}

void VehicleManager::Update(float deltaTime) {
    for (auto& [id, vehicle] : m_activeVehicles) {
        if (vehicle) {
            vehicle->Update(deltaTime);
        }
    }
}

void VehicleManager::DispatchEvent(VehicleEvent event, uint32_t vehicleId) {
    m_eventLog.emplace_back(event, vehicleId);
}

} // namespace battledecomp::systems
