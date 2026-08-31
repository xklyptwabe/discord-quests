#include <windows.h>
#include <string>

// Window procedure for the dummy game window
LRESULT CALLBACK DummyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int) {
    // Determine game window title from command line or default
    std::wstring windowTitle = (pCmdLine && wcslen(pCmdLine) > 0) ? pCmdLine : L"Roblox";

    // Strip wrapping quotes if passed
    if (windowTitle.front() == L'"' && windowTitle.back() == L'"' && windowTitle.length() >= 2) {
        windowTitle = windowTitle.substr(1, windowTitle.length() - 2);
    }

    // Register a dummy game window class
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DummyWndProc;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = L"DiscordGameDummyClass";

    RegisterClassExW(&wcex);

    // Create a real top-level window so Discord's EnumWindows scanner recognizes it as an active game
    HWND hWnd = CreateWindowExW(
        0,
        wcex.lpszClassName,
        windowTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        -32000, -32000, // Placed offscreen to remain unobtrusive
        100, 100,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hWnd) {
        ShowWindow(hWnd, SW_SHOWNOACTIVATE);
        UpdateWindow(hWnd);
    }

    // Message loop consuming 0% CPU
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
