#include "telegram.h"
#include <curl/curl.h>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <iostream>

namespace Telegram {

using json = nlohmann::json;

struct CurlResponse {
    std::string data;
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, CurlResponse* response) {
    size_t total_size = size * nmemb;
    response->data.append(static_cast<char*>(contents), total_size);
    return total_size;
}

Client::Client(const std::string& botToken) 
    : bot_token(botToken), last_update_id(0) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

Client::~Client() {
    curl_global_cleanup();
}


std::vector<Message> Client::WaitForMessages() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    
    // Build getUpdates URL with long polling
    std::string url = "https://api.telegram.org/bot" + bot_token + "/getUpdates";
    url += "?timeout=60&offset=" + std::to_string(last_update_id + 1);
    
    CurlResponse response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    // Perform HTTP request (libcurl handles efficient blocking internally)
    CURLcode result = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (result != CURLE_OK) {
        throw std::runtime_error("HTTP request failed: " + std::string(curl_easy_strerror(result)));
    }
    
    // Parse JSON response
    std::vector<Message> messages;
    try {
        json response_json = json::parse(response.data);
        
        if (!response_json["ok"]) {
            throw std::runtime_error("Telegram API error");
        }
        
        for (const auto& update : response_json["result"]) {
            if (update.contains("message")) {
                const auto& msg = update["message"];

                std::cout << msg << std::endl;
                
                Message message;
                message.message_id = msg["message_id"];
                message.chat_id = msg["chat"]["id"];
                message.username = msg["from"].value("username", "");
                if (message.username.empty()) {
                    message.username = msg["from"]["first_name"];
                }
                message.text = msg.value("text", "");
                message.timestamp = msg["date"];
                
                messages.push_back(message);
                last_update_id = std::max(last_update_id, static_cast<int64_t>(update["update_id"]));
            }
        }
    } catch (const json::exception& e) {
        throw std::runtime_error("JSON parsing error: " + std::string(e.what()));
    }
    
    return messages;
}

void Client::SendMessage(int64_t chatId, const std::string& text) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    
    std::string url = "https://api.telegram.org/bot" + bot_token + "/sendMessage";
    
    json payload;
    payload["chat_id"] = chatId;
    payload["text"] = text;
    
    std::string post_data = payload.dump();
    CurlResponse response;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    CURLcode result = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (result != CURLE_OK) {
        throw std::runtime_error("Failed to send message: " + std::string(curl_easy_strerror(result)));
    }
}

} // namespace Telegram