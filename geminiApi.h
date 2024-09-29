#pragma once
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

// Function to perform a GET request and return the response as a string
std::string requestGemini(std::string request, const std::string& apiKey);