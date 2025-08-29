#include "openai.h"
#include <curl/curl.h>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <iostream>

namespace OpenAI {

using json = nlohmann::json;

struct CurlResponse {
    std::string data;
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, CurlResponse* response) {
    size_t total_size = size * nmemb;
    response->data.append(static_cast<char*>(contents), total_size);
    return total_size;
}

Client::Client(const std::string& apiKey) : api_key(apiKey) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

Response Client::SendMessages(std::vector<Message> messages, std::string previousMessageId) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    // Build messages array from chat history
    json json_messages = json::array();
    
    // Build request payload
    json payload = {
        {"model", "gpt-5-nano"}
    };

    // Add previous_response_id if not -1 (conversation continuation)
    if (!previousMessageId.empty()) {
        payload["previous_response_id"] = previousMessageId;
    }  else {
        // Add system message
        json_messages.push_back({
        {"role", "system"},
        {"content", "You are a helpful assistant in a group chat. Respond naturally to questions. If you want to mention someone - use @ followed by their username."}
        });
    }
    
    // Convert chat history to OpenAI format
    for (const auto& msg : messages) {
        json_messages.push_back({
            {"role", "user"},
            {"content", msg.username + ": " + msg.text}
        });
    }

    payload["input"] = json_messages;

    std::cout << payload << "\n";

    std::string post_data = payload.dump();
    CurlResponse response;

    // Set up CURL
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/responses");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Set headers
    struct curl_slist* headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform request
    CURLcode result = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        throw std::runtime_error("HTTP request failed: " + std::string(curl_easy_strerror(result)));
    }

    // Parse response
    try {
        json response_json = json::parse(response.data);
        std::cout << response_json << "\n";
        
        if (response_json.contains("error") && !response_json["error"].is_null()) {
            std::cout << response_json["error"] << "\n";
            std::cout << "ya dolboeb\n";
            throw std::runtime_error("OpenAI API error: " + response_json["error"]["message"].get<std::string>());
        }

        // std::cout << response_json << "\n";

        Response openai_response;
        openai_response.text = response_json["output"][1]["content"][0]["text"];
        
        // Extract response_message_id if present (for conversation chaining)
        if (response_json.contains("id")) {
            openai_response.response_message_id = response_json["id"];
        } else {
            openai_response.response_message_id = "";
        }

        return openai_response;
        
    } catch (const json::exception& e) {
        throw std::runtime_error("JSON parsing error: " + std::string(e.what()));
    }
}

} // namespace OpenAI