#include <string>
#include <vector>

namespace OpenAI {

struct Response {
    std::string response_message_id;
    std::string text;
};

struct Message {
    std::string role;
    std::string username;
    std::string text;
};

class Client {
public:
    explicit Client(const std::string& apiKey);
    Response SendMessages(std::vector<Message> messages, std::string previousMessageId);
    
private:
    std::string api_key;
};

} // namespace OpenAI