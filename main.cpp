#include <windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>

void capturePrimaryScreen(const std::string& filename) {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    // Get dimensions of primary screen
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    // Create a bitmap
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);

    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    // Create an OpenCV Mat from the bitmap
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    // Create a Mat to hold the image
    cv::Mat mat(bmp.bmHeight, bmp.bmWidth, CV_8UC4);
    GetBitmapBits(hBitmap, bmp.bmHeight * bmp.bmWidthBytes, mat.data);

    // Convert BGRA to BGR
    cv::cvtColor(mat, mat, cv::COLOR_BGRA2BGR);

    // Save the image to disk
    cv::imwrite(filename, mat);

    // Clean up
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

int main() {
    std::string filename = "primary_screenshot.png";
    capturePrimaryScreen(filename);
    std::cout << "Primary screen screenshot saved as " << filename << std::endl;
    return 0;
}