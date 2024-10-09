#pragma once
#include <iostream>
#include <windows.h>
#include <chrono>

constexpr auto KEY = 0xC0; // Key code for `

static HHOOK keyboardHook;
static std::chrono::steady_clock::time_point keyPressTime;
static bool keyPressed = false;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void SetHook();
void ReleaseHook();
void onKeyRelease(double duration); // Defined in main.cpp!! so cool right?