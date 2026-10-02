#include "ai/bot_ai.h"
#include <algorithm>

namespace battledecomp::ai {

namespace {

const char* const kSoldierSubActionStrings[] = {
    "UNSET",
    "RUNNING_RANDOMLY",
    "RUNNING_TO_THREAT",
    "INCOMING_LIGHTSABER_THROW",
    "AT_THREAT",
    "MOVING_RANDOMLY",
    "PERFORMING_FORCE_MOVE",
    "MOVING_FOR_LIGHTSABER_ATTACK",
    "PERFORMING_LIGHTSABER_ATTACK",
    "MOVING_AWAY_AFTER_LIGHTSABER_ATTACK",
    "FINISHED"
};

} // anonymous namespace

BotAIController::BotAIController()
    : m_initialized(false),
      m_currentTargetGuid(kNullTargetGuid),
      m_homePosition{0.0f, 0.0f, 0.0f},
      m_behaviorMode(BehaviorMode::AttackCpVariant), // 0x00188A44: param_1[0x13] = 0x3f2 (1010)
      m_targetAngle(0.0f),
      m_targetPosition{0.0f, 0.0f, 0.0f},
      m_threatCount(0) {
    // 0x00188A44: Loop 5 times setting threats to 999 (null target)
    m_threats.fill(kNullTargetGuid);
}

BotAIController::~BotAIController() = default;

void BotAIController::Update() {
    // 0x00188D58: func_00188d58
    if (!m_initialized) {
        m_initialized = true;
    }
}

const char* BotAIController::GetBehaviorModeName(BehaviorMode mode) {
    // 0x00188EF4: func_00188ef4 (Switch at lines 55-85)
    switch (mode) {
        case BehaviorMode::HunterSeeker:      return "Hunter Seeker";
        case BehaviorMode::AttackCpVariant:   return "Attack : CP Variant";
        case BehaviorMode::AttackCtfVariant:  return "Attack : CTF Variant";
        case BehaviorMode::AttackCtfVariantB: return "Attack : CTF Variant-b";
        case BehaviorMode::DefendCommandPost: return "Defend : Command Post";
        case BehaviorMode::StandardAttack:    return "Standard Attack";
        case BehaviorMode::StandardDefend:    return "Standard Defend";
        case BehaviorMode::Withdrawal:        return "Withdrawl"; // Preserved engine typo
        case BehaviorMode::Hero:              return "Hero";
        default:                              return "Unknown behaviour";
    }
}

const char* BotAIController::GetSoldierSubActionName(SoldierSubAction action) {
    auto idx = static_cast<size_t>(action);
    if (idx < sizeof(kSoldierSubActionStrings) / sizeof(kSoldierSubActionStrings[0])) {
        return kSoldierSubActionStrings[idx];
    }
    return "UNKNOWN_ACTION";
}

void BotAIController::SetTargetTracking(const Vector3& pos, float angle) {
    // 0x00189284: FUN_00189284
    m_targetPosition = pos;
    m_targetAngle = angle;
}

uint32_t BotAIController::GetThreat(size_t index) const {
    if (index < m_threatCount) {
        return m_threats[index];
    }
    return kNullTargetGuid;
}

bool BotAIController::AddThreat(uint32_t guid) {
    if (guid == kNullTargetGuid || m_threatCount >= kMaxThreats) {
        return false;
    }
    for (size_t i = 0; i < m_threatCount; ++i) {
        if (m_threats[i] == guid) {
            return false; // Already present
        }
    }
    m_threats[m_threatCount++] = guid;
    return true;
}

bool BotAIController::RemoveThreat(uint32_t guid) {
    // 0x00189854: FUN_00189854
    bool found = false;
    for (size_t i = 0; i < m_threatCount; ++i) {
        if (!found && m_threats[i] == guid) {
            found = true;
        }
        if (found) {
            if (i + 1 < kMaxThreats) {
                m_threats[i] = m_threats[i + 1];
            } else {
                m_threats[i] = kNullTargetGuid;
            }
        }
    }

    if (found) {
        m_threatCount--;
        m_threats[m_threatCount] = kNullTargetGuid;
        return true;
    }
    return false;
}

void BotAIController::FilterThreats(const std::function<bool(uint32_t)>& isAlivePredicate) {
    // 0x00188DB8: FUN_00188db8
    size_t i = 0;
    while (i < m_threatCount) {
        uint32_t guid = m_threats[i];
        if (guid == kNullTargetGuid || !isAlivePredicate(guid)) {
            RemoveThreat(guid); // Shifts array and decrements m_threatCount
        } else {
            ++i;
        }
    }
}

} // namespace battledecomp::ai
