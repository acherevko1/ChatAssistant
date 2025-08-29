#include "config.h"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Config LoadConfig() {
    std::ifstream config_file(".config.json");
    if (!config_file.is_open()) {
        throw std::runtime_error("Unable to open .config.json file. Please create it with your bot token.");
    }
    
    json config_json;
    try {
        config_file >> config_json;
    } catch (const json::exception& e) {
        throw std::runtime_error("Invalid JSON in .config.json: " + std::string(e.what()));
    }
    
    Config config;
    
    if (!config_json.contains("bot_token") || !config_json["bot_token"].is_string()) {
        throw std::runtime_error("Missing or invalid 'bot_token' field in .config.json");
    }
    
    config.bot_token = config_json["bot_token"];
    
    if (config.bot_token.empty()) {
        throw std::runtime_error("Bot token cannot be empty");
    }

    if (!config_json.contains("openai_token") || !config_json["openai_token"].is_string()) {
        throw std::runtime_error("Missing or invalid 'openai_token' field in .config.json");
    }
    
    config.openai_token = config_json["openai_token"];
    
    if (config.openai_token.empty()) {
        throw std::runtime_error("Openai token cannot be empty");
    }
    
    return config;
}