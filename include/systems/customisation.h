#pragma once

#include "core/asura_blueprint.h"
#include <string>
#include <vector>
#include <cstdint>

namespace battledecomp::systems {

// The 8 customisation categories in Renegade Squadron (BFF.FE_Customisation_Category)
enum class CustomisationCategory : uint32_t {
    PrimaryWeapon   = 0,
    SecondaryWeapon = 1,
    Explosive       = 2,
    Equipment       = 3,
    PowerUp         = 4,
    Health          = 5,
    Speed           = 6,
    Agility         = 7,
    Count           = 8
};

const char* CustomisationCategoryToString(CustomisationCategory cat);

// Represents an equipable item in a customisation category
struct CustomisationItem {
    uint32_t hash{0};
    std::string internalName;
    int32_t cost{0};
};

// Represents a complete player or bot soldier loadout
struct SoldierLoadout {
    int32_t primaryWeaponIndex{1};   // Default: BlasterRifle (1)
    int32_t secondaryWeaponIndex{0}; // Default: BlasterPistol (0)
    int32_t explosiveIndex{1};       // Default: Thermal Detonator (1)
    int32_t equipmentIndex{0};       // Default: None (0)
    int32_t powerUpIndex{0};         // Default: None (0)
    int32_t healthTier{0};           // Tier 0..3
    int32_t speedTier{0};            // Tier 0..3
    int32_t agilityTier{0};          // Tier 0..3

    static constexpr int32_t kDefaultCreditBudget = 100;

    int32_t CalculateTotalCost() const;
    int32_t GetRemainingCredits(int32_t budget = kDefaultCreditBudget) const;
    bool IsValid(int32_t budget = kDefaultCreditBudget) const;
};

// Manages the Customisation categories, items, and credit economy (0x00224878)
class CustomisationManager {
public:
    static CustomisationManager& Instance();

    void Initialize(const core::AsuraBlueprintArchive* blueprintArchive = nullptr);

    size_t GetCategoryItemCount(CustomisationCategory cat) const;
    const CustomisationItem* GetItem(CustomisationCategory cat, size_t index) const;
    int32_t GetItemCost(CustomisationCategory cat, size_t index) const;
    int32_t GetStatCost(CustomisationCategory cat, int32_t tier) const;

    const std::vector<CustomisationItem>& GetCategoryItems(CustomisationCategory cat) const;

private:
    CustomisationManager() = default;

    std::vector<CustomisationItem> m_categories[static_cast<size_t>(CustomisationCategory::Count)];
    bool m_initialized{false};
};

} // namespace battledecomp::systems
