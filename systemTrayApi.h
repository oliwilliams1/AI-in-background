#pragma once
#include <windows.h>
#include <shellapi.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <glm/vec3.hpp>
#include <iostream>

static glm::vec3 C_STATE_READY             = glm::vec3(0,   255, 0);
static glm::vec3 C_STATE_AWAITING_RESPONSE = glm::vec3(255, 255, 0);
static glm::vec3 C_STATE_SCREENSHOTTING    = glm::vec3(255, 0, 255);
static glm::vec3 C_STATE_AWATING_OCR       = glm::vec3(0,   0, 255);
static glm::vec3 C_STATE_RECORDING_STROKES = glm::vec3(255, 0, 255);

void InitializeTrayIcon(HINSTANCE hInstance);
void CleanupTrayIcon();
void SetTrayIcon(const cv::Mat& image);
void UpdateTrayIcon(glm::vec3 colour);