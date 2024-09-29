#include "geminiApi.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string requestGemini(std::string request, const std::string& apiKey) {
    CURL* curl;
    CURLcode res;
    std::string response;

    // Construct the URL with the API key
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + apiKey;

    // JSON payload
    std::string jsonData = R"({
        "contents": [{
            "parts": [{"text": ")" + request + R"("}]
        }]
    })";

    curl = curl_easy_init();
    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Specify that we are sending a POST request
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the content type header
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the JSON data to send
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (const char*)jsonData.c_str());

        // Set the write callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            response.clear();  // Clear response on error
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    else {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }

    json j;
    j = json::parse(response);

	response = j["candidates"][0]["content"]["parts"][0]["text"];
    return response;
}