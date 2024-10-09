#include <iostream>
#include <unordered_set>
#include <windows.h>
#include <chrono>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "screenshotApi.h"
#include "geminiApi.h"
#include "systemTrayApi.h"
#include "clipboardApi.h"
#include "keyboardCallbacks.h"

glm::vec3 C_STATE_READY = glm::vec3(0, 255, 0);
glm::vec3 C_STATE_AWAITING_RESPONSE = glm::vec3(255, 255, 0);
glm::vec3 C_STATE_SCREENSHOTTING = glm::vec3(255, 0, 255);
glm::vec3 C_STATE_AWATING_OCR = glm::vec3(0, 0, 255);

const std::string API_KEY = "AIzaSyAjtbqg0T4aTbqDnJPl8kIlRH9ZYr6v32g";

// Fast patch to a promblem I cant be bothered to dig into
std::string serializeResponse(const std::string& input) {
    std::string output;

    // Numerous charecters that fish out weird ones from the response i.e. 😄
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

    // Indicate application state via icon
    UpdateTrayIcon(C_STATE_AWAITING_RESPONSE);

    // Send off request
    std::string response = requestGemini(text, API_KEY);

    // Remove weird artifacts from response
	response = serializeResponse(response);
    SetClipboardText(response);

    // Indicate application state via icon
    UpdateTrayIcon(C_STATE_READY);
}

void ocrInteraction(cv::Rect roi) {
    // Indicate application state via icon
    UpdateTrayIcon(C_STATE_SCREENSHOTTING);

    // Take screenshot & save to local mat
    cv::Mat screenshot = captureScreen();

    // Crop image
    cv::Mat croppedScreenshot = screenshot(roi).clone();

    cv::imshow("Screenshot", croppedScreenshot);

    // After everything is done, return to ready staate
    UpdateTrayIcon(C_STATE_READY);
}

void onKeyRelease(double duration, const glm::vec2& p1, const glm::vec2& p2) {
    if (duration < 200) {
        basicInteration();
    }
    else {
        // Get roi based on two mouse positions
        cv::Rect roi(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
            std::abs(p1.x - p2.x), std::abs(p1.y - p2.y));

        ocrInteraction(roi);
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