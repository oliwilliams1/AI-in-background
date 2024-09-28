#include <iostream>
#include "screenshotApi.h"
#include "geminiApi.h"
#include "systemTrayApi.h"

const std::string API_KEY = "AIzaSyAjtbqg0T4aTbqDnJPl8kIlRH9ZYr6v32g";

int main() {
    // std::string filename = "primary_screenshot.png";
    // captureScreen(filename);
    // std::cout << "Primary screen screenshot saved as " << filename << std::endl;
    
    //std::cout << requestGemini(API_KEY);

    InitializeTrayIcon(GetModuleHandle(NULL));

    cv::Mat mat = cv::Mat(32, 32, CV_8UC3, cv::Scalar(255, 0, 0));
    SetTrayIcon(mat);
    Sleep(3000);

    mat = cv::Mat(32, 32, CV_8UC3, cv::Scalar(0, 255, 0));
    SetTrayIcon(mat);
    Sleep(3000);

    mat = cv::Mat(32, 32, CV_8UC3, cv::Scalar(0, 0, 255));
    SetTrayIcon(mat);
    Sleep(3000);

	CleanupTrayIcon();

    return 0;
}