# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ChatAssistant is a Telegram bot that responds to @ mentions in group chats using OpenAI API. The bot loads chat history as context and provides intelligent responses.

### Core Features
- Responds to @ mentions in Telegram group chats
- Uses OpenAI API for generating responses
- Maintains chat history for context
- Event-driven architecture (no polling/sleeping)

## Architecture

### High-Level Design
- **main.cpp**: Simple entry point that connects components
- **Bot class**: Handles all message processing logic
- **TelegramClient**: Event-driven message receiving 
- **Config**: Loads API keys and settings

### Key Design Decisions
- Event-driven approach: `TelegramClient.WaitForMessages()` blocks efficiently until messages arrive
- Simple main loop: `for (message : messages) { bot.ProcessMessage(message); }`
- All bot logic encapsulated in `Bot.ProcessMessage()` method
- No polling or sleeping - uses OS-level socket waiting

## Coding Standards

- **Function Names**: Use PascalCase (e.g., `LoadConfig()`, `ProcessMessage()`)
- **Language**: C++23 standard
- **Architecture**: Event-driven, modular design
- **Development**: Incremental changes with manual review/approval

## File Structure

```
ChatAssistant/
├── main.cpp          # Entry point
├── config.hpp        # Configuration and API keys
├── telegram.hpp      # Telegram client and messaging
├── bot.hpp           # Core bot logic
└── CLAUDE.md         # This file
```

## Development Workflow

- Make small, incremental changes
- Present code for review before implementing
- Focus on clean, simple design
- Use event-driven patterns over polling