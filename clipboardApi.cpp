#include "clipboardApi.h"

std::string GetClipboardText() {
// Open the clipboard
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open clipboard." << std::endl;
        return "";
    }

    // Get the handle to the clipboard data
    HGLOBAL hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        std::cerr << "No text data in clipboard." << std::endl;
        CloseClipboard();
        return "";
    }

    // Lock the global memory to get a pointer to the data
    char* pData = static_cast<char*>(GlobalLock(hData));
    if (pData == nullptr) {
        std::cerr << "Failed to lock global memory." << std::endl;
        CloseClipboard();
        return "";
    }

    // Create a string from the clipboard data
    std::string clipboardText(pData);

    // Unlock the global memory
    GlobalUnlock(hData);

    // Close the clipboard
    CloseClipboard();

    return clipboardText;
}

bool SetClipboardText(std::string text) {
    // Open the clipboard
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open clipboard." << std::endl;
        return false;
    }

    // Empty the clipboard
    EmptyClipboard();

    // Allocate global memory for the text
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (hGlobal == nullptr) {
        std::cerr << "Failed to allocate global memory." << std::endl;
        CloseClipboard();
        return false;
    }

    // Lock the global memory to get a pointer to the data
    char* pGlobal = static_cast<char*>(GlobalLock(hGlobal));
    if (pGlobal == nullptr) {
        std::cerr << "Failed to lock global memory." << std::endl;
        GlobalFree(hGlobal);
        CloseClipboard();
        return false;
    }

    // Copy the text to the global memory
    strcpy_s(pGlobal, text.size() + 1, text.c_str());

    // Unlock the global memory
    GlobalUnlock(hGlobal);

    // Set the clipboard data
    if (SetClipboardData(CF_TEXT, hGlobal) == nullptr) {
        std::cerr << "Failed to set clipboard data." << std::endl;
        GlobalFree(hGlobal);
        CloseClipboard();
        return false;
    }

    // Close the clipboard
    CloseClipboard();

    return true;
}