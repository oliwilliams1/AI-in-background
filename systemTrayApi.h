#pragma once
#include <windows.h>
#include <shellapi.h>
#include <opencv2/core.hpp>
#include <iostream>

void InitializeTrayIcon(HINSTANCE hInstance);
void CleanupTrayIcon();
void SetTrayIcon(const cv::Mat& image);