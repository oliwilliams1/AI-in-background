#include "systemTrayApi.h"
#include <opencv2/highgui.hpp>

#define WM_TRAYICON (WM_USER + 1)

// Global variables
HINSTANCE hInst;
NOTIFYICONDATA nid;

// Window procedure declaration
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

cv::Mat icon = cv::imread("icon.bmp", cv::IMREAD_UNCHANGED);

void InitializeTrayIcon(HINSTANCE hInstance) {
    hInst = hInstance;

    // Register the window class
    const wchar_t CLASS_NAME[] = L"SampleWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"System Tray Icon",
        0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    // Initialize NOTIFYICONDATA
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Load a default icon
    wcscpy_s(nid.szTip, L"Tray Icon Example");

    // Add the icon to the system tray
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void CleanupTrayIcon() {
    // Remove the icon before exiting
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

HICON CreateHICONFromMat(const cv::Mat& image) {
    // Ensure the image is in the correct format and size
    if (image.empty() || image.cols != 32 || image.rows != 32 || image.type() != CV_8UC4) {
        return NULL; // Expecting 32-bit ARGB image
    }

    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = image.cols;
    bi.biHeight = -image.rows; // Negative to indicate a top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 32; // ARGB
    bi.biCompression = BI_RGB;

    // Create a compatible DC and a bitmap
    HDC hdc = GetDC(NULL);
    void* pPixels = NULL;
    HBITMAP hBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pPixels, NULL, 0);
    ReleaseDC(NULL, hdc);

    // Check if the bitmap was created successfully
    if (!hBitmap) {
        return NULL;
    }

    // Copy the image data to the bitmap
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            cv::Vec4b pixel = image.at<cv::Vec4b>(y, x); // Use CV_8UC4 for 32-bit images
            BYTE* destPixel = static_cast<BYTE*>(pPixels) + (y * image.cols + x) * 4; // 4 bytes for ARGB
            destPixel[0] = pixel[0]; // B
            destPixel[1] = pixel[1]; // G
            destPixel[2] = pixel[2]; // R
            destPixel[3] = pixel[3]; // A (Alpha channel)
        }
    }

    // Create an icon from the bitmap
    HICON hIcon = CreateIcon(GetModuleHandle(NULL), 32, 32, 1, 32, NULL, (BYTE*)pPixels);

    // Clean up the bitmap
    DeleteObject(hBitmap);

    return hIcon;
}

void SetTrayIcon(const cv::Mat& image) {
    HICON hIcon = CreateHICONFromMat(image);
    
    if (hIcon) {
        nid.hIcon = hIcon;
        Shell_NotifyIcon(NIM_MODIFY, &nid);
        std::cout << "Tray icon updated." << std::endl;
        DestroyIcon(hIcon); // Clean up the icon
    }
}

void UpdateTrayIcon(glm::vec3 colour) {
    cv::Mat mat;
    mat = icon.clone();

    for (int y = 0; y < icon.rows; ++y) {
        for (int x = 0; x < icon.cols; ++x) {
            cv::Vec3b& pixel = mat.at<cv::Vec3b>(y, x);
            if (pixel[2] > 150 && pixel[1] < 200 && pixel[0] < 200) {
                pixel = cv::Vec3b(colour.x, colour.y, colour.z);
            }
        }
    }

    SetTrayIcon(mat);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_LBUTTONDBLCLK) {
            // Handle left double click (if needed)
            MessageBox(hwnd, L"Tray Icon Double Clicked!", L"Notification", MB_OK);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}