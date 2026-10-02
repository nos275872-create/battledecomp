#pragma once

#include "core/asura_archive.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace battledecomp::core {

class ResourceMgr {
public:
    static ResourceMgr& Instance();

    // Sets base directory for assets (e.g. "orig/extracted/PSP_GAME/USRDIR")
    void SetBaseDirectory(const std::string& baseDir);
    const std::string& GetBaseDirectory() const { return m_baseDir; }

    // Sets current active language for localized resource lookup
    void SetCurrentLanguage(LanguageId lang);
    LanguageId GetCurrentLanguage() const { return m_currentLang; }

    // Emulates Asura_ResourceMgr_LoadArchive (0x00023B7C)
    bool LoadArchive(const std::string& relativePath);

    // Emulates Asura_ResourceMgr_LoadCore (0x00023BEC)
    bool LoadCore(const std::string& fullPath, int flags = 0);

    // Unload archive by path
    void UnloadArchive(const std::string& relativePath);

    // Get loaded archive
    const AsuraArchive* GetArchive(const std::string& relativePath) const;

    // Look up localized text across loaded archives
    std::string GetText(const std::string& tableName, size_t index) const;

    // Direct string lookup specifying language
    std::string GetText(const std::string& tableName, LanguageId lang, size_t index) const;

    // Initialize all default text archives (emulates 0x0017603C)
    bool InitTextArchives();

private:
    ResourceMgr();
    ~ResourceMgr();

    std::string ResolvePath(const std::string& relativePath) const;

    std::string m_baseDir;
    LanguageId m_currentLang;
    int m_loadDepth; // Mirrors DAT_00311a94

    std::unordered_map<std::string, std::unique_ptr<AsuraArchive>> m_loadedArchives;
};

} // namespace battledecomp::core
