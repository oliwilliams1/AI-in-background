#pragma once
#include <iostream>
#include <windows.h>
#include <chrono>
#include <glm/vec2.hpp>
#include "systemTrayApi.h"

constexpr auto PRIMARY_KEY = 0xC0; // Key code for `
constexpr auto SECONDARY_KEY = 0x10; // Key code for {shift}

static glm::vec2 mouseDownPos;
static glm::vec2 mouseUpPos;

static HHOOK keyboardHook;
static std::chrono::steady_clock::time_point keyPressTime;
static bool keyPressed = false;
static bool secondaryKeyPressed = false;
static bool recordingStrokes = false;
static std::string strokes = "";

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void SetHook();
void ReleaseHook();
void keystrokeResponse(const std::string& strokes);
void onKeyRelease(double duration, const glm::vec2& p1, const glm::vec2& p2); // Defined in main.cpp!! so cool right?