#ifndef CHATLOGMONITOR_H
#define CHATLOGMONITOR_H

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <map>
#include <fstream>
#include "Player/ChatMessage.h"

class ChatLogMonitor {
public:
    using ChatCallback = std::function<void(const ChatMessage&)>;

    ChatLogMonitor();
    ~ChatLogMonitor();

    // Start monitoring chat logs for a player
    bool StartMonitoring(const std::string& playerName, ChatCallback callback);

    // Stop monitoring
    void StopMonitoring();

    // Check if actively monitoring
    bool IsMonitoring() const;

private:
    std::string playerName;
    std::string logFilePath;
    ChatCallback callback;

    std::thread monitorThread;
    std::atomic<bool> shouldStop;
    std::atomic<bool> isActive;

    std::streampos lastPosition;

    // Find the log file for this player
    bool FindLogFile(const std::string& playerName);

    // Monitor thread function
    void MonitorThreadFunc();

    // Parse a chat log line
    ChatMessage ParseLogLine(const std::string& line);

    // Extract chat type from log line prefix
    ChatMessageType ExtractChatType(const std::string& line);
};

#endif // CHATLOGMONITOR_H
