#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <functional>

namespace battledecomp::ai {

// High-level Tactical AI Modes (Evaluated at BotActor + 0x4C)
enum class BehaviorMode : uint32_t {
    Unknown            = 0,
    HunterSeeker       = 1008, // 0x3F0 ("Hunter Seeker")
    AttackCpVariant    = 1010, // 0x3F2 ("Attack : CP Variant" - Command Post Attack, Default)
    AttackCtfVariant   = 1011, // 0x3F3 ("Attack : CTF Variant")
    AttackCtfVariantB  = 1012, // 0x3F4 ("Attack : CTF Variant-b")
    DefendCommandPost  = 1013, // 0x3F5 ("Defend : Command Post")
    StandardAttack     = 1014, // 0x3F6 ("Standard Attack")
    StandardDefend     = 1015, // 0x3F7 ("Standard Defend")
    Withdrawal         = 1016, // 0x3F8 ("Withdrawl")
    Hero               = 1017  // 0x3F9 ("Hero" - Jedi / Sith Hero AI)
};

// Soldier / Lightsaber Action Sub-States (Table at 0x003175B0 / 0x00317640)
enum class SoldierSubAction : uint32_t {
    Unset                           = 0,
    RunningRandomly                 = 1,
    RunningToThreat                 = 2,
    IncomingLightsaberThrow         = 3,
    AtThreat                        = 4,
    MovingRandomly                  = 5,
    PerformingForceMove             = 6,
    MovingForLightsaberAttack       = 7,
    PerformingLightsaberAttack      = 8,
    MovingAwayAfterLightsaberAttack = 9,
    Finished                        = 10
};

struct Vector3 {
    float x;
    float y;
    float z;

    bool operator==(const Vector3& o) const {
        return x == o.x && y == o.y && z == o.z;
    }
};

class BotAIController {
public:
    static constexpr size_t kMaxThreats = 5;
    static constexpr uint32_t kNullTargetGuid = 999;

    BotAIController();
    virtual ~BotAIController();

    // Reconstructed per-frame update loop (0x00188D58)
    void Update();

    // Behavior Modes
    BehaviorMode GetBehaviorMode() const { return m_behaviorMode; }
    void SetBehaviorMode(BehaviorMode mode) { m_behaviorMode = mode; }
    static const char* GetBehaviorModeName(BehaviorMode mode);

    // Sub-actions
    static const char* GetSoldierSubActionName(SoldierSubAction action);

    // Target tracking (0x00189284)
    uint32_t GetCurrentTargetGuid() const { return m_currentTargetGuid; }
    void SetCurrentTargetGuid(uint32_t guid) { m_currentTargetGuid = guid; }
    const Vector3& GetTargetPosition() const { return m_targetPosition; }
    float GetTargetAngle() const { return m_targetAngle; }
    void SetTargetTracking(const Vector3& pos, float angle);

    // Home / Patrol base position
    const Vector3& GetHomePosition() const { return m_homePosition; }
    void SetHomePosition(const Vector3& pos) { m_homePosition = pos; }

    // Threat Management (0x00188DB8 & 0x00189854)
    size_t GetThreatCount() const { return m_threatCount; }
    uint32_t GetThreat(size_t index) const;
    bool AddThreat(uint32_t guid);
    bool RemoveThreat(uint32_t guid); // 0x00189854
    void FilterThreats(const std::function<bool(uint32_t)>& isAlivePredicate); // 0x00188DB8

    bool IsInitialized() const { return m_initialized; }

private:
    bool m_initialized;                       // +0x28
    uint32_t m_currentTargetGuid;             // +0x3C
    Vector3 m_homePosition;                   // +0x40
    BehaviorMode m_behaviorMode;              // +0x4C
    float m_targetAngle;                      // +0x54
    Vector3 m_targetPosition;                 // +0x58
    std::array<uint32_t, kMaxThreats> m_threats; // +0x98
    uint32_t m_threatCount;                   // +0xAC
};

} // namespace battledecomp::ai
