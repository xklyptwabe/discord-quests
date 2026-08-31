#include "core/DiscordRpcClient.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>

namespace core {

namespace {

constexpr uint32_t OPCODE_HANDSHAKE = 0;
constexpr uint32_t OPCODE_FRAME = 1;
constexpr uint32_t OPCODE_CLOSE = 2;
constexpr uint32_t OPCODE_PING = 3;
constexpr uint32_t OPCODE_PONG = 4;

#pragma pack(push, 1)
struct PipeHeader {
    uint32_t opcode;
    uint32_t length;
};
#pragma pack(pop)

}

DiscordRpcClient::DiscordRpcClient() = default;

DiscordRpcClient::~DiscordRpcClient() {
    Disconnect();
}

bool DiscordRpcClient::OpenPipe() {
    ClosePipe();

    // Iterate through Discord IPC pipe slots 0 to 9
    for (int i = 0; i < 10; ++i) {
        std::wstring pipePath = L"\\\\.\\pipe\\discord-ipc-" + std::to_wstring(i);

        HANDLE handle = CreateFileW(
            pipePath.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr
        );

        if (handle != INVALID_HANDLE_VALUE) {
            DWORD mode = PIPE_READMODE_BYTE;
            SetNamedPipeHandleState(handle, &mode, nullptr, nullptr);
            m_pipeHandle = handle;
            m_pipeIndex = i;
            return true;
        }
    }

    m_lastError = "Discord client not found. Ensure Discord is running.";
    return false;
}

void DiscordRpcClient::ClosePipe() {
    if (m_pipeHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_pipeHandle);
        m_pipeHandle = INVALID_HANDLE_VALUE;
    }
    m_pipeIndex = -1;
}

bool DiscordRpcClient::SendFrame(uint32_t opcode, const std::string& payload) {
    if (m_pipeHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    PipeHeader header;
    header.opcode = opcode;
    header.length = static_cast<uint32_t>(payload.size());

    DWORD bytesWritten = 0;
    BOOL res = WriteFile(m_pipeHandle, &header, sizeof(header), &bytesWritten, nullptr);
    if (!res || bytesWritten != sizeof(header)) {
        return false;
    }

    if (!payload.empty()) {
        res = WriteFile(m_pipeHandle, payload.data(), static_cast<DWORD>(payload.size()), &bytesWritten, nullptr);
        if (!res || bytesWritten != payload.size()) {
            return false;
        }
    }

    return true;
}

bool DiscordRpcClient::ReadFrame(uint32_t& outOpcode, std::string& outPayload) {
    if (m_pipeHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    PipeHeader header;
    DWORD bytesRead = 0;
    BOOL res = ReadFile(m_pipeHandle, &header, sizeof(header), &bytesRead, nullptr);
    if (!res || bytesRead != sizeof(header)) {
        return false;
    }

    outOpcode = header.opcode;
    outPayload.clear();

    if (header.length > 0) {
        std::vector<char> buffer(header.length);
        DWORD totalRead = 0;
        while (totalRead < header.length) {
            res = ReadFile(
                m_pipeHandle,
                buffer.data() + totalRead,
                header.length - totalRead,
                &bytesRead,
                nullptr
            );
            if (!res || bytesRead == 0) {
                return false;
            }
            totalRead += bytesRead;
        }
        outPayload.assign(buffer.data(), totalRead);
    }

    return true;
}

bool DiscordRpcClient::Connect(const std::string& clientId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastError.clear();
    m_clientId = clientId;

    if (m_clientId.empty()) {
        m_lastError = "Client ID cannot be empty.";
        return false;
    }

    if (!OpenPipe()) {
        return false;
    }

    // Send handshake JSON payload
    nlohmann::json handshakeJson = {
        {"v", 1},
        {"client_id", m_clientId}
    };

    if (!SendFrame(OPCODE_HANDSHAKE, handshakeJson.dump())) {
        ClosePipe();
        m_lastError = "Failed to send handshake frame to Discord pipe.";
        return false;
    }

    // Read handshake response
    uint32_t respOpcode = 0;
    std::string respPayload;
    if (!ReadFrame(respOpcode, respPayload)) {
        ClosePipe();
        m_lastError = "Failed to receive handshake response from Discord.";
        return false;
    }

    m_connected = true;
    return true;
}

void DiscordRpcClient::Disconnect() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_connected) {
        ClearActivity();
        SendFrame(OPCODE_CLOSE, "{}");
        ClosePipe();
        m_connected = false;
    }
}

bool DiscordRpcClient::IsConnected() const noexcept {
    return m_connected;
}

void DiscordRpcClient::SetActivity(const DiscordActivity& activity) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_connected || m_pipeHandle == INVALID_HANDLE_VALUE) {
        return;
    }

    nlohmann::json actObj = nlohmann::json::object();

    if (!activity.state.empty()) {
        actObj["state"] = activity.state;
    }
    if (!activity.details.empty()) {
        actObj["details"] = activity.details;
    }
    if (activity.startTimestamp.has_value()) {
        actObj["timestamps"] = {
            {"start", *activity.startTimestamp}
        };
    }

    nlohmann::json assets = nlohmann::json::object();
    if (!activity.largeImageKey.empty()) {
        assets["large_image"] = activity.largeImageKey;
    }
    if (!activity.largeImageText.empty()) {
        assets["large_text"] = activity.largeImageText;
    }
    if (!activity.smallImageKey.empty()) {
        assets["small_image"] = activity.smallImageKey;
    }
    if (!activity.smallImageText.empty()) {
        assets["small_text"] = activity.smallImageText;
    }

    if (!assets.empty()) {
        actObj["assets"] = assets;
    }

    nlohmann::json payload = {
        {"cmd", "SET_ACTIVITY"},
        {"args", {
            {"pid", static_cast<int>(GetCurrentProcessId())},
            {"activity", actObj}
        }},
        {"nonce", std::to_string(std::chrono::system_clock::now().time_since_epoch().count())}
    };

    if (!SendFrame(OPCODE_FRAME, payload.dump())) {
        m_connected = false;
        ClosePipe();
    }
}

void DiscordRpcClient::ClearActivity() {
    if (!m_connected || m_pipeHandle == INVALID_HANDLE_VALUE) {
        return;
    }

    nlohmann::json payload = {
        {"cmd", "SET_ACTIVITY"},
        {"args", {
            {"pid", static_cast<int>(GetCurrentProcessId())},
            {"activity", nullptr}
        }},
        {"nonce", std::to_string(std::chrono::system_clock::now().time_since_epoch().count())}
    };

    SendFrame(OPCODE_FRAME, payload.dump());
}

std::string DiscordRpcClient::GetLastError() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_lastError;
}

} 
