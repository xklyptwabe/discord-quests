#include "ui/AppWindow.hpp"
#include "ui/Theme.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <dwmapi.h>
#include <iostream>

// Forward declare Win32 message handler from ImGui backend
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ui {

AppWindow::AppWindow() = default;

AppWindow::~AppWindow() {
    Shutdown();
}

bool AppWindow::CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &sd,
        &m_pSwapChain,
        &m_d3dDevice,
        &featureLevel,
        &m_d3dDeviceContext
    );

    if (hr == DXGI_ERROR_UNSUPPORTED) {
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            createDeviceFlags,
            featureLevelArray,
            2,
            D3D11_SDK_VERSION,
            &sd,
            &m_pSwapChain,
            &m_d3dDevice,
            &featureLevel,
            &m_d3dDeviceContext
        );
    }

    if (FAILED(hr)) {
        return false;
    }

    CreateRenderTarget();
    return true;
}

void AppWindow::CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (m_pSwapChain) {
        m_pSwapChain->Release();
        m_pSwapChain = nullptr;
    }
    if (m_d3dDeviceContext) {
        m_d3dDeviceContext->Release();
        m_d3dDeviceContext = nullptr;
    }
    if (m_d3dDevice) {
        m_d3dDevice->Release();
        m_d3dDevice = nullptr;
    }
}

void AppWindow::CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer = nullptr;
    m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer) {
        m_d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
        pBackBuffer->Release();
    }
}

void AppWindow::CleanupRenderTarget() {
    if (m_mainRenderTargetView) {
        m_mainRenderTargetView->Release();
        m_mainRenderTargetView = nullptr;
    }
}

void AppWindow::InitTrayIcon() {
    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize = sizeof(NOTIFYICONDATAW);
    m_nid.hWnd = m_hwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;
    m_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcsncpy_s(m_nid.szTip, m_title.c_str(), _TRUNCATE);

    Shell_NotifyIconW(NIM_ADD, &m_nid);
    m_trayIconAdded = true;
}

void AppWindow::RemoveTrayIcon() {
    if (m_trayIconAdded) {
        Shell_NotifyIconW(NIM_DELETE, &m_nid);
        m_trayIconAdded = false;
    }
}

void AppWindow::MinimizeToTray() {
    ShowWindow(m_hwnd, SW_HIDE);
}

void AppWindow::RestoreFromTray() {
    ShowWindow(m_hwnd, SW_SHOW);
    ShowWindow(m_hwnd, SW_RESTORE);
    SetForegroundWindow(m_hwnd);
}

void AppWindow::ShowTrayMenu() {
    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_SHOW, L"Open Discord Quests");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

    SetForegroundWindow(m_hwnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hwnd, nullptr);
    DestroyMenu(hMenu);
}

LRESULT WINAPI AppWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }

    auto* app = reinterpret_cast<AppWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED) {
            if (app && app->m_minimizeToTrayOnClose) {
                app->MinimizeToTray();
                return 0;
            }
        }
        if (app && app->m_d3dDevice) {
            app->m_resizeWidth = (UINT)LOWORD(lParam);
            app->m_resizeHeight = (UINT)HIWORD(lParam);
        }
        return 0;

    case WM_TRAYICON:
        if (LOWORD(lParam) == WM_LBUTTONDBLCLK || LOWORD(lParam) == WM_LBUTTONUP) {
            if (app) {
                app->RestoreFromTray();
            }
        } else if (LOWORD(lParam) == WM_RBUTTONUP) {
            if (app) {
                app->ShowTrayMenu();
            }
        }
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_TRAY_SHOW:
            if (app) {
                app->RestoreFromTray();
            }
            break;
        case ID_TRAY_EXIT:
            PostQuitMessage(0);
            break;
        }
        return 0;

    case WM_CLOSE:
        if (app && app->m_minimizeToTrayOnClose) {
            app->MinimizeToTray();
            return 0;
        }
        break;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) {
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool AppWindow::Initialize(const std::wstring& title, int width, int height) {
    m_title = title;
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    m_wc = {
        sizeof(m_wc),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        GetModuleHandle(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        L"DiscordQuestsWindowClass",
        nullptr
    };

    RegisterClassExW(&m_wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - width) / 2;
    int posY = (screenHeight - height) / 2;

    m_hwnd = CreateWindowW(
        m_wc.lpszClassName,
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        posX,
        posY,
        width,
        height,
        nullptr,
        nullptr,
        m_wc.hInstance,
        nullptr
    );

    if (!m_hwnd) {
        return false;
    }

    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(m_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

    if (!CreateDeviceD3D(m_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
        return false;
    }

    InitTrayIcon();

    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Apply custom Discord theme
    ApplyDiscordTheme();

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_d3dDevice, m_d3dDeviceContext);

    return true;
}

void AppWindow::Run(const std::function<void()>& renderCallback) {
    bool done = false;
    while (!done) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                done = true;
            }
        }
        if (done) {
            break;
        }

        if (m_resizeWidth != 0 && m_resizeHeight != 0) {
            CleanupRenderTarget();
            m_pSwapChain->ResizeBuffers(0, m_resizeWidth, m_resizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            m_resizeWidth = m_resizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (renderCallback) {
            renderCallback();
        }

        ImGui::Render();
        const float clear_color[4] = { 0.118f, 0.122f, 0.133f, 1.00f }; // #1E1F22
        m_d3dDeviceContext->OMSetRenderTargets(1, &m_mainRenderTargetView, nullptr);
        m_d3dDeviceContext->ClearRenderTargetView(m_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        HRESULT hr = m_pSwapChain->Present(1, 0);
        m_swapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }
}

void AppWindow::Shutdown() {
    RemoveTrayIcon();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();

    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    if (m_wc.hInstance) {
        UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
        m_wc.hInstance = nullptr;
    }
}

} // namespace ui
