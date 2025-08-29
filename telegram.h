#include <string>
#include <vector>
#include <cstdint>

namespace Telegram {

struct Message {
    int64_t message_id;
    int64_t chat_id;
    std::string username;
    std::string text;
    int64_t timestamp;
};

class Client {
public:
    explicit Client(const std::string& botToken);
    ~Client();
    
    std::vector<Message> WaitForMessages();
    void SendMessage(int64_t chatId, const std::string& text);
    
private:
    std::string bot_token;
    int64_t last_update_id;
};

} // namespace Telegram