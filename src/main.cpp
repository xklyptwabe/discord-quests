#include "ui/AppWindow.hpp"
#include <imgui.h>
#include <iostream>

int main() {
    ui::AppWindow window;
    if (!window.Initialize(L"Discord Quests & Activity Emulator", 650, 480)) {
        std::cerr << "[ERROR] Failed to initialize application window." << std::endl;
        return 1;
    }

    // Buffer for target game name input
    char gameNameBuffer[128] = "Grand Theft Auto VI";
    bool isRunning = false;

    window.Run([&]() {
        // Stretch ImGui window over the whole client area
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
        ImGui::InputText("##GameName", gameNameBuffer, sizeof(gameNameBuffer));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (!isRunning) {
            if (ImGui::Button("Start Activity", ImVec2(140, 36))) {
                isRunning = true;
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("Stop Activity", ImVec2(140, 36))) {
                isRunning = false;
            }
            ImGui::PopStyleColor();
        }

        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        if (isRunning) {
            ImGui::TextColored(ImVec4(0.2f, 0.85f, 0.3f, 1.0f), "Status: Active -> Playing %s.exe", gameNameBuffer);
        } else {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Status: Idle");
        }

        ImGui::End();
    });

    return 0;
}
