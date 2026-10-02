#include "core/asura_blueprint.h"

#include <fstream>
#include <iostream>
#include <cstring>

namespace battledecomp::core {

namespace {

class AsuraStreamReader {
public:
    AsuraStreamReader(const uint8_t* data, size_t size)
        : m_data(data), m_size(size), m_pos(0) {}

    bool HasBytes(size_t n) const {
        return m_pos + n <= m_size;
    }

    size_t GetPos() const { return m_pos; }

    uint32_t ReadU32() {
        if (!HasBytes(4)) return 0;
        uint32_t val = static_cast<uint32_t>(m_data[m_pos]) |
                       (static_cast<uint32_t>(m_data[m_pos + 1]) << 8) |
                       (static_cast<uint32_t>(m_data[m_pos + 2]) << 16) |
                       (static_cast<uint32_t>(m_data[m_pos + 3]) << 24);
        m_pos += 4;
        return val;
    }

    int32_t ReadI32() {
        return static_cast<int32_t>(ReadU32());
    }

    float ReadF32() {
        uint32_t u = ReadU32();
        float f;
        std::memcpy(&f, &u, sizeof(float));
        return f;
    }

    uint8_t ReadU8() {
        if (!HasBytes(1)) return 0;
        return m_data[m_pos++];
    }

    // In Asura Engine (FUN_00029D88), strings are read in 4-byte chunks until a null terminator byte is found.
    std::string ReadString() {
        std::string s;
        bool foundNull = false;
        while (!foundNull && HasBytes(4)) {
            for (int i = 0; i < 4; ++i) {
                char c = static_cast<char>(m_data[m_pos++]);
                if (c == '\0') {
                    foundNull = true;
                    // Skip remaining bytes of this 4-byte chunk
                    m_pos += (3 - i);
                    break;
                }
                s.push_back(c);
            }
        }
        return s;
    }

private:
    const uint8_t* m_data;
    size_t m_size;
    size_t m_pos;
};

uint32_t ReadU32LE(const uint8_t* ptr) {
    return static_cast<uint32_t>(ptr[0]) |
           (static_cast<uint32_t>(ptr[1]) << 8) |
           (static_cast<uint32_t>(ptr[2]) << 16) |
           (static_cast<uint32_t>(ptr[3]) << 24);
}

} // anonymous namespace

AsuraBlueprintArchive::AsuraBlueprintArchive() = default;
AsuraBlueprintArchive::~AsuraBlueprintArchive() = default;

bool AsuraBlueprintArchive::LoadFromFile(const std::string& filePath) {
    m_blueprints.clear();

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "[AsuraBlueprintArchive] Error opening file: " << filePath << std::endl;
        return false;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize < 8) {
        return false;
    }

    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        return false;
    }

    return LoadFromMemory(buffer.data(), buffer.size());
}

bool AsuraBlueprintArchive::LoadFromMemory(const uint8_t* data, size_t size) {
    if (size < 8) return false;

    if (std::memcmp(data, "Asura   ", 8) != 0) {
        std::cerr << "[AsuraBlueprintArchive] Invalid magic identifier." << std::endl;
        return false;
    }

    return ParseChunks(data + 8, size - 8);
}

bool AsuraBlueprintArchive::ParseChunks(const uint8_t* data, size_t size) {
    size_t offset = 0;
    while (offset + 8 <= size) {
        uint32_t fourcc = ReadU32LE(data + offset);
        uint32_t chunkSize = ReadU32LE(data + offset + 4);

        if (chunkSize < 8 || offset + chunkSize > size) {
            std::cerr << "[AsuraBlueprintArchive] Invalid chunk size: " << chunkSize
                      << " at offset " << offset << std::endl;
            return false;
        }

        // 'BLUE' chunk FourCC = 0x45554C42
        if (fourcc == 0x45554C42) {
            if (!ParseBlueChunk(data + offset + 8, chunkSize - 8)) {
                std::cerr << "[AsuraBlueprintArchive] Failed to parse BLUE chunk at offset "
                          << offset << std::endl;
                return false;
            }
        }

        offset += chunkSize;
    }

    return true;
}

bool AsuraBlueprintArchive::ParseBlueChunk(const uint8_t* chunkData, size_t chunkSize) {
    AsuraStreamReader stream(chunkData, chunkSize);

    // BLUE chunk header: v0, v1, v2, bp_count
    if (!stream.HasBytes(16)) return false;
    /* uint32_t v0 = */ stream.ReadU32();
    /* uint32_t v1 = */ stream.ReadU32();
    /* uint32_t v2 = */ stream.ReadU32();
    uint32_t bpCount = stream.ReadU32();

    for (uint32_t bpIdx = 0; bpIdx < bpCount; ++bpIdx) {
        if (!stream.HasBytes(8)) return false;
        uint32_t bpId = stream.ReadU32();
        uint32_t propCount = stream.ReadU32();
        std::string bpName = stream.ReadString();

        Blueprint bp;
        bp.id = bpId;
        bp.name = bpName;

        for (uint32_t pIdx = 0; pIdx < propCount; ++pIdx) {
            if (!stream.HasBytes(12)) return false;
            uint32_t ver = stream.ReadU32();
            uint32_t pId = stream.ReadU32();
            uint32_t pFlags = stream.ReadU32();
            std::string pName = stream.ReadString();
            uint32_t elemCount = stream.ReadU32();

            BlueprintProperty prop;
            prop.id = pId;
            prop.name = pName;
            prop.flags = pFlags;

            for (uint32_t eIdx = 0; eIdx < elemCount; ++eIdx) {
                if (!stream.HasBytes(8)) return false;
                uint32_t eId = stream.ReadU32();
                std::string eName = stream.ReadString();
                uint32_t eFlags = stream.ReadU32();

                if (ver > 1) {
                    /* uint32_t subHash = */ stream.ReadU32();
                }

                uint32_t valCount = 1;
                if (ver > 2) {
                    valCount = stream.ReadU32();
                }

                BlueprintElement elem;
                elem.id = eId;
                elem.name = eName;
                elem.flags = eFlags;
                elem.values.reserve(valCount);

                for (uint32_t vIdx = 0; vIdx < valCount; ++vIdx) {
                    uint32_t typeTag = stream.ReadU32();
                    BlueprintValue val;

                    if (typeTag == 0) {
                        val.type = BlueprintValueType::Int32;
                        val.value = stream.ReadI32();
                    } else if (typeTag == 1) {
                        val.type = BlueprintValueType::Float32;
                        val.value = stream.ReadF32();
                    } else if (typeTag == 2) {
                        val.type = BlueprintValueType::Bool;
                        val.value = (stream.ReadU8() != 0);
                    } else if (typeTag == 3 || typeTag == 4) {
                        val.type = BlueprintValueType::String;
                        uint32_t strFlag = stream.ReadU32();
                        if (strFlag != 0) {
                            val.value = stream.ReadString();
                        } else {
                            val.value = std::string("");
                        }
                    } else {
                        // Unknown fallback
                        val.type = BlueprintValueType::Int32;
                        val.value = stream.ReadI32();
                    }

                    elem.values.push_back(std::move(val));
                }

                prop.elements[eName] = std::move(elem);
            }

            bp.properties[pName] = std::move(prop);
        }

        m_blueprints[bpName] = std::move(bp);
    }

    return true;
}

const Blueprint* AsuraBlueprintArchive::FindBlueprint(const std::string& name) const {
    auto it = m_blueprints.find(name);
    if (it != m_blueprints.end()) return &it->second;
    for (const auto& [bpName, bp] : m_blueprints) {
        if (bpName.size() == name.size()) {
            bool match = true;
            for (size_t i = 0; i < name.size(); ++i) {
                if (std::tolower(static_cast<unsigned char>(bpName[i])) !=
                    std::tolower(static_cast<unsigned char>(name[i]))) {
                    match = false;
                    break;
                }
            }
            if (match) return &bp;
        }
    }
    return nullptr;
}

std::vector<std::string> AsuraBlueprintArchive::GetBlueprintNames() const {
    std::vector<std::string> names;
    names.reserve(m_blueprints.size());
    for (const auto& [name, bp] : m_blueprints) {
        names.push_back(name);
    }
    return names;
}

} // namespace battledecomp::core
