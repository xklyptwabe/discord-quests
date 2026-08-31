#pragma once

#include <windows.h>
#include <string>
#include <filesystem>
#include <chrono>
#include <optional>

namespace core {

class ProcessManager {
public:
    ProcessManager();
    ~ProcessManager();

    // Disable copy semantics
    ProcessManager(const ProcessManager&) = delete;
    ProcessManager& operator=(const ProcessManager&) = delete;

    bool StartGame(const std::string& gameName);
    bool StopGame();

    bool IsRunning() const noexcept;
    std::string GetCurrentGameName() const;
    std::filesystem::path GetCurrentExecutablePath() const;
    std::chrono::seconds GetElapsedTime() const;
    std::string GetLastErrorMessage() const;

private:
    void CleanupProcessHandles();
    std::filesystem::path GetStubSourcePath() const;
    std::filesystem::path GetWorkingDir() const;
    static std::string SanitizeFilename(const std::string& input);

private:
    HANDLE m_processHandle = nullptr;
    HANDLE m_threadHandle = nullptr;
    HANDLE m_jobHandle = nullptr;
    DWORD m_processId = 0;

    std::string m_currentGameName;
    std::filesystem::path m_currentExePath;
    std::optional<std::chrono::steady_clock::time_point> m_startTime;
    std::string m_lastError;
};

}