#pragma once

#include <imgui.h>

namespace ui {

inline void ApplyDiscordTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Discord-inspired modern color palette
    colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.95f, 0.96f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.58f, 0.61f, 0.64f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.12f, 0.12f, 0.13f, 1.00f); // #1E1F22
    colors[ImGuiCol_ChildBg]               = ImVec4(0.17f, 0.18f, 0.19f, 1.00f); // #2B2D31
    colors[ImGuiCol_PopupBg]               = ImVec4(0.17f, 0.18f, 0.19f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.24f, 0.25f, 0.27f, 0.50f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.19f, 0.20f, 0.22f, 1.00f); // #313338
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.23f, 0.24f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.27f, 0.29f, 0.32f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.12f, 0.12f, 0.13f, 0.75f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.12f, 0.12f, 0.13f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.24f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.31f, 0.33f, 0.36f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.38f, 0.40f, 0.44f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.35f, 0.40f, 0.95f, 1.00f); // #5865F2 (Blurple)
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.35f, 0.40f, 0.95f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.28f, 0.32f, 0.77f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.35f, 0.40f, 0.95f, 1.00f); // Blurple
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.28f, 0.32f, 0.77f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.24f, 0.27f, 0.65f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.24f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.31f, 0.33f, 0.36f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.35f, 0.40f, 0.95f, 1.00f);
    colors[ImGuiCol_Separator]             = ImVec4(0.24f, 0.25f, 0.27f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.35f, 0.40f, 0.95f, 0.78f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.35f, 0.40f, 0.95f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.24f, 0.25f, 0.27f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.35f, 0.40f, 0.95f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.35f, 0.40f, 0.95f, 0.95f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.24f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_TabSelected]           = ImVec4(0.24f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_TabDimmed]             = ImVec4(0.12f, 0.12f, 0.13f, 0.97f);
    colors[ImGuiCol_TabDimmedSelected]     = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);

    // Padding & geometry rounding
    style.WindowPadding    = ImVec2(16.0f, 16.0f);
    style.FramePadding     = ImVec2(10.0f, 6.0f);
    style.ItemSpacing      = ImVec2(10.0f, 10.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.WindowRounding   = 10.0f;
    style.ChildRounding    = 8.0f;
    style.FrameRounding    = 6.0f;
    style.PopupRounding    = 6.0f;
    style.ScrollbarRounding= 6.0f;
    style.GrabRounding     = 5.0f;
    style.TabRounding      = 6.0f;
}

} // namespace ui
