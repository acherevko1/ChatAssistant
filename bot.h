#include "telegram.h"
#include "openai.h"
#include "database.h"

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>


class Bot {
public:
    Bot(const std::string& bot_token, const std::string& openai_token);
    void Run();

private:
    void ProcessMessages(const std::vector<Telegram::Message>& messages);
    bool BotMentioned(const std::string& text);
    std::string StripMention(const std::string message);
    std::vector<OpenAI::Message> CreateOpenAIMessages(const std::vector<Telegram::Message> messages);

private:
    Telegram::Client telegram_client_;
    OpenAI::Client openai_client_;
    Database database_;
    std::unordered_map<int64_t, std::vector<Telegram::Message>> chat_histories_;
    std::unordered_map<int64_t, std::string> last_openai_message_id_;
};