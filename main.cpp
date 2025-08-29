#include "bot.h"
#include "config.h"

int main() {
    auto config = LoadConfig();
    
    Bot bot(config.bot_token, config.openai_token);
    bot.Run();
    
    return 0;
}