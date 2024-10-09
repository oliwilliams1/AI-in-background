#pragma once
#include <windows.h>
#include <shellapi.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <glm/vec3.hpp>
#include <iostream>

void InitializeTrayIcon(HINSTANCE hInstance);
void CleanupTrayIcon();
void SetTrayIcon(const cv::Mat& image);
void UpdateTrayIcon(glm::vec3 colour);