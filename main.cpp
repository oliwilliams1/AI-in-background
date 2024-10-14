#include <iostream>
#include <unordered_set>
#include <windows.h>
#include <chrono>
#include <regex>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>
#include "screenshotApi.h"
#include "geminiApi.h"
#include "clipboardApi.h"
#include "keyboardCallbacks.h"

const std::string API_KEY = "AIzaSyAjtbqg0T4aTbqDnJPl8kIlRH9ZYr6v32g";

std::string stripMarkdown(const std::string& input) {
    std::string output = input;

    // Remove headers (###, ##, #)
    output = std::regex_replace(output, std::regex(R"(^#{1,6}\s*)"), "");

    // Remove bold (**) and (__) formatting
    output = std::regex_replace(output, std::regex(R"(\*\*(.*?)\*\*|__(.*?)__)"), "$1$2");

    // Remove italic (*) and (_) formatting
    output = std::regex_replace(output, std::regex(R"(\*(.*?)\*|_(.*?)_)"), "$1$2");

    // Remove inline code (``)
    output = std::regex_replace(output, std::regex(R"(`(.*?)`)"), "$1");

    // Remove links [text](url)
    output = std::regex_replace(output, std::regex(R"(\[.*?\]\(.*?\))"), "");

    // Remove images ![alt](url)
    output = std::regex_replace(output, std::regex(R"(!\[.*?\]\(.*?\))"), "");

    // Remove blockquotes (>)
    output = std::regex_replace(output, std::regex(R"(>\s*)"), "");

    // Remove lists (*) and (-)
    output = std::regex_replace(output, std::regex(R"(^\s*[\*\-]\s*)"), "");

    return output;
}

// Fast patch to a promblem I cant be bothered to dig into
static std::string serializeResponse(const std::string& input) {
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
    
    return stripMarkdown(output);
}

static std::string performOCR(const cv::Mat& image) {
    // Create a Tesseract OCR engine instance
    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();

    const char* dataPath = "./tessdata";
    // Init tesseract with english
    if (ocr->Init(dataPath, "eng")) {
        UpdateTrayIcon(C_STATE_ERROR);
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

static void basicInteration() {
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

static void ocrInteraction(cv::Rect roi) {
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

    // Set ocr result to clipboard
    SetClipboardText(ocrResult);

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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    InitializeTrayIcon(hInstance);

    UpdateTrayIcon(C_STATE_READY);

    SetHook();

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CleanupTrayIcon();
    ReleaseHook();
    return 0;
}

int main(HINSTANCE hInstance) {
    InitializeTrayIcon(hInstance);

    UpdateTrayIcon(C_STATE_READY);

    SetHook();

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CleanupTrayIcon();
    ReleaseHook();
    return 0;
}