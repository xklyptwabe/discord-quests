#include "core/ProcessManager.hpp"
#include <algorithm>
#include <system_error>

namespace core {

ProcessManager::ProcessManager() {
    // Create job object to guarantee child process termination on app exit
    m_jobHandle = CreateJobObjectW(nullptr, nullptr);
    if (m_jobHandle) {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {};
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(
            m_jobHandle,
            JobObjectExtendedLimitInformation,
            &jeli,
            sizeof(jeli)
        );
    }
}

ProcessManager::~ProcessManager() {
    StopGame();
    if (m_jobHandle) {
        CloseHandle(m_jobHandle);
        m_jobHandle = nullptr;
    }
}

std::string ProcessManager::SanitizeFilename(const std::string& input) {
    // Strip illegal Windows filename characters: < > : " / \ | ? *
    const std::string illegalChars = "<>:\"/\\|?*";
    std::string result;
    result.reserve(input.size());

    for (char c : input) {
        if (illegalChars.find(c) == std::string::npos && static_cast<unsigned char>(c) >= 32) {
            result.push_back(c);
        }
    }

    // Trim trailing spaces and dots
    while (!result.empty() && (result.back() == ' ' || result.back() == '.')) {
        result.pop_back();
    }

    return result;
}

std::filesystem::path ProcessManager::GetWorkingDir() const {
    wchar_t tempPath[MAX_PATH];
    DWORD len = GetTempPathW(MAX_PATH, tempPath);
    if (len == 0 || len > MAX_PATH) {
        return std::filesystem::current_path() / "games";
    }
    return std::filesystem::path(tempPath) / "DiscordQuests";
}

std::filesystem::path ProcessManager::GetStubSourcePath() const {
    wchar_t modulePath[MAX_PATH];
    GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
    std::filesystem::path exeDir = std::filesystem::path(modulePath).parent_path();
    return exeDir / "discord_dummy_stub.exe";
}

bool ProcessManager::StartGame(const std::string& gameName) {
    m_lastError.clear();

    if (IsRunning()) {
        StopGame();
    }

    std::string sanitized = SanitizeFilename(gameName);
    if (sanitized.empty()) {
        m_lastError = "Game name cannot be empty or contain only invalid characters.";
        return false;
    }

    // Ensure .exe extension for process filename
    std::string exeName = sanitized;
    if (exeName.length() < 4 || exeName.substr(exeName.length() - 4) != ".exe") {
        exeName += ".exe";
    }

    // Extract raw title without .exe for window caption
    std::string gameTitle = sanitized;
    if (gameTitle.length() >= 4 && gameTitle.substr(gameTitle.length() - 4) == ".exe") {
        gameTitle = gameTitle.substr(0, gameTitle.length() - 4);
    }

    std::filesystem::path workingDir = GetWorkingDir();
    std::error_code ec;
    std::filesystem::create_directories(workingDir, ec);
    if (ec) {
        m_lastError = "Failed to create working directory: " + ec.message();
        return false;
    }

    std::filesystem::path targetExePath = workingDir / exeName;
    std::filesystem::path stubPath = GetStubSourcePath();

    if (!std::filesystem::exists(stubPath)) {
        m_lastError = "Stub executable not found at: " + stubPath.string();
        return false;
    }

    // Copy stub to target game executable path
    std::filesystem::copy_file(
        stubPath,
        targetExePath,
        std::filesystem::copy_options::overwrite_existing,
        ec
    );
    if (ec) {
        m_lastError = "Failed to create game executable: " + ec.message();
        return false;
    }

    STARTUPINFOW si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    // Convert game title to wide string for command line argument
    int wlen = MultiByteToWideChar(CP_UTF8, 0, gameTitle.c_str(), -1, nullptr, 0);
    std::wstring wGameTitle(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, gameTitle.c_str(), -1, wGameTitle.data(), wlen);

    std::wstring commandLine = L"\"" + targetExePath.wstring() + L"\" \"" + wGameTitle + L"\"";

    BOOL success = CreateProcessW(
        targetExePath.c_str(),
        commandLine.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        workingDir.c_str(),
        &si,
        &pi
    );

    if (!success) {
        DWORD err = ::GetLastError();
        m_lastError = "CreateProcess failed with error code: " + std::to_string(err);
        return false;
    }

    m_processHandle = pi.hProcess;
    m_threadHandle = pi.hThread;
    m_processId = pi.dwProcessId;
    m_currentGameName = gameTitle;
    m_currentExePath = targetExePath;
    m_startTime = std::chrono::steady_clock::now();

    // Assign process to Job Object for automatic lifecycle cleanup
    if (m_jobHandle) {
        AssignProcessToJobObject(m_jobHandle, m_processHandle);
    }

    return true;
}

bool ProcessManager::StopGame() {
    if (!IsRunning()) {
        CleanupProcessHandles();
        return true;
    }

    if (m_processHandle) {
        TerminateProcess(m_processHandle, 0);
        WaitForSingleObject(m_processHandle, 2000);
    }

    CleanupProcessHandles();

    // Attempt to remove temporary executable
    if (!m_currentExePath.empty()) {
        std::error_code ec;
        std::filesystem::remove(m_currentExePath, ec);
    }

    m_currentGameName.clear();
    m_currentExePath.clear();
    m_startTime.reset();
    return true;
}

void ProcessManager::CleanupProcessHandles() {
    if (m_threadHandle) {
        CloseHandle(m_threadHandle);
        m_threadHandle = nullptr;
    }
    if (m_processHandle) {
        CloseHandle(m_processHandle);
        m_processHandle = nullptr;
    }
    m_processId = 0;
}

bool ProcessManager::IsRunning() const noexcept {
    if (!m_processHandle) {
        return false;
    }

    DWORD exitCode = 0;
    if (GetExitCodeProcess(m_processHandle, &exitCode)) {
        return (exitCode == STILL_ACTIVE);
    }

    return false;
}

std::string ProcessManager::GetCurrentGameName() const {
    return m_currentGameName;
}

std::filesystem::path ProcessManager::GetCurrentExecutablePath() const {
    return m_currentExePath;
}

std::chrono::seconds ProcessManager::GetElapsedTime() const {
    if (!m_startTime.has_value() || !IsRunning()) {
        return std::chrono::seconds(0);
    }

    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - *m_startTime);
}

std::string ProcessManager::GetLastErrorMessage() const {
    return m_lastError;
}

} // namespace core
