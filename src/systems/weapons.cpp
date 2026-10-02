#include "systems/weapons.h"
#include "core/resource_mgr.h"

#include <stdexcept>

namespace battledecomp::systems {

namespace {

static const char* const kWeaponInternalIds[static_cast<size_t>(WeaponId::Count)] = {
    "WP_BLASTER_RIFLE",
    "WP_BLASTER_PISTOL",
    "WP_ARC_CASTER",
    "WP_FUSION_CUTTER",
    "WP_CARBONITE_FREEZE_GUN",
    "WP_TRI_SHOT",
    "WP_INCINERATOR",
    "WP_WRIST_ROCKET",
    "WP_SHOTGUN",
    "WP_EMP_LAUNCHER",
    "WP_SNIPER_RIFLE",
    "WP_EXPLOSIVE_BLASTER_PISTOL",
    "WP_CHAINGUN",
    "WP_GRENADE_LAUNCHER",
    "WP_BOWCASTER",
    "WP_GUIDED_ROCKET",
    "WP_ROCKET_LAUNCHER",
    "WP_THERMAL_DETONATOR",
    "WP_DETPACKS",
    "WP_MINES",
    "WP_CLUSTER_GRENADE"
};

} // anonymous namespace

const char* Weapons_GetHudArchiveName(GameEra era) {
    // 0x00171F04: FUN_00171f04
    // pcVar1 = "Graphics\\HUD_Weapons_Prequel.asr";
    // if ((DAT_00317bdc != 0) && (pcVar1 = "Graphics\\HUD_Weapons_Classic.asr", DAT_00317bdc != 1)) {
    //     pcVar1 = (char *)0x0;
    // }
    if (era == GameEra::Prequel) {
        return "Graphics\\HUD_Weapons_Prequel.asr";
    }
    if (era == GameEra::Classic) {
        return "Graphics\\HUD_Weapons_Classic.asr";
    }
    return nullptr;
}

const char* Weapons_GetInternalId(WeaponId id) {
    auto idx = static_cast<size_t>(id);
    if (idx < static_cast<size_t>(WeaponId::Count)) {
        return kWeaponInternalIds[idx];
    }
    return "WP_UNKNOWN";
}

std::string Weapons_GetWeaponName(WeaponId id, core::LanguageId lang) {
    auto idx = static_cast<size_t>(id);
    return core::ResourceMgr::Instance().GetText("WeaponNames", lang, idx);
}

std::vector<WeaponInfo> Weapons_GetAllWeapons(core::LanguageId lang) {
    std::vector<WeaponInfo> list;
    list.reserve(static_cast<size_t>(WeaponId::Count));

    for (size_t i = 0; i < static_cast<size_t>(WeaponId::Count); ++i) {
        WeaponId id = static_cast<WeaponId>(i);
        WeaponInfo info;
        info.id = id;
        info.internalId = Weapons_GetInternalId(id);
        info.localizedName = Weapons_GetWeaponName(id, lang);
        list.push_back(std::move(info));
    }

    return list;
}

} // namespace battledecomp::systems
