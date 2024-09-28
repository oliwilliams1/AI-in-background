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
    Sleep(5000);
	CleanupTrayIcon();

    return 0;
}