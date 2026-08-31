#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace core {

enum class ActivityMode : int {
    ProcessEmulation = 0,
    DiscordRpc = 1,
    Hybrid = 2
};

struct AppConfig {
    ActivityMode mode = ActivityMode::Hybrid;
    std::string lastGameName = "Grand Theft Auto VI";
    std::string discordClientId = "1104849045763567676"; // Default general gaming App ID
    std::string rpcDetails = "In Game";
    std::string rpcState = "Playing Campaign";
    std::string rpcLargeImage = "game_icon";
    std::vector<std::string> recentGames = {
        "Grand Theft Auto VI",
        "Cyberpunk 2077",
        "Counter-Strike 2",
        "Minecraft",
        "Elden Ring"
    };
};

class ConfigManager {
public:
    static bool Load(AppConfig& outConfig);
    static bool Save(const AppConfig& config);
    static std::filesystem::path GetConfigFilePath();

private:
    static std::filesystem::path GetAppDataDir();
};

} 
