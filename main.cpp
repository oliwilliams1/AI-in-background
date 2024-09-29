#include <iostream>
#include <windows.h>
#include <chrono>
#include "screenshotApi.h"
#include "geminiApi.h"
#include "systemTrayApi.h"
#include "clipboardApi.h"

constexpr auto KEY = 0xC0; // Key code for '`'

const std::string API_KEY = "AIzaSyAjtbqg0T4aTbqDnJPl8kIlRH9ZYr6v32g";
HHOOK keyboardHook;

std::chrono::steady_clock::time_point keyPressTime;
bool keyPressed = false;



void onKeyRelease(double duration) {
    if (duration < 200) {
        std::string text = GetClipboardText();
        std::cout << "Request: " << text << std::endl;
        std::string response = requestGemini(text, API_KEY);
        SetClipboardText(response);
        std::cout << "Response: " << response << std::endl;
    }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) {
            // Record the time when the key is pressed
            if (!keyPressed && pKeyboard->vkCode == KEY) {
                keyPressTime = std::chrono::steady_clock::now();
                keyPressed = true;
            }
        }
        else if (wParam == WM_KEYUP) {
            // Calculate the duration when the key is released
            if (keyPressed && pKeyboard->vkCode == KEY) {
                auto keyReleaseTime = std::chrono::steady_clock::now();
                std::chrono::duration<double, std::milli> duration = keyReleaseTime - keyPressTime;

                onKeyRelease(duration.count());
                keyPressed = false;
            }
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void SetHook() {
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    if (!keyboardHook) {
        std::cerr << "Failed to set hook!" << std::endl;
    }
}

void ReleaseHook() {
    UnhookWindowsHookEx(keyboardHook);
}

void UpdateTrayIcon() {
    cv::Mat mat;
    for (int i = 0; i < 3; ++i) {
        mat = cv::Mat(32, 32, CV_8UC3, CV_RGB((i % 2) * 255, ((i + 1) % 2) * 255, (i % 3) * 255));
        SetTrayIcon(mat);
        Sleep(3000); // Wait for 3 seconds
    }
}

int main() {
    SetHook();
    InitializeTrayIcon(GetModuleHandle(NULL));

    // Message loop for keyboard events
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    CleanupTrayIcon();
    ReleaseHook();
    return 0;
}