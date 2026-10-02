#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace battledecomp::core {

// FourCC helper to convert 4-character string to 32-bit integer (Little-Endian)
constexpr uint32_t MakeFourCC(char a, char b, char c, char d) {
    return static_cast<uint32_t>(static_cast<uint8_t>(a)) |
           (static_cast<uint32_t>(static_cast<uint8_t>(b)) << 8) |
           (static_cast<uint32_t>(static_cast<uint8_t>(c)) << 16) |
           (static_cast<uint32_t>(static_cast<uint8_t>(d)) << 24);
}

// Common Asura Engine FourCC chunk identifiers
namespace FourCC {
    constexpr uint32_t FNFO = MakeFourCC('F', 'N', 'F', 'O'); // File Info
    constexpr uint32_t RSFL = MakeFourCC('R', 'S', 'F', 'L'); // Resource File
    constexpr uint32_t PTXT = MakeFourCC('P', 'T', 'X', 'T'); // Plain / Parsed Text Table
    constexpr uint32_t LTXT = MakeFourCC('L', 'T', 'X', 'T'); // Localized Text
    constexpr uint32_t TTXT = MakeFourCC('T', 'T', 'X', 'T'); // Texture Text
    constexpr uint32_t TEXT = MakeFourCC('T', 'E', 'X', 'T'); // Texture
    constexpr uint32_t MTRL = MakeFourCC('M', 'T', 'R', 'L'); // Material
    constexpr uint32_t ENTI = MakeFourCC('E', 'N', 'T', 'I'); // Entity
    constexpr uint32_t FONT = MakeFourCC('F', 'O', 'N', 'T'); // Font
    constexpr uint32_t CUTS = MakeFourCC('C', 'U', 'T', 'S'); // Cutscene
}

// Asura localized language identifiers
enum class LanguageId : uint32_t {
    English = 0,
    French  = 1,
    German  = 2,
    Italian = 3,
    Spanish = 4,
    Default = 6,
    Unknown = 0xFFFFFFFF
};

const char* LanguageIdToString(LanguageId lang);

// Generic Chunk Header present at the beginning of each chunk
struct ChunkHeader {
    uint32_t fourcc;
    uint32_t size;      // Total size of chunk including this 8-byte header
};

// PTXT string table entry structure
struct TextTable {
    std::string tableName;
    uint32_t tableId;
    LanguageId language;
    std::vector<std::string> strings; // UTF-8 encoded strings for convenient access
};

class AsuraArchive {
public:
    AsuraArchive();
    ~AsuraArchive();

    // Loads and parses an .asr archive file from disk
    bool LoadFromFile(const std::string& filePath);

    // Loads and parses an .asr archive from raw bytes
    bool LoadFromMemory(const uint8_t* data, size_t size);

    // Queries
    bool IsLoaded() const { return m_loaded; }
    bool IsCompressed() const { return m_compressed; }
    const std::string& GetFilePath() const { return m_filePath; }

    // Text tables
    const std::vector<TextTable>& GetTextTables() const { return m_textTables; }
    const TextTable* FindTextTable(const std::string& name, LanguageId lang) const;

    // String lookup
    std::string GetString(const std::string& tableName, LanguageId lang, size_t index) const;

private:
    bool ParseChunks(const uint8_t* data, size_t size);
    bool ParsePtxtChunk(const uint8_t* chunkData, size_t chunkSize);

    bool m_loaded;
    bool m_compressed;
    std::string m_filePath;
    std::vector<TextTable> m_textTables;
};

} // namespace battledecomp::core
