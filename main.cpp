#include <iostream>
#include <unordered_set>
#include <windows.h>
#include <chrono>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>
#include "screenshotApi.h"
#include "geminiApi.h"
#include "clipboardApi.h"
#include "keyboardCallbacks.h"

const std::string API_KEY = "AIzaSyAjtbqg0T4aTbqDnJPl8kIlRH9ZYr6v32g";

// Fast patch to a promblem I cant be bothered to dig into
std::string serializeResponse(const std::string& input) {
    std::string output;

    // Numerous charecters that fish out weird ones from the response
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

std::string performOCR(const cv::Mat& image) {
    // Create a Tesseract OCR engine instance
    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();

    // Init tesseract with english
    if (ocr->Init(NULL, "eng")) {
        std::cerr << "Could not initialize tesseract." << std::endl;
        return "";
    }

    ocr->SetImage(image.data, image.cols, image.rows, 1, image.step[0]);

    // Perform OCR
    char* outText = ocr->GetUTF8Text();
    std::string result(outText);

    // Clean up
    delete[] outText;
    ocr->End();

    return result;
}

void keystrokeResponse(const std::string& strokes) {
    // Indicate application state via icon
	UpdateTrayIcon(C_STATE_AWAITING_RESPONSE);

	// Send off request
	std::string response = requestGemini(strokes, API_KEY);

	// Remove weird artifacts from response
	response = serializeResponse(response);

	// Set clipboard
	SetClipboardText(response);

    // Indicate application state via icon
	UpdateTrayIcon(C_STATE_READY);
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

    // Conv to grey-scale
    cv::Mat grayImage;
    cv::cvtColor(croppedScreenshot, grayImage, cv::COLOR_BGR2GRAY);

    // Apply thresholds
    cv::Mat binaryImage;
    cv::threshold(grayImage, binaryImage, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Resize (easier for tesseract)
    cv::Mat resizedImage;
    cv::resize(binaryImage, resizedImage, cv::Size(), 2.0, 2.0);

    // Ocr cropped
    UpdateTrayIcon(C_STATE_AWATING_OCR);
    std::string ocrResult = performOCR(resizedImage);

    // Send ocr response off to gemini
    UpdateTrayIcon(C_STATE_AWAITING_RESPONSE);
    std::string response = requestGemini(ocrResult, API_KEY);

    // Remove wierd artifacts fro response
    response = serializeResponse(response);
    SetClipboardText(response);

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