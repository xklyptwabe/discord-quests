#pragma once

#include <windows.h>
#include <string>
#include <chrono>
#include <optional>
#include <cstdint>
#include <atomic>
#include <thread>
#include <mutex>

namespace core {

struct DiscordActivity {
    std::string state;
    std::string details;
    std::optional<int64_t> startTimestamp;
    std::string largeImageKey;
    std::string largeImageText;
    std::string smallImageKey;
    std::string smallImageText;
};

class DiscordRpcClient {
public:
    DiscordRpcClient();
    ~DiscordRpcClient();

    // Disable copy semantics
    DiscordRpcClient(const DiscordRpcClient&) = delete;
    DiscordRpcClient& operator=(const DiscordRpcClient&) = delete;

    bool Connect(const std::string& clientId);
    void Disconnect();

    bool IsConnected() const noexcept;
    void SetActivity(const DiscordActivity& activity);
    void ClearActivity();

    std::string GetLastError() const;

private:
    bool OpenPipe();
    void ClosePipe();
    bool SendFrame(uint32_t opcode, const std::string& payload);
    bool ReadFrame(uint32_t& outOpcode, std::string& outPayload);

private:
    HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
    std::string m_clientId;
    std::atomic<bool> m_connected = false;
    mutable std::mutex m_mutex;
    std::string m_lastError;
    int m_pipeIndex = -1;
};

} 