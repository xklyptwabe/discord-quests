# 🎮 Discord Quests & Activity Emulator

<div align="center">

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![DirectX](https://img.shields.io/badge/Renderer-DirectX%2011-000000?style=for-the-badge)
![UI](https://img.shields.io/badge/GUI-Dear%20ImGui-blueviolet?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![CI](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge&logo=githubactions&logoColor=white)

**A modern, lightweight Windows C++20 application to emulate game processes and custom Rich Presence status for Discord Quests.**

</div>

---

## ✨ Features

- **🎯 Dual Emulation Engines**:
  - **Native Process Emulation (`.exe`)**: Spawns an ultra-low-overhead dummy process named `<GameName>.exe` detected by Discord's native game scanner without needing special developer keys. Perfect for Discord Quests completion.
  - **Discord Rich Presence (IPC)**: Directly sends custom activity status (`details`, `state`, start timestamp, custom icons) through local Windows Named Pipes (`\\.\pipe\discord-ipc-0`).
  - **Hybrid Mode**: Simultaneously launches the dummy process and broadcasts Rich Presence data.
- **⚡ Zero Overhead & Safe**:
  - Background stub consumes 0% CPU and minimal RAM.
  - Guarded by **Windows Job Objects** (`JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE`), ensuring no orphaned background processes remain on exit or crash.
- **🎨 Discord Dark Theme UI**:
  - Styled with Dear ImGui and hardware-accelerated DirectX 11 rendering.
  - Per-Monitor High-DPI Awareness v2 and Windows 10/11 immersive dark title bar support.
- **📥 System Tray Integration**:
  - Minimize to Windows System Tray (`Shell_NotifyIconW`).
  - Tray context menu (Open / Exit) and double-click to restore.
- **💾 Presets & History**:
  - Automatically remembers recently played games and custom RPC configurations in `%APPDATA%/DiscordQuests/config.json`.

---

## 🛠️ Architecture

```mermaid
flowchart TD
    UI[Dear ImGui GUI + DirectX 11] --> Controller[Application Controller]
    Controller --> Config[ConfigManager<br/>%APPDATA%/DiscordQuests]
    Controller --> ProcMgr[ProcessManager<br/>Job Object + Win32]
    Controller --> RpcMgr[DiscordRpcClient<br/>Named Pipe IPC]
    
    ProcMgr -->|Creates & Spawns| Dummy[<GameName>.exe<br/>0% CPU Stub]
    RpcMgr -->|JSON Frames| DiscordPipe[\\\\.\\pipe\\discord-ipc-X]
    
    Dummy -.->|Detected by| Discord[Discord Desktop Client]
    DiscordPipe -.->|Rich Presence| Discord
```

---

## 🚀 Getting Started

### Prerequisites
- **OS**: Windows 10 / Windows 11 (64-bit)
- **Compiler**: Visual Studio 2022 / 2026 (MSVC with C++20 support) or Clang
- **Build Tool**: CMake 3.20+

### Building from Source

```powershell
# 1. Clone the repository
git clone https://github.com/your-username/discord-quests.git
cd discord-quests

# 2. Configure project with CMake
cmake -B build

# 3. Build Release binary
cmake --build build --config Release

# 4. Run application
.\build\bin\Release\DiscordQuests.exe
```

---

## 🕹️ Usage Guide

1. **Launch `DiscordQuests.exe`**.
2. **Select Mode**:
   - `Process Emulation`: For native Discord game detection and Quests.
   - `Discord RPC`: For rich status with custom details and timestamps.
   - `Hybrid`: For both at the same time.
3. **Enter Game Name** (e.g. `Grand Theft Auto VI`, `Cyberpunk 2077`, `Minecraft`).
4. Click **`START ACTIVITY`**.
5. Discord will immediately show your status as **"Playing `<GameName>`"**.
6. When finished, click **`STOP ACTIVITY`** or close the app.

---

## 📂 Project Structure

```text
discord-quests/
├── .github/
│   └── workflows/
│       └── ci.yml               # GitHub Actions CI/CD pipeline
├── include/
│   ├── core/
│   │   ├── ConfigManager.hpp    # JSON configuration manager
│   │   ├── DiscordRpcClient.hpp # Native Discord Named Pipe IPC client
│   │   └── ProcessManager.hpp   # Win32 process spawning & Job Objects
│   └── ui/
│       ├── AppWindow.hpp        # Win32 + DirectX 11 + Tray window
│       └── Theme.hpp            # Discord-inspired ImGui theme
├── src/
│   ├── core/
│   │   ├── ConfigManager.cpp
│   │   ├── DiscordRpcClient.cpp
│   │   └── ProcessManager.cpp
│   ├── ui/
│   │   └── AppWindow.cpp
│   ├── dummy_stub.cpp           # Zero-overhead background dummy executable
│   └── main.cpp                 # Main GUI entry point
├── .gitignore
├── CMakeLists.txt               # Modern CMake configuration (C++20)
├── CMakePresets.json            # Build presets for VS Code / Visual Studio
├── CONTRIBUTING.md              # Contribution guidelines
├── LICENSE                      # MIT License
└── README.md
```

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.
