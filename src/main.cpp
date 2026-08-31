#include "ui/AppWindow.hpp"
#include "core/ProcessManager.hpp"
#include "core/DiscordRpcClient.hpp"
#include "core/ConfigManager.hpp"
#include <imgui.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <algorithm>

namespace {

std::string FormatDuration(std::chrono::seconds totalSeconds) {
    auto hrs = std::chrono::duration_cast<std::chrono::hours>(totalSeconds);
    totalSeconds -= hrs;
    auto mins = std::chrono::duration_cast<std::chrono::minutes>(totalSeconds);
    totalSeconds -= mins;
    auto secs = totalSeconds;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hrs.count() << ":"
        << std::setfill('0') << std::setw(2) << mins.count() << ":"
        << std::setfill('0') << std::setw(2) << secs.count();
    return oss.str();
}

} // anonymous namespace

int main() {
    ui::AppWindow window;
    if (!window.Initialize(L"Discord Quests & Activity Emulator", 680, 520)) {
        std::cerr << "[ERROR] Failed to initialize application window." << std::endl;
        return 1;
    }

    core::AppConfig config;
    core::ConfigManager::Load(config);

    core::ProcessManager processManager;
    core::DiscordRpcClient rpcClient;

    char gameNameBuffer[128];
    char clientIdBuffer[64];
    char detailsBuffer[128];
    char stateBuffer[128];

    auto SyncBuffersFromConfig = [&]() {
        strncpy_s(gameNameBuffer, sizeof(gameNameBuffer), config.lastGameName.c_str(), _TRUNCATE);
        strncpy_s(clientIdBuffer, sizeof(clientIdBuffer), config.discordClientId.c_str(), _TRUNCATE);
        strncpy_s(detailsBuffer, sizeof(detailsBuffer), config.rpcDetails.c_str(), _TRUNCATE);
        strncpy_s(stateBuffer, sizeof(stateBuffer), config.rpcState.c_str(), _TRUNCATE);
    };

    SyncBuffersFromConfig();

    bool isActivityActive = false;
    bool minimizeToTrayOnClose = window.IsMinimizeToTrayOnClose();
    std::string generalStatusMessage = "Ready";

    auto StartActivity = [&]() {
        config.lastGameName = gameNameBuffer;
        config.discordClientId = clientIdBuffer;
        config.rpcDetails = detailsBuffer;
        config.rpcState = stateBuffer;

        // Add to recent list if not present
        auto it = std::find(config.recentGames.begin(), config.recentGames.end(), config.lastGameName);
        if (it != config.recentGames.end()) {
            config.recentGames.erase(it);
        }
        config.recentGames.insert(config.recentGames.begin(), config.lastGameName);
        if (config.recentGames.size() > 10) {
            config.recentGames.pop_back();
        }

        core::ConfigManager::Save(config);

        bool procOk = true;
        bool rpcOk = true;

        if (config.mode == core::ActivityMode::ProcessEmulation || config.mode == core::ActivityMode::Hybrid) {
            procOk = processManager.StartGame(config.lastGameName);
        }

        if (config.mode == core::ActivityMode::DiscordRpc || config.mode == core::ActivityMode::Hybrid) {
            if (rpcClient.Connect(config.discordClientId)) {
                core::DiscordActivity act;
                act.details = config.rpcDetails.empty() ? ("Playing " + config.lastGameName) : config.rpcDetails;
                act.state = config.rpcState.empty() ? "In Game" : config.rpcState;
                act.startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();
                act.largeImageKey = config.rpcLargeImage;
                act.largeImageText = config.lastGameName;
                rpcClient.SetActivity(act);
            } else {
                rpcOk = false;
            }
        }

        if (procOk || rpcOk) {
            isActivityActive = true;
            generalStatusMessage = "Activity running successfully";
        } else {
            isActivityActive = false;
            generalStatusMessage = "Failed to start activity";
        }
    };

    auto StopActivity = [&]() {
        if (processManager.IsRunning()) {
            processManager.StopGame();
        }
        if (rpcClient.IsConnected()) {
            rpcClient.Disconnect();
        }
        isActivityActive = false;
        generalStatusMessage = "Activity stopped";
    };

    window.Run([&]() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                                ImGuiWindowFlags_NoMove | 
                                ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("MainPanel", nullptr, flags);

        // Header Title
        ImGui::TextColored(ImVec4(0.35f, 0.40f, 0.95f, 1.0f), "DISCORD QUESTS");
        ImGui::SameLine();
        ImGui::TextDisabled("| Activity & Process Emulator");

        ImGui::SameLine(ImGui::GetWindowWidth() - 150);
        if (ImGui::SmallButton("Minimize to Tray")) {
            window.MinimizeToTray();
        }

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginTabBar("Tabs")) {
            // TAB 1: Main Control
            if (ImGui::BeginTabItem("Main Activity")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.96f, 1.0f), "Select Mode:");

                int currentMode = static_cast<int>(config.mode);
                if (isActivityActive) {
                    ImGui::BeginDisabled();
                }
                ImGui::RadioButton("Process Emulation (.exe)", &currentMode, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Discord RPC", &currentMode, 1);
                ImGui::SameLine();
                ImGui::RadioButton("Hybrid (Both)", &currentMode, 2);
                if (isActivityActive) {
                    ImGui::EndDisabled();
                }
                config.mode = static_cast<core::ActivityMode>(currentMode);

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.96f, 1.0f), "Target Game Name:");
                if (isActivityActive) {
                    ImGui::BeginDisabled();
                }
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##GameName", gameNameBuffer, sizeof(gameNameBuffer));
                if (isActivityActive) {
                    ImGui::EndDisabled();
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // Start / Stop Button with Discord-inspired colors
                if (!isActivityActive) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.65f, 0.35f, 1.0f)); // Discord Green
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.12f, 0.55f, 0.30f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.45f, 0.25f, 1.0f));
                    if (ImGui::Button("START ACTIVITY", ImVec2(160, 40))) {
                        StartActivity();
                    }
                    ImGui::PopStyleColor(3);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.25f, 0.26f, 1.0f)); // Discord Red
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.22f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.68f, 0.15f, 0.17f, 1.0f));
                    if (ImGui::Button("STOP ACTIVITY", ImVec2(160, 40))) {
                        StopActivity();
                    }
                    ImGui::PopStyleColor(3);
                }

                ImGui::SameLine();
                ImGui::AlignTextToFramePadding();

                if (isActivityActive) {
                    std::string timeStr = FormatDuration(processManager.GetElapsedTime());
                    ImGui::TextColored(
                        ImVec4(0.14f, 0.85f, 0.35f, 1.0f),
                        "Playing: %s (%s)",
                        gameNameBuffer,
                        timeStr.c_str()
                    );
                } else {
                    ImGui::TextColored(ImVec4(0.58f, 0.61f, 0.64f, 1.0f), "Status: %s", generalStatusMessage.c_str());
                }

                if (!processManager.GetLastErrorMessage().empty()) {
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.95f, 0.25f, 0.26f, 1.0f), "Process Error: %s", processManager.GetLastErrorMessage().c_str());
                }
                if (!rpcClient.GetLastError().empty()) {
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.95f, 0.25f, 0.26f, 1.0f), "RPC Error: %s", rpcClient.GetLastError().c_str());
                }

                ImGui::Spacing();
                ImGui::Separator();
                if (ImGui::Checkbox("Minimize to tray on window close", &minimizeToTrayOnClose)) {
                    window.SetMinimizeToTrayOnClose(minimizeToTrayOnClose);
                }

                ImGui::EndTabItem();
            }

            // TAB 2: Discord RPC Settings
            if (ImGui::BeginTabItem("RPC Settings")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.96f, 1.0f), "Discord Application Client ID:");
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##ClientId", clientIdBuffer, sizeof(clientIdBuffer));
                ImGui::TextDisabled("Developer Portal: https://discord.com/developers/applications");

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.96f, 1.0f), "Activity Details:");
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##Details", detailsBuffer, sizeof(detailsBuffer));

                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.96f, 1.0f), "Activity State:");
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##State", stateBuffer, sizeof(stateBuffer));

                ImGui::Spacing();
                if (ImGui::Button("Save RPC Config", ImVec2(150, 32))) {
                    config.discordClientId = clientIdBuffer;
                    config.rpcDetails = detailsBuffer;
                    config.rpcState = stateBuffer;
                    core::ConfigManager::Save(config);
                    generalStatusMessage = "RPC Settings saved";
                }

                ImGui::EndTabItem();
            }

            // TAB 3: Recent Games & Presets
            if (ImGui::BeginTabItem("Presets & History")) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.96f, 1.0f), "Recent Games (Click to activate):");
                ImGui::Separator();

                for (size_t i = 0; i < config.recentGames.size(); ++i) {
                    const auto& game = config.recentGames[i];
                    if (ImGui::Selectable(game.c_str(), game == gameNameBuffer)) {
                        strncpy_s(gameNameBuffer, sizeof(gameNameBuffer), game.c_str(), _TRUNCATE);
                        config.lastGameName = game;
                        core::ConfigManager::Save(config);
                    }
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    });

    // Cleanup on exit
    StopActivity();
    core::ConfigManager::Save(config);
    return 0;
}
