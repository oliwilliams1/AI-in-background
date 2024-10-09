#pragma once
#include <iostream>
#include <windows.h>
#include <chrono>
#include <glm/vec2.hpp>

constexpr auto KEY = 0xC0; // Key code for `

static glm::vec2 mouseDownPos;
static glm::vec2 mouseUpPos;

static HHOOK keyboardHook;
static std::chrono::steady_clock::time_point keyPressTime;
static bool keyPressed = false;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void SetHook();
void ReleaseHook();
void onKeyRelease(double duration, const glm::vec2& p1, const glm::vec2& p2); // Defined in main.cpp!! so cool right?