#pragma once
#include <iostream>
#include <Windows.h>

std::string GetClipboardText();

bool SetClipboardText(std::string text);