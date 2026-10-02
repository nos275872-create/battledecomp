#include "ai/bot_ai.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace battledecomp::ai;

int main() {
    std::cout << "====================================================\n";
    std::cout << "  STAR WARS: BATTLEFRONT: RENEGADE SQUADRON (PSP)   \n";
    std::cout << "          Bot AI Controller & Behaviors Test        \n";
    std::cout << "====================================================\n\n";

    BotAIController bot;

    // 1. Initial State verification (0x00188A44)
    std::cout << "[+] Verifying Initial Bot State (0x00188A44)...\n";
    assert(bot.GetBehaviorMode() == BehaviorMode::AttackCpVariant);
    assert(bot.GetCurrentTargetGuid() == BotAIController::kNullTargetGuid);
    assert(bot.GetThreatCount() == 0);
    for (size_t i = 0; i < BotAIController::kMaxThreats; ++i) {
        assert(bot.GetThreat(i) == BotAIController::kNullTargetGuid);
    }
    std::cout << "    Default Behavior: " << BotAIController::GetBehaviorModeName(bot.GetBehaviorMode())
              << " (" << static_cast<uint32_t>(bot.GetBehaviorMode()) << ") [OK]\n";
    std::cout << "    Threat slots: 5 empty slots [OK]\n\n";

    // 2. Behavior Mode Strings (0x00188EF4)
    std::cout << "[+] Verifying Behavior Mode Strings (0x00188EF4)...\n";
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::HunterSeeker)) == "Hunter Seeker");
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::AttackCpVariant)) == "Attack : CP Variant");
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::AttackCtfVariant)) == "Attack : CTF Variant");
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::AttackCtfVariantB)) == "Attack : CTF Variant-b");
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::DefendCommandPost)) == "Defend : Command Post");
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::StandardAttack)) == "Standard Attack");
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::StandardDefend)) == "Standard Defend");
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::Withdrawal)) == "Withdrawl");
    assert(std::string(BotAIController::GetBehaviorModeName(BehaviorMode::Hero)) == "Hero");
    std::cout << "    All 9 Behavior Mode string mappings verified! [OK]\n\n";

    // 3. Sub-Action Strings (0x003175B0 / 0x00317640)
    std::cout << "[+] Verifying Soldier & Lightsaber Subactions (0x003175B0)...\n";
    assert(std::string(BotAIController::GetSoldierSubActionName(SoldierSubAction::Unset)) == "UNSET");
    assert(std::string(BotAIController::GetSoldierSubActionName(SoldierSubAction::RunningToThreat)) == "RUNNING_TO_THREAT");
    assert(std::string(BotAIController::GetSoldierSubActionName(SoldierSubAction::IncomingLightsaberThrow)) == "INCOMING_LIGHTSABER_THROW");
    assert(std::string(BotAIController::GetSoldierSubActionName(SoldierSubAction::PerformingLightsaberAttack)) == "PERFORMING_LIGHTSABER_ATTACK");
    std::cout << "    Subaction strings verified! [OK]\n\n";

    // 4. Target Tracking (0x00189284)
    std::cout << "[+] Verifying Target Tracking Position & Angle (0x00189284)...\n";
    Vector3 targetPos{120.5f, 10.0f, -45.2f};
    bot.SetTargetTracking(targetPos, 1.57f);
    assert(bot.GetTargetPosition() == targetPos);
    assert(bot.GetTargetAngle() == 1.57f);
    std::cout << "    Target tracking: Pos(" << targetPos.x << ", " << targetPos.y << ", " << targetPos.z
              << "), Angle=" << bot.GetTargetAngle() << " rad [OK]\n\n";

    // 5. Threat Management (0x00188DB8 & 0x00189854)
    std::cout << "[+] Verifying Threat Management (0x00188DB8 & 0x00189854)...\n";
    assert(bot.AddThreat(101));
    assert(bot.AddThreat(102));
    assert(bot.AddThreat(103));
    assert(bot.AddThreat(104));
    assert(bot.AddThreat(105));
    assert(!bot.AddThreat(106)); // Exceeds kMaxThreats
    assert(bot.GetThreatCount() == 5);
    std::cout << "    Added 5 threats (capacity limit enforced) [OK]\n";

    // Remove middle threat (103) - verify array shift (0x00189854)
    assert(bot.RemoveThreat(103));
    assert(bot.GetThreatCount() == 4);
    assert(bot.GetThreat(0) == 101);
    assert(bot.GetThreat(1) == 102);
    assert(bot.GetThreat(2) == 104); // Shifted
    assert(bot.GetThreat(3) == 105); // Shifted
    assert(bot.GetThreat(4) == BotAIController::kNullTargetGuid);
    std::cout << "    Removed threat 103: shift verified [OK]\n";

    // Filter threats (simulate death of 102 and 105) (0x00188DB8)
    bot.FilterThreats([](uint32_t guid) {
        return guid != 102 && guid != 105; // 102 and 105 are dead
    });
    assert(bot.GetThreatCount() == 2);
    assert(bot.GetThreat(0) == 101);
    assert(bot.GetThreat(1) == 104);
    std::cout << "    Filtered dead threats: remaining [101, 104] [OK]\n\n";

    std::cout << "====================================================\n";
    std::cout << "          ALL BOT AI TESTS PASSED (100%)            \n";
    std::cout << "====================================================\n";
    return 0;
}
