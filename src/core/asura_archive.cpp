#include "core/asura_archive.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>

namespace battledecomp::core {

namespace {

// Helper to convert UTF-16LE code units to UTF-8 std::string
std::string Utf16LeToUtf8(const uint8_t* bytes, size_t charCount) {
    std::string out;
    out.reserve(charCount);

    for (size_t i = 0; i < charCount; ++i) {
        uint16_t ch = static_cast<uint16_t>(bytes[i * 2]) |
                      (static_cast<uint16_t>(bytes[i * 2 + 1]) << 8);

        if (ch == 0) {
            // Null terminator reached
            break;
        }

        if (ch <= 0x7F) {
            out.push_back(static_cast<char>(ch));
        } else if (ch <= 0x7FF) {
            out.push_back(static_cast<char>(0xC0 | ((ch >> 6) & 0x1F)));
            out.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
        } else {
            out.push_back(static_cast<char>(0xE0 | ((ch >> 12) & 0x0F)));
            out.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
        }
    }
    return out;
}

// Safely read a 32-bit little-endian integer
uint32_t ReadU32(const uint8_t* ptr) {
    return static_cast<uint32_t>(ptr[0]) |
           (static_cast<uint32_t>(ptr[1]) << 8) |
           (static_cast<uint32_t>(ptr[2]) << 16) |
           (static_cast<uint32_t>(ptr[3]) << 24);
}

} // anonymous namespace

const char* LanguageIdToString(LanguageId lang) {
    switch (lang) {
        case LanguageId::English: return "English";
        case LanguageId::French:  return "French";
        case LanguageId::German:  return "German";
        case LanguageId::Italian: return "Italian";
        case LanguageId::Spanish: return "Spanish";
        case LanguageId::Default: return "Default";
        default:                  return "Unknown";
    }
}

AsuraArchive::AsuraArchive()
    : m_loaded(false), m_compressed(false) {}

AsuraArchive::~AsuraArchive() = default;

bool AsuraArchive::LoadFromFile(const std::string& filePath) {
    m_filePath = filePath;
    m_loaded = false;
    m_compressed = false;
    m_textTables.clear();

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "[AsuraArchive] Error opening file: " << filePath << std::endl;
        return false;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize < 8) {
        std::cerr << "[AsuraArchive] File too small: " << filePath << std::endl;
        return false;
    }

    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        std::cerr << "[AsuraArchive] Error reading file: " << filePath << std::endl;
        return false;
    }

    return LoadFromMemory(buffer.data(), buffer.size());
}

bool AsuraArchive::LoadFromMemory(const uint8_t* data, size_t size) {
    if (size < 8) {
        return false;
    }

    // Verify 8-byte magic
    if (std::memcmp(data, "Asura   ", 8) == 0) {
        m_compressed = false;
    } else if (std::memcmp(data, "AsuraCmp", 8) == 0) {
        m_compressed = true;
        std::cerr << "[AsuraArchive] Compressed Asura archive not yet implemented." << std::endl;
        return false;
    } else {
        std::cerr << "[AsuraArchive] Invalid magic identifier." << std::endl;
        return false;
    }

    if (!ParseChunks(data + 8, size - 8)) {
        return false;
    }

    m_loaded = true;
    return true;
}

bool AsuraArchive::ParseChunks(const uint8_t* data, size_t size) {
    size_t offset = 0;

    while (offset + 8 <= size) {
        uint32_t fourcc = ReadU32(data + offset);
        uint32_t chunkSize = ReadU32(data + offset + 4);

        if (chunkSize < 8 || offset + chunkSize > size) {
            std::cerr << "[AsuraArchive] Corrupt chunk size: " << chunkSize
                      << " at offset " << offset << std::endl;
            return false;
        }

        const uint8_t* chunkPayload = data + offset;

        if (fourcc == FourCC::PTXT) {
            if (!ParsePtxtChunk(chunkPayload, chunkSize)) {
                std::cerr << "[AsuraArchive] Failed to parse PTXT chunk." << std::endl;
                return false;
            }
        }

        offset += chunkSize;
    }

    return true;
}

bool AsuraArchive::ParsePtxtChunk(const uint8_t* chunkData, size_t chunkSize) {
    // PTXT Chunk Header layout:
    // +0x00: FourCC 'PTXT'
    // +0x04: uint32 chunkSize
    // +0x08: uint32 version
    // +0x0C: uint32 flags
    // +0x10: uint32 tableId
    // +0x14: uint32 stringCount
    if (chunkSize < 24) {
        return false;
    }

    uint32_t version = ReadU32(chunkData + 8);
    // uint32_t flags = ReadU32(chunkData + 12);
    uint32_t tableId = ReadU32(chunkData + 16);
    uint32_t stringCount = ReadU32(chunkData + 20);

    size_t pos = 24;

    LanguageId lang = LanguageId::English;
    if (version >= 5) {
        if (pos + 4 > chunkSize) return false;
        lang = static_cast<LanguageId>(ReadU32(chunkData + pos));
        pos += 4;
    }

    // Read null-terminated table name string
    size_t nameStart = pos;
    while (pos < chunkSize && chunkData[pos] != '\0') {
        pos++;
    }
    if (pos >= chunkSize) return false;

    std::string tableName(reinterpret_cast<const char*>(chunkData + nameStart), pos - nameStart);
    pos++; // Skip null terminator

    // Align stream position to 4-byte boundary
    size_t nameLen = (pos - nameStart);
    size_t paddedLen = (nameLen + 3) & ~3;
    pos = nameStart + paddedLen;

    TextTable table;
    table.tableName = tableName;
    table.tableId = tableId;
    table.language = lang;
    table.strings.reserve(stringCount);

    for (uint32_t i = 0; i < stringCount; ++i) {
        if (pos + 4 > chunkSize) return false;
        uint32_t charCount = ReadU32(chunkData + pos);
        pos += 4;

        size_t byteCount = charCount * 2;
        if (pos + byteCount > chunkSize) return false;

        std::string str = Utf16LeToUtf8(chunkData + pos, charCount);
        pos += byteCount;

        table.strings.push_back(std::move(str));
    }

    m_textTables.push_back(std::move(table));
    return true;
}

const TextTable* AsuraArchive::FindTextTable(const std::string& name, LanguageId lang) const {
    for (const auto& tbl : m_textTables) {
        if (tbl.tableName == name && tbl.language == lang) {
            return &tbl;
        }
    }
    // Fallback to first matching table if language not found
    for (const auto& tbl : m_textTables) {
        if (tbl.tableName == name) {
            return &tbl;
        }
    }
    return nullptr;
}

std::string AsuraArchive::GetString(const std::string& tableName, LanguageId lang, size_t index) const {
    const TextTable* tbl = FindTextTable(tableName, lang);
    if (!tbl || index >= tbl->strings.size()) {
        return "";
    }
    return tbl->strings[index];
}

} // namespace battledecomp::core
