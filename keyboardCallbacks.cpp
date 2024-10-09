#include "keyboardCallbacks.h"

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