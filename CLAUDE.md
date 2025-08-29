# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ChatAssistant is a Telegram bot that responds to @ mentions in group chats using OpenAI API. The bot loads chat history as context and provides intelligent responses.

### Core Features
- Responds to @ mentions in Telegram group chats
- Uses OpenAI API for generating responses with conversation continuity
- RAM-based chat history storage (per-chat, unlimited growth)
- SQLite persistence for OpenAI conversation IDs across restarts
- Event-driven architecture (no polling/sleeping)

## Architecture

### High-Level Design
- **main.cpp**: Simple entry point that connects components
- **Bot class**: Handles all message processing logic
- **Telegram::Client**: Event-driven message receiving with RAM-based history storage
- **OpenAI::Client**: API client with conversation chaining support
- **Database**: SQLite storage for persisting OpenAI conversation IDs
- **Config**: Loads API keys and settings

### Key Design Decisions
- Event-driven approach: `Telegram::Client.WaitForMessages()` blocks efficiently until messages arrive
- Simple main loop: `for (message : messages) { bot.ProcessMessage(message); }`
- All bot logic encapsulated in `Bot.ProcessMessage()` method
- No polling or sleeping - uses OS-level socket waiting
- RAM-only chat storage: Messages stored in `std::unordered_map<int64_t, std::vector<Message>>` per chat_id
- Conversation continuity: OpenAI conversation IDs persisted in SQLite across bot restarts
- Security-focused: No sensitive chat data persisted to disk (except conversation IDs)

## Coding Standards

- **Function Names**: Use PascalCase (e.g., `LoadConfig()`, `ProcessMessage()`)
- **Language**: C++23 standard
- **Architecture**: Event-driven, modular design
- **Development**: Incremental changes with manual review/approval

## File Structure

```
ChatAssistant/
├── main.cpp          # Entry point
├── config.h/.cpp     # Configuration and API keys
├── telegram.h/.cpp   # Telegram client with RAM-based message storage
├── openai.h/.cpp     # OpenAI API client with conversation chaining
├── bot.h/.cpp        # Core bot logic and message processing
├── database.h/.cpp   # SQLite storage for conversation persistence
├── .vscode/          # VSCode debug configuration
└── CLAUDE.md         # This file
```

## Development Workflow

- Make small, incremental changes
- Present code for review before implementing
- Focus on clean, simple design
- Use event-driven patterns over polling

## Dependencies

- **libcurl**: HTTP requests to Telegram and OpenAI APIs
- **nlohmann/json**: JSON parsing and generation
- **sqlite3**: Persistent storage for conversation IDs
- **CMake**: Build system with debug configuration

## Security Model

- **RAM-only chat storage**: Messages never persisted to disk
- **Minimal disk persistence**: Only OpenAI conversation IDs stored in SQLite
- **Automatic cleanup**: All chat history lost on restart (by design)
- **No API key logging**: Sensitive data excluded from logs