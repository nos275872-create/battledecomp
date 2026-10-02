#include "core/asura_archive.h"
#include "core/resource_mgr.h"
#include "systems/weapons.h"

#include <iostream>
#include <cassert>
#include <iomanip>

using namespace battledecomp::core;
using namespace battledecomp::systems;

int main() {
    std::cout << "====================================================\n";
    std::cout << "  STAR WARS: BATTLEFRONT: RENEGADE SQUADRON (PSP)   \n";
    std::cout << "      Resource Loader & Weapons Subsystem Test      \n";
    std::cout << "====================================================\n\n";

    // 1. Verify HUD Archive names for Eras (FUN_00171f04)
    std::cout << "[+] Verifying HUD Weapons Archive Era Mapping (0x00171F04)...\n";
    const char* prequelHud = Weapons_GetHudArchiveName(GameEra::Prequel);
    const char* classicHud = Weapons_GetHudArchiveName(GameEra::Classic);
    assert(prequelHud != nullptr);
    assert(classicHud != nullptr);
    assert(std::string(prequelHud) == "Graphics\\HUD_Weapons_Prequel.asr");
    assert(std::string(classicHud) == "Graphics\\HUD_Weapons_Classic.asr");
    std::cout << "    Prequel Era HUD: " << prequelHud << " [OK]\n";
    std::cout << "    Classic Era HUD: " << classicHud << " [OK]\n\n";

    // 2. Initialize ResourceMgr and load WEAPONNAMES.ASR
    std::cout << "[+] Initializing Resource Manager and Loading Weapons Archive...\n";
    ResourceMgr& resMgr = ResourceMgr::Instance();
    resMgr.SetBaseDirectory("orig/extracted/PSP_GAME/USRDIR");

    bool loaded = resMgr.LoadArchive("text/weaponnames.asr");
    if (!loaded) {
        std::cerr << "[-] FAILED to load text/weaponnames.asr!\n";
        return 1;
    }
    std::cout << "    Loaded text/weaponnames.asr successfully! [OK]\n\n";

    // 3. Verify Weapon Tables across Languages
    const LanguageId testLangs[] = {
        LanguageId::English,
        LanguageId::Spanish,
        LanguageId::French,
        LanguageId::German,
        LanguageId::Italian
    };

    for (LanguageId lang : testLangs) {
        std::cout << "----------------------------------------------------\n";
        std::cout << "  Language: " << LanguageIdToString(lang) << "\n";
        std::cout << "----------------------------------------------------\n";

        auto weapons = Weapons_GetAllWeapons(lang);
        assert(weapons.size() == static_cast<size_t>(WeaponId::Count));

        for (size_t i = 0; i < weapons.size(); ++i) {
            std::cout << "  [" << std::right << std::setw(2) << std::setfill('0') << i << "] "
                      << std::left << std::setw(28) << std::setfill(' ') << weapons[i].internalId
                      << " => " << weapons[i].localizedName << "\n";
        }
        std::cout << "\n";
    }

    // 4. Assert specific known translations
    assert(Weapons_GetWeaponName(WeaponId::BlasterRifle, LanguageId::English) == "Blaster Rifle");
    assert(Weapons_GetWeaponName(WeaponId::BlasterRifle, LanguageId::Spanish) == "Fusil bláster");
    assert(Weapons_GetWeaponName(WeaponId::ThermalDetonator, LanguageId::English) == "Thermal Detonator");
    assert(Weapons_GetWeaponName(WeaponId::ThermalDetonator, LanguageId::Spanish) == "Detonador térmico");
    assert(Weapons_GetWeaponName(WeaponId::ClusterGrenade, LanguageId::Spanish) == "Granada racimo");

    std::cout << "====================================================\n";
    std::cout << "  ALL ASSERTIONS PASSED (21 Weapons, 5 Languages)   \n";
    std::cout << "====================================================\n";
    return 0;
}
