#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>

namespace battledecomp::core {

// Supported Asura Blueprint primitive value types
enum class BlueprintValueType {
    Int32,
    Float32,
    Bool,
    String
};

struct BlueprintValue {
    BlueprintValueType type;
    std::variant<int32_t, float, bool, std::string> value;

    int32_t AsInt(int32_t defaultVal = 0) const {
        if (std::holds_alternative<int32_t>(value)) return std::get<int32_t>(value);
        if (std::holds_alternative<float>(value)) return static_cast<int32_t>(std::get<float>(value));
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? 1 : 0;
        return defaultVal;
    }

    float AsFloat(float defaultVal = 0.0f) const {
        if (std::holds_alternative<float>(value)) return std::get<float>(value);
        if (std::holds_alternative<int32_t>(value)) return static_cast<float>(std::get<int32_t>(value));
        return defaultVal;
    }

    bool AsBool(bool defaultVal = false) const {
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
        if (std::holds_alternative<int32_t>(value)) return std::get<int32_t>(value) != 0;
        return defaultVal;
    }

    std::string AsString(const std::string& defaultVal = "") const {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        return defaultVal;
    }
};

// Represents an element inside a Blueprint property
struct BlueprintElement {
    uint32_t id{0};
    std::string name;
    uint32_t flags{0};
    std::vector<BlueprintValue> values;

    const BlueprintValue* GetFirstValue() const {
        return values.empty() ? nullptr : &values[0];
    }
};

// Represents a named property (e.g. "Base", "BlasterRifle", "ALL_Blaster_Pistol")
struct BlueprintProperty {
    uint32_t id{0};
    std::string name;
    uint32_t flags{0};
    std::unordered_map<std::string, BlueprintElement> elements;

    bool HasElement(const std::string& elemName) const {
        return elements.find(elemName) != elements.end();
    }

    const BlueprintElement* FindElement(const std::string& elemName) const {
        auto it = elements.find(elemName);
        if (it != elements.end()) return &it->second;
        for (const auto& [name, el] : elements) {
            if (name.size() == elemName.size()) {
                bool match = true;
                for (size_t i = 0; i < name.size(); ++i) {
                    if (std::tolower(static_cast<unsigned char>(name[i])) !=
                        std::tolower(static_cast<unsigned char>(elemName[i]))) {
                        match = false;
                        break;
                    }
                }
                if (match) return &el;
            }
        }
        return nullptr;
    }

    float GetFloat(const std::string& elemName, float defaultVal = 0.0f) const {
        const auto* el = FindElement(elemName);
        if (!el || el->values.empty()) return defaultVal;
        return el->values[0].AsFloat(defaultVal);
    }

    int32_t GetInt(const std::string& elemName, int32_t defaultVal = 0) const {
        const auto* el = FindElement(elemName);
        if (!el || el->values.empty()) return defaultVal;
        return el->values[0].AsInt(defaultVal);
    }

    bool GetBool(const std::string& elemName, bool defaultVal = false) const {
        const auto* el = FindElement(elemName);
        if (!el || el->values.empty()) return defaultVal;
        return el->values[0].AsBool(defaultVal);
    }

    std::string GetString(const std::string& elemName, const std::string& defaultVal = "") const {
        const auto* el = FindElement(elemName);
        if (!el || el->values.empty()) return defaultVal;
        return el->values[0].AsString(defaultVal);
    }
};

// Represents a Blueprint (e.g. "Weapon", "Projectile", "LaserBolt")
struct Blueprint {
    uint32_t id{0};
    std::string name;
    std::unordered_map<std::string, BlueprintProperty> properties;

    bool HasProperty(const std::string& propName) const {
        return FindProperty(propName) != nullptr;
    }

    const BlueprintProperty* FindProperty(const std::string& propName) const {
        auto it = properties.find(propName);
        if (it != properties.end()) return &it->second;
        for (const auto& [name, prop] : properties) {
            if (name.size() == propName.size()) {
                bool match = true;
                for (size_t i = 0; i < name.size(); ++i) {
                    if (std::tolower(static_cast<unsigned char>(name[i])) !=
                        std::tolower(static_cast<unsigned char>(propName[i]))) {
                        match = false;
                        break;
                    }
                }
                if (match) return &prop;
            }
        }
        return nullptr;
    }
};

// Blueprint Archive Loader
class AsuraBlueprintArchive {
public:
    AsuraBlueprintArchive();
    ~AsuraBlueprintArchive();

    bool LoadFromFile(const std::string& filePath);
    bool LoadFromMemory(const uint8_t* data, size_t size);

    const Blueprint* FindBlueprint(const std::string& name) const;
    std::vector<std::string> GetBlueprintNames() const;
    size_t GetBlueprintCount() const { return m_blueprints.size(); }

private:
    bool ParseChunks(const uint8_t* data, size_t size);
    bool ParseBlueChunk(const uint8_t* chunkData, size_t chunkSize);

    std::unordered_map<std::string, Blueprint> m_blueprints;
};

} // namespace battledecomp::core
