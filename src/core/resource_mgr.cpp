#include "core/resource_mgr.h"

#include <iostream>
#include <algorithm>
#include <filesystem>

namespace battledecomp::core {

namespace {

// Table of default text archives loaded during engine startup (0x00317BE0)
const char* const kDefaultTextArchives[] = {
    "text/stdtext.asr",
    "text/cpnames.asr",
    "text/GalacticConquest.asr",
    "text/objectnames.asr",
    "text/missionobjectives.asr",
    "text/vehiclenames.asr",
    "text/weaponnames.asr",
    "text/botnames_character.asr",
    "text/EndGame_Message.asr",
    "text/LoadingHints_ConquestGround.asr",
    "text/LoadingHints_ConquestSpace.asr",
    "text/LoadingHints_CTF.asr",
    "text/LoadingHints_GC.asr",
    nullptr // Sentinel NULL terminating the loop at 0x00176098
};

// Normalize path separators and find existing file case-insensitively if needed
std::string NormalizePath(const std::string& path) {
    std::string norm = path;
    for (char& c : norm) {
        if (c == '\\') c = '/';
    }
    return norm;
}

} // anonymous namespace

ResourceMgr& ResourceMgr::Instance() {
    static ResourceMgr instance;
    return instance;
}

ResourceMgr::ResourceMgr()
    : m_baseDir("orig/extracted/PSP_GAME/USRDIR"),
      m_currentLang(LanguageId::English),
      m_loadDepth(0) {}

ResourceMgr::~ResourceMgr() = default;

void ResourceMgr::SetBaseDirectory(const std::string& baseDir) {
    m_baseDir = baseDir;
}

void ResourceMgr::SetCurrentLanguage(LanguageId lang) {
    m_currentLang = lang;
}

std::string ResourceMgr::ResolvePath(const std::string& relativePath) const {
    std::string norm = NormalizePath(relativePath);
    std::filesystem::path current(m_baseDir);

    std::stringstream ss(norm);
    std::string segment;
    while (std::getline(ss, segment, '/')) {
        if (segment.empty() || segment == ".") continue;

        bool found = false;
        if (std::filesystem::exists(current) && std::filesystem::is_directory(current)) {
            for (const auto& entry : std::filesystem::directory_iterator(current)) {
                std::string entryName = entry.path().filename().string();
                std::string eUpper = entryName;
                std::string sUpper = segment;
                std::transform(eUpper.begin(), eUpper.end(), eUpper.begin(), ::toupper);
                std::transform(sUpper.begin(), sUpper.end(), sUpper.begin(), ::toupper);
                if (eUpper == sUpper) {
                    current = entry.path();
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            current /= segment;
        }
    }
    return current.string();
}

bool ResourceMgr::LoadArchive(const std::string& relativePath) {
    // 0x00023B7C: Asura_ResourceMgr_LoadArchive calls FUN_00023bec(path, 0)
    return LoadCore(relativePath, 0);
}

bool ResourceMgr::LoadCore(const std::string& relativePath, int flags) {
    // 0x00023BEC: Asura_ResourceMgr_LoadCore
    (void)flags;
    m_loadDepth++;

    std::string resolved = ResolvePath(relativePath);
    if (!std::filesystem::exists(resolved)) {
        std::cerr << "[ResourceMgr] Archive file not found: " << resolved << std::endl;
        m_loadDepth--;
        return false;
    }

    auto archive = std::make_unique<AsuraArchive>();
    if (!archive->LoadFromFile(resolved)) {
        std::cerr << "[ResourceMgr] Failed to parse archive: " << resolved << std::endl;
        m_loadDepth--;
        return false;
    }

    std::string normKey = NormalizePath(relativePath);
    std::transform(normKey.begin(), normKey.end(), normKey.begin(), ::tolower);
    m_loadedArchives[normKey] = std::move(archive);

    m_loadDepth--;
    return true;
}

void ResourceMgr::UnloadArchive(const std::string& relativePath) {
    std::string normKey = NormalizePath(relativePath);
    std::transform(normKey.begin(), normKey.end(), normKey.begin(), ::tolower);
    m_loadedArchives.erase(normKey);
}

const AsuraArchive* ResourceMgr::GetArchive(const std::string& relativePath) const {
    std::string normKey = NormalizePath(relativePath);
    std::transform(normKey.begin(), normKey.end(), normKey.begin(), ::tolower);
    auto it = m_loadedArchives.find(normKey);
    if (it != m_loadedArchives.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::string ResourceMgr::GetText(const std::string& tableName, size_t index) const {
    return GetText(tableName, m_currentLang, index);
}

std::string ResourceMgr::GetText(const std::string& tableName, LanguageId lang, size_t index) const {
    for (const auto& pair : m_loadedArchives) {
        std::string text = pair.second->GetString(tableName, lang, index);
        if (!text.empty()) {
            return text;
        }
    }
    return "";
}

bool ResourceMgr::InitTextArchives() {
    // 0x0017603C: Text_InitResourceArchives
    bool success = true;
    for (size_t i = 0; kDefaultTextArchives[i] != nullptr; ++i) {
        if (!LoadArchive(kDefaultTextArchives[i])) {
            success = false;
        }
    }
    return success;
}

} // namespace battledecomp::core
