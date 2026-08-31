#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // Keep process alive until terminated by parent or system signal
    HANDLE hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (hEvent) {
        WaitForSingleObject(hEvent, INFINITE);
        CloseHandle(hEvent);
    }
    return 0;
}
