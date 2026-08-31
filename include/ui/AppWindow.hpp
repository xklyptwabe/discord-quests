#pragma once

#include <windows.h>
#include <d3d11.h>
#include <string>
#include <functional>

namespace ui {

class AppWindow {
public:
    AppWindow();
    ~AppWindow();

    AppWindow(const AppWindow&) = delete;
    AppWindow& operator=(const AppWindow&) = delete;

    bool Initialize(const std::wstring& title, int width = 640, int height = 480);
    void Run(const std::function<void()>& renderCallback);
    void Shutdown();

    HWND GetHwnd() const noexcept { return m_hwnd; }

private:
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

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
};

} 
