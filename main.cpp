#include <iostream>
#include <unordered_set>
#include <windows.h>
#include <chrono>
#include <glm/vec3.hpp>
#include "screenshotApi.h"
#include "geminiApi.h"
#include "systemTrayApi.h"
#include "clipboardApi.h"
#include "keyboardCallbacks.h"

glm::vec3 C_STATE_READY = glm::vec3(0, 255, 0);
glm::vec3 C_STATE_AWAITING_RESPONSE = glm::vec3(255, 255, 0);

const std::string API_KEY = "AIzaSyAjtbqg0T4aTbqDnJPl8kIlRH9ZYr6v32g";

std::string serializeResponse(const std::string& input) {
    std::string output;

    std::unordered_set<char> allowedCharecters = {
        '"', '\'', ',', '.', '?', '!', '+', '_', ')', '(', '*', '&', '^',
        '%', '$', '#', '@', '{', '}', '[', ']', '\\', '|', '/', '<', '>',
        '-', '='
    };

    for (char ch : input) {
        // Checkif alphanumeric, whitespace, or allowed charecters
        if (std::isalnum(static_cast<unsigned char>(ch)) ||
            std::isspace(static_cast<unsigned char>(ch)) ||
            allowedCharecters.find(ch) != allowedCharecters.end()) {
            output += ch;
        }
    }
    
    return output;
}

void basicInteration() {
    // Get question from clipboard
    std::string text = GetClipboardText();

    // Tell user request has been sent off
    UpdateTrayIcon(C_STATE_AWAITING_RESPONSE);

    // Send off request
    std::string response = requestGemini(text, API_KEY);

    // Remove weird artifacts from response
	response = serializeResponse(response);

    // Place response in clipboard
    SetClipboardText(response);

    // Tell user response is ready to view
    UpdateTrayIcon(C_STATE_READY);
}

void onKeyRelease(double duration) {
    if (duration < 200) {
        basicInteration();
    }
    else {
        // IMPL screenshot ocr
    };
}

int main() {
    SetHook();
    InitializeTrayIcon(GetModuleHandle(NULL));

	UpdateTrayIcon(C_STATE_READY);

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