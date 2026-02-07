
#ifdef GEODE_IS_WINDOWS

#include "windowsUtils.hpp"

HWND hwnd_window = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD windowPid = 0;
    GetWindowThreadProcessId(hwnd, &windowPid);

    DWORD currentPid = GetCurrentProcessId();
    if (windowPid == currentPid && hwnd != GetConsoleWindow()) {
        hwnd_window = hwnd;
        return FALSE;
    }
    return TRUE;
}

HWND GDH::WindowUtils::FindMainHWND() {
    EnumWindows(EnumWindowsProc, 0);
    return hwnd_window;
}

#endif