#include "core/asura_blueprint.h"
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>

using namespace battledecomp::core;

int main() {
    AsuraBlueprintArchive archive;
    if (!archive.LoadFromFile("orig/extracted/PSP_GAME/USRDIR/MISC/COMMON.ASR")) {
        std::cerr << "Failed to load COMMON.ASR\n";
        return 1;
    }

    std::vector<std::string> categories = {"Tank", "Turret", "Flyer", "Walker"};
    for (const auto& cat : categories) {
        std::cout << "\n========================================\n";
        std::cout << " CATEGORY: " << cat << "\n";
        std::cout << "========================================\n";

        const Blueprint* bp = archive.FindBlueprint(cat);
        if (!bp) {
            std::cout << "Blueprint not found: " << cat << "\n";
            continue;
        }

        std::cout << "Templates (properties) count: " << bp->properties.size() << "\n";
        for (const auto& [propName, prop] : bp->properties) {
            std::cout << "\nTemplate: [" << propName << "] (elements count: " << prop.elements.size() << ")\n";
            for (const auto& [elemName, elem] : prop.elements) {
                std::cout << "   - " << std::left << std::setw(28) << elemName << " : ";
                for (const auto& v : elem.values) {
                    switch (v.type) {
                        case BlueprintValueType::Int32:
                            std::cout << "int(" << v.AsInt() << ") ";
                            break;
                        case BlueprintValueType::Float32:
                            std::cout << "float(" << v.AsFloat() << ") ";
                            break;
                        case BlueprintValueType::Bool:
                            std::cout << "bool(" << (v.AsBool() ? "true" : "false") << ") ";
                            break;
                        case BlueprintValueType::String:
                            std::cout << "str(\"" << v.AsString() << "\") ";
                            break;
                    }
                }
                std::cout << "\n";
            }
        }
    }

    return 0;
}
