cmake_minimum_required(VERSION 3.20)

project(
    DiscordQuests
    VERSION 1.0.0
    DESCRIPTION "Discord Custom Activity & Game Process Emulator"
    LANGUAGES CXX
)

# Standard C++20 configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Binary output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Minimal background stub executable
add_executable(discord_dummy_stub
    src/dummy_stub.cpp
)
target_compile_features(discord_dummy_stub PRIVATE cxx_std_20)
if(MSVC)
    target_link_options(discord_dummy_stub PRIVATE /SUBSYSTEM:WINDOWS)
endif()

# Fetch Dear ImGui
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

# Fetch nlohmann_json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# ImGui static library with Win32 and DirectX 11 backends
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# Main executable
add_executable(DiscordQuests
    src/main.cpp
    src/ui/AppWindow.cpp
    include/ui/AppWindow.hpp
    src/core/ProcessManager.cpp
    include/core/ProcessManager.hpp
    src/core/DiscordRpcClient.cpp
    include/core/DiscordRpcClient.hpp
    src/core/ConfigManager.cpp
    include/core/ConfigManager.hpp
)

add_dependencies(DiscordQuests discord_dummy_stub)

target_include_directories(DiscordQuests PRIVATE
    include
)

target_link_libraries(DiscordQuests PRIVATE
    imgui_lib
    nlohmann_json::nlohmann_json
    d3d11
    dxgi
    d3dcompiler
    dwmapi
)

target_compile_features(DiscordQuests PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(DiscordQuests PRIVATE
        /W4
        /permissive-
        /utf-8
        /MP
    )
    # Configure entry point for Win32 (console in Debug, window in Release)
    target_link_options(DiscordQuests PRIVATE
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
    )
else()
    target_compile_options(DiscordQuests PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
