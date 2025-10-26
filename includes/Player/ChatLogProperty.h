#pragma once

#include <Windows.h>
#include "Player/ChatMessage.h"
#include <deque>
#include <functional>
#include <map>
#include <string>
#include <regex>

// Forward declarations
struct PlayerProcessInfo;

// Property for monitoring chat log from memory
class ChatLogProperty
{
public:
    using ChatCallback = std::function<void(DWORD procId, const ChatMessage&)>;

    ChatLogProperty();

    // Register callback for new chat messages
    void RegisterCallback(ChatCallback callback);
    void UnregisterCallback();

    // Called by monitoring loop to read new chat messages
    void refresh(const PlayerProcessInfo& processInfo);

private:
    // Chat buffer memory address: FFXiMain.dll + 0x00128AD4 + 0x10
    static const DWORD CHAT_LOG_BASE = 0x00128AD4;
    static const DWORD CHAT_LOG_OFFSET = 0x10;
    static const int CHAT_BUFFER_SIZE = 4096; // Size to read

    // Track last chat content per process to detect changes
    std::map<DWORD, std::string> lastChatContent;

    // Chat callback
    ChatCallback chatCallback;

    // Regex patterns for chat type detection (Lua patterns converted to C++ regex)
    struct ChatPattern {
        std::regex pattern;
        ChatMessageType type;
    };
    std::vector<ChatPattern> chatPatterns;

    // Initialize regex patterns
    void initializePatterns();

    // Parse chat line and determine type
    ChatMessage ParseChatLine(const std::string& line);

    // Extract message type from line using regex patterns
    ChatMessageType DetermineChatType(const std::string& line);

    // Helper to remove FFXI special characters from strings
    std::string CleanFFXIString(const std::string& input);
};
