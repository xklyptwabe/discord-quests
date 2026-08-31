#include "ui/AppWindow.hpp"
#include "core/ProcessManager.hpp"
#include <imgui.h>
#include <iostream>
#include <iomanip>
#include <sstream>

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

} 

int main() {
    ui::AppWindow window;
    if (!window.Initialize(L"Discord Quests & Activity Emulator", 650, 480)) {
        std::cerr << "[ERROR] Failed to initialize application window." << std::endl;
        return 1;
    }

    core::ProcessManager processManager;
    char gameNameBuffer[128] = "Grand Theft Auto VI";

    window.Run([&]() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                                ImGuiWindowFlags_NoMove | 
                                ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("MainPanel", nullptr, flags);

        ImGui::TextColored(ImVec4(0.35f, 0.65f, 1.0f, 1.0f), "Discord Quests & Activity Emulator");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Enter Target Game Name:");
        const bool isRunning = processManager.IsRunning();

        if (isRunning) {
            ImGui::BeginDisabled();
        }
        ImGui::InputText("##GameName", gameNameBuffer, sizeof(gameNameBuffer));
        if (isRunning) {
            ImGui::EndDisabled();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (!isRunning) {
            if (ImGui::Button("Start Activity", ImVec2(150, 36))) {
                processManager.StartGame(gameNameBuffer);
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("Stop Activity", ImVec2(150, 36))) {
                processManager.StopGame();
            }
            ImGui::PopStyleColor();
        }

        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();

        if (isRunning) {
            std::string timeStr = FormatDuration(processManager.GetElapsedTime());
            ImGui::TextColored(
                ImVec4(0.2f, 0.85f, 0.3f, 1.0f),
                "Status: Active -> Playing %s.exe (%s)",
                processManager.GetCurrentGameName().c_str(),
                timeStr.c_str()
            );
        } else {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Status: Idle");
        }

        if (!processManager.GetLastErrorMessage().empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: %s", processManager.GetLastErrorMessage().c_str());
        }

        ImGui::End();
    });

    return 0;
}
