#pragma once

#include <string>
#include <ctime>

enum class ChatMessageType {
    Say = 0,
    Shout = 1,
    Tell = 2,
    Party = 3,
    Linkshell1 = 4,
    Linkshell2 = 5,
    Yell = 6,
    Unity = 7,
    Drops = 8,
    Obtained = 9,
    Trial = 10,
    System = 11,
    Emote = 12,
    Unknown = 99
};

struct ChatMessage {
    std::string message;
    std::string sender;
    std::string rawContent;  // Original formatted message (e.g., "(Piplup) Hello" or "Piplup : Hello")
    ChatMessageType type;
    std::time_t timestamp;

    std::string getMessageTypeString() const {
        switch (type) {
            case ChatMessageType::Say: return "SAY";
            case ChatMessageType::Shout: return "SHOUT";
            case ChatMessageType::Tell: return "TELL";
            case ChatMessageType::Party: return "PARTY";
            case ChatMessageType::Linkshell1: return "LINKSHELL1";
            case ChatMessageType::Linkshell2: return "LINKSHELL2";
            case ChatMessageType::Yell: return "YELL";
            case ChatMessageType::Unity: return "UNITY";
            case ChatMessageType::Drops: return "DROPS";
            case ChatMessageType::Obtained: return "OBTAINED";
            case ChatMessageType::Trial: return "TRIAL";
            case ChatMessageType::System: return "SYSTEM";
            case ChatMessageType::Emote: return "EMOTE";
            default: return "UNKNOWN";
        }
    }
};
