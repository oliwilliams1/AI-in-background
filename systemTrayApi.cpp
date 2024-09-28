#include "systemTrayApi.h"

#define WM_TRAYICON (WM_USER + 1)

// Global variables
HINSTANCE hInst;
NOTIFYICONDATA nid;

// Window procedure declaration
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void InitializeTrayIcon(HINSTANCE hInstance) {
    hInst = hInstance;

    // Register the window class
    const wchar_t CLASS_NAME[] = L"SampleWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"System Tray Icon",
        0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    // Initialize NOTIFYICONDATA
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Load a default icon
    wcscpy_s(nid.szTip, L"Tray Icon Example");

    // Add the icon to the system tray
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void CleanupTrayIcon() {
    // Remove the icon before exiting
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_LBUTTONDBLCLK) {
            // Handle left double click (if needed)
            MessageBox(hwnd, L"Tray Icon Double Clicked!", L"Notification", MB_OK);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}