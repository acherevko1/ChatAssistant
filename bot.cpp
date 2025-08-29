#include "bot.h"
#include <string>

Bot::Bot(const std::string& bot_token, const std::string& openai_token) : telegram_client_(bot_token), openai_client_(openai_token) {
}

void Bot::Run() {
    while (true) {
        ProcessMessages(telegram_client_.WaitForMessages());
    }
}

void Bot::ProcessMessages(const std::vector<Telegram::Message>& messages) {
    for (auto message : messages) {
        chat_histories_[message.chat_id].push_back(message);

        if (BotMentioned(message.text)) {
            chat_histories_[message.chat_id].back().text = StripMention(chat_histories_[message.chat_id].back().text);
            auto openai_messages = CreateOpenAIMessages(chat_histories_[message.chat_id]);
            auto reply = openai_client_.SendMessages(openai_messages, last_openai_message_id_[message.chat_id]);

            chat_histories_[message.chat_id].clear();
            last_openai_message_id_[message.chat_id] = reply.response_message_id;
            telegram_client_.SendMessage(message.chat_id, reply.text);
        }
    }
}

bool Bot::BotMentioned(const std::string& text) {
    return text.find("@krisa_assistant_bot") != std::string::npos;
}

std::string Bot::StripMention(const std::string message) {
    std::string mention = "@krisa_assistant_bot";
    auto mention_length = mention.size();
    auto mention_pos = message.find(mention);
    return message.substr(0, mention_pos) + message.substr(mention_pos + mention_length);
}

std::vector<OpenAI::Message> Bot::CreateOpenAIMessages(const std::vector<Telegram::Message> messages) {
    std::vector<OpenAI::Message> result;

    for (const auto& message : messages) {
        result.push_back({ "user", message.username, message.text });
    }
    
    return result;
}