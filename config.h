#include <string>

struct Config {
    std::string bot_token;
    std::string openai_token;
};

Config LoadConfig();
