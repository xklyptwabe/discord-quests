#pragma once

#include <windows.h>
#include <shellapi.h>
#include <d3d11.h>
#include <string>
#include <functional>

namespace ui {

constexpr UINT WM_TRAYICON = WM_USER + 1;
constexpr UINT ID_TRAY_SHOW = 1001;
constexpr UINT ID_TRAY_EXIT = 1002;

class AppWindow {
public:
    AppWindow();
    ~AppWindow();

    // Disable copy semantics
    AppWindow(const AppWindow&) = delete;
    AppWindow& operator=(const AppWindow&) = delete;

    bool Initialize(const std::wstring& title, int width = 680, int height = 520);
    void Run(const std::function<void()>& renderCallback);
    void Shutdown();

    void MinimizeToTray();
    void RestoreFromTray();
    void SetMinimizeToTrayOnClose(bool enable) noexcept { m_minimizeToTrayOnClose = enable; }
    bool IsMinimizeToTrayOnClose() const noexcept { return m_minimizeToTrayOnClose; }

    HWND GetHwnd() const noexcept { return m_hwnd; }

private:
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

    void InitTrayIcon();
    void RemoveTrayIcon();
    void ShowTrayMenu();

    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwnd = nullptr;
    WNDCLASSEXW m_wc = {};
    
    ID3D11Device* m_d3dDevice = nullptr;
    ID3D11DeviceContext* m_d3dDeviceContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;
    UINT m_resizeWidth = 0;
    UINT m_resizeHeight = 0;
    bool m_swapChainOccluded = false;

    NOTIFYICONDATAW m_nid = {};
    bool m_trayIconAdded = false;
    bool m_minimizeToTrayOnClose = true;
    std::wstring m_title;
};

} // namespace ui
