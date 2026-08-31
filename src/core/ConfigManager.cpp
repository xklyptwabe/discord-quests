#include "core/ConfigManager.hpp"
#include <nlohmann/json.hpp>
#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <algorithm>

namespace core {

std::filesystem::path ConfigManager::GetAppDataDir() {
    wchar_t appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, appDataPath))) {
        return std::filesystem::path(appDataPath) / "DiscordQuests";
    }
    return std::filesystem::current_path();
}

std::filesystem::path ConfigManager::GetConfigFilePath() {
    return GetAppDataDir() / "config.json";
}

bool ConfigManager::Load(AppConfig& outConfig) {
    std::filesystem::path path = GetConfigFilePath();
    if (!std::filesystem::exists(path)) {
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;

        if (j.contains("mode")) {
            outConfig.mode = static_cast<ActivityMode>(j["mode"].get<int>());
        }
        if (j.contains("lastGameName")) {
            outConfig.lastGameName = j["lastGameName"].get<std::string>();
        }
        if (j.contains("discordClientId")) {
            outConfig.discordClientId = j["discordClientId"].get<std::string>();
        }
        if (j.contains("rpcDetails")) {
            outConfig.rpcDetails = j["rpcDetails"].get<std::string>();
        }
        if (j.contains("rpcState")) {
            outConfig.rpcState = j["rpcState"].get<std::string>();
        }
        if (j.contains("rpcLargeImage")) {
            outConfig.rpcLargeImage = j["rpcLargeImage"].get<std::string>();
        }
        if (j.contains("recentGames") && j["recentGames"].is_array()) {
            outConfig.recentGames = j["recentGames"].get<std::vector<std::string>>();
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigManager::Save(const AppConfig& config) {
    std::filesystem::path dir = GetAppDataDir();
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    if (ec) {
        return false;
    }

    std::filesystem::path path = GetConfigFilePath();
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json j;
        j["mode"] = static_cast<int>(config.mode);
        j["lastGameName"] = config.lastGameName;
        j["discordClientId"] = config.discordClientId;
        j["rpcDetails"] = config.rpcDetails;
        j["rpcState"] = config.rpcState;
        j["rpcLargeImage"] = config.rpcLargeImage;
        j["recentGames"] = config.recentGames;

        file << j.dump(4);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace core
