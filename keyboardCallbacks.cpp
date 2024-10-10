#include "keyboardCallbacks.h"

glm::vec2 GetMousePos() {
    POINT mousePos;
    GetCursorPos(&mousePos);
    return glm::vec2(mousePos.x, mousePos.y);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) {
            // Check if the primary key is pressed
            if (pKeyboard->vkCode == PRIMARY_KEY) {
                bool secondaryKeyDown = GetAsyncKeyState(SECONDARY_KEY) & 0x8000;

                if (secondaryKeyDown) {
                    // Toggle recording
                    UpdateTrayIcon(C_STATE_RECORDING_STROKES);
                    recordingStrokes = true;
                    secondaryKeyPressed = !secondaryKeyPressed;
                }

                if (!secondaryKeyDown && !keyPressed) {
                    // Record mouse position and time only if the key is not already pressed
                    mouseDownPos = GetMousePos();
                    keyPressTime = std::chrono::steady_clock::now();
                    keyPressed = true;
                }
            }
            else {
                // Must be a keystroke, add to list
                if (recordingStrokes) {
                    char buffer[2] = { 0 };
                    BYTE keyboardState[256];
                    BOOL b = GetKeyboardState(keyboardState);

                    int result = ToAsciiEx(pKeyboard->vkCode, pKeyboard->scanCode, keyboardState, (LPWORD)buffer, 0, GetKeyboardLayout(0));
                    if (result > 0) {
                        strokes += buffer[0];
                    }
                }
            }
        }
        else if (wParam == WM_KEYUP) {
            // Check if the primary key is released
            if (pKeyboard->vkCode == PRIMARY_KEY) {
                // Check if the secondary key is down (changes mode)
                bool secondaryKeyDown = GetAsyncKeyState(SECONDARY_KEY) & 0x8000;

                if (!secondaryKeyPressed && secondaryKeyDown) {
                    // If the secondary key was pressed, finalize the strokes
                    secondaryKeyPressed = false;
                    recordingStrokes = false;
                    keystrokeResponse(strokes);
                    strokes.clear(); // Clear strokes after processing
                }
                // Handle key release only if it was previously pressed
                if (keyPressed && !secondaryKeyDown && !recordingStrokes) {
                    mouseUpPos = GetMousePos();
                    auto keyReleaseTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double, std::milli> duration = keyReleaseTime - keyPressTime;

                    onKeyRelease(duration.count(), mouseDownPos, mouseUpPos);
                    keyPressed = false; // Reset the keyPressed state
                }
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