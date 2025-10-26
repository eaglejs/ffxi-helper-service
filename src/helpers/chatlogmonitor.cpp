#include "helpers/chatlogmonitor.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <Windows.h>
#include <ShlObj.h>

namespace fs = std::filesystem;

ChatLogMonitor::ChatLogMonitor()
    : shouldStop(false)
    , isActive(false)
    , lastPosition(0)
{
}

ChatLogMonitor::~ChatLogMonitor()
{
    StopMonitoring();
}

bool ChatLogMonitor::FindLogFile(const std::string& playerName)
{
    // Try multiple possible locations for the log directory
    std::vector<fs::path> possibleDirs;

    // Get My Documents path
    char documentsPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath)))
    {
        possibleDirs.push_back(fs::path(documentsPath) / "My Games" / "PlayOnline" / "PolLog");
    }

    // Also try without "My Games" subfolder (some installations)
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath)))
    {
        possibleDirs.push_back(fs::path(documentsPath) / "PlayOnline" / "PolLog");
    }

    // Try user profile directly
    char* userProfile = getenv("USERPROFILE");
    if (userProfile)
    {
        possibleDirs.push_back(fs::path(userProfile) / "Documents" / "My Games" / "PlayOnline" / "PolLog");
        possibleDirs.push_back(fs::path(userProfile) / "Documents" / "PlayOnline" / "PolLog");
    }

    fs::path logDir;
    bool foundDir = false;

    for (const auto& dir : possibleDirs)
    {
        if (fs::exists(dir) && fs::is_directory(dir))
        {
            logDir = dir;
            foundDir = true;
            std::cout << "[ChatLog] Found log directory: " << logDir << std::endl;
            break;
        }
    }

    if (!foundDir)
    {
        std::cerr << "[ChatLog] ERROR: FFXI log directory not found!" << std::endl;
        std::cerr << "[ChatLog] Tried locations:" << std::endl;
        for (const auto& dir : possibleDirs)
        {
            std::cerr << "[ChatLog]   - " << dir << std::endl;
        }
        std::cerr << "[ChatLog] " << std::endl;
        std::cerr << "[ChatLog] To enable chat logging in FFXI:" << std::endl;
        std::cerr << "[ChatLog]   1. Press ESC in game" << std::endl;
        std::cerr << "[ChatLog]   2. Go to Config → Misc. 2" << std::endl;
        std::cerr << "[ChatLog]   3. Enable 'Chat Log'" << std::endl;
        std::cerr << "[ChatLog]   4. Set log format and filters as desired" << std::endl;
        return false;
    }

    // Find log files matching pattern: YYYY-MM-DD_HH.MM.SS_<servername>.log
    // We'll look for the most recent file
    fs::path mostRecentLog;
    std::time_t mostRecentTime = 0;

    try
    {
        for (const auto& entry : fs::directory_iterator(logDir))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".log")
            {
                auto lastWrite = fs::last_write_time(entry);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    lastWrite - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                );
                std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

                if (cftime > mostRecentTime)
                {
                    mostRecentTime = cftime;
                    mostRecentLog = entry.path();
                }
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "[ChatLog] Error scanning log directory: " << e.what() << std::endl;
        return false;
    }

    if (mostRecentLog.empty())
    {
        std::cerr << "[ChatLog] No log files found in: " << logDir << std::endl;
        std::cerr << "[ChatLog] Make sure you have logged into FFXI at least once with chat logging enabled" << std::endl;
        return false;
    }

    logFilePath = mostRecentLog.string();
    std::cout << "[ChatLog] Monitoring log file: " << logFilePath << std::endl;

    // Seek to end of file to only read new lines
    std::ifstream file(logFilePath, std::ios::ate);
    if (file.is_open())
    {
        lastPosition = file.tellg();
        file.close();
        std::cout << "[ChatLog] Starting from end of file (position: " << lastPosition << ")" << std::endl;
    }

    return true;
}

bool ChatLogMonitor::StartMonitoring(const std::string& playerName, ChatCallback callback)
{
    if (isActive)
    {
        std::cerr << "[ChatLog] Already monitoring" << std::endl;
        return false;
    }

    this->playerName = playerName;
    this->callback = callback;

    if (!FindLogFile(playerName))
    {
        return false;
    }

    shouldStop = false;
    isActive = true;

    monitorThread = std::thread(&ChatLogMonitor::MonitorThreadFunc, this);

    std::cout << "[ChatLog] Started monitoring for player: " << playerName << std::endl;
    return true;
}

void ChatLogMonitor::StopMonitoring()
{
    if (!isActive)
    {
        return;
    }

    std::cout << "[ChatLog] Stopping monitoring..." << std::endl;
    shouldStop = true;

    if (monitorThread.joinable())
    {
        monitorThread.join();
    }

    isActive = false;
    std::cout << "[ChatLog] Monitoring stopped" << std::endl;
}

bool ChatLogMonitor::IsMonitoring() const
{
    return isActive;
}

ChatMessageType ChatLogMonitor::ExtractChatType(const std::string& line)
{
    // FFXI log format examples:
    // [HH:MM:SS] PlayerName : Message text
    // [HH:MM:SS] PlayerName >> Message text (tell)
    // [HH:MM:SS] (PlayerName) Message text (party)
    // [HH:MM:SS] <PlayerName> Message text (linkshell)
    // [HH:MM:SS] PlayerName says: Message text (say - explicit)

    if (line.find(" >> ") != std::string::npos)
        return ChatMessageType::Tell;

    if (line.find('(') != std::string::npos && line.find(')') != std::string::npos)
        return ChatMessageType::Party;

    if (line.find('<') != std::string::npos && line.find('>') != std::string::npos)
        return ChatMessageType::Linkshell;

    if (line.find(" shouts:") != std::string::npos || line.find(" shouts :") != std::string::npos)
        return ChatMessageType::Shout;

    if (line.find(" yells:") != std::string::npos || line.find(" yells :") != std::string::npos)
        return ChatMessageType::Yell;

    // Check for emote patterns
    if (line.find(" uses ") != std::string::npos ||
        line.find(" starts ") != std::string::npos ||
        line.find(" waves") != std::string::npos)
        return ChatMessageType::Emote;

    // Default to Say for standard format
    return ChatMessageType::Say;
}

ChatMessage ChatLogMonitor::ParseLogLine(const std::string& line)
{
    ChatMessage msg;
    msg.type = ExtractChatType(line);
    msg.timestamp = std::time(nullptr); // Use current time since log doesn't have full timestamp

    // Remove timestamp and leading whitespace
    std::string content = line;
    size_t bracketEnd = content.find(']');
    if (bracketEnd != std::string::npos)
    {
        content = content.substr(bracketEnd + 1);
    }

    // Trim leading whitespace
    content.erase(0, content.find_first_not_of(" \t"));

    // Parse based on chat type
    switch (msg.type)
    {
        case ChatMessageType::Tell:
        {
            // Format: PlayerName >> Message
            size_t arrowPos = content.find(" >> ");
            if (arrowPos != std::string::npos)
            {
                msg.sender = content.substr(0, arrowPos);
                msg.message = content.substr(arrowPos + 4);
            }
            break;
        }

        case ChatMessageType::Party:
        {
            // Format: (PlayerName) Message
            size_t parenEnd = content.find(')');
            if (parenEnd != std::string::npos)
            {
                msg.sender = content.substr(1, parenEnd - 1);
                msg.message = content.substr(parenEnd + 1);
                msg.message.erase(0, msg.message.find_first_not_of(" \t"));
            }
            break;
        }

        case ChatMessageType::Linkshell:
        {
            // Format: <PlayerName> Message
            size_t bracketEnd = content.find('>');
            if (bracketEnd != std::string::npos)
            {
                msg.sender = content.substr(1, bracketEnd - 1);
                msg.message = content.substr(bracketEnd + 1);
                msg.message.erase(0, msg.message.find_first_not_of(" \t"));
            }
            break;
        }

        default:
        {
            // Format: PlayerName : Message or PlayerName says: Message
            size_t colonPos = content.find(':');
            if (colonPos != std::string::npos)
            {
                msg.sender = content.substr(0, colonPos);

                // Remove trailing "says", "shouts", "yells" from sender
                size_t spacePos = msg.sender.find_last_of(' ');
                if (spacePos != std::string::npos)
                {
                    std::string verb = msg.sender.substr(spacePos + 1);
                    if (verb == "says" || verb == "shouts" || verb == "yells")
                    {
                        msg.sender = msg.sender.substr(0, spacePos);
                    }
                }

                // Trim sender
                msg.sender.erase(msg.sender.find_last_not_of(" \t") + 1);

                msg.message = content.substr(colonPos + 1);
                msg.message.erase(0, msg.message.find_first_not_of(" \t"));
            }
            break;
        }
    }

    return msg;
}

void ChatLogMonitor::MonitorThreadFunc()
{
    std::cout << "[ChatLog] Monitor thread started" << std::endl;

    while (!shouldStop)
    {
        try
        {
            std::ifstream file(logFilePath);
            if (!file.is_open())
            {
                std::cerr << "[ChatLog] Failed to open log file: " << logFilePath << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            // Seek to last read position
            file.seekg(lastPosition);

            std::string line;
            while (std::getline(file, line))
            {
                if (!line.empty())
                {
                    ChatMessage msg = ParseLogLine(line);

                    // Only process if we got valid sender and message
                    if (!msg.sender.empty() && !msg.message.empty())
                    {
                        std::cout << "[ChatLog] " << msg.sender << " ("
                                  << static_cast<int>(msg.type) << "): "
                                  << msg.message << std::endl;

                        if (callback)
                        {
                            callback(msg);
                        }
                    }
                }
            }

            // Update last position
            lastPosition = file.tellg();
            file.close();
        }
        catch (const std::exception& e)
        {
            std::cerr << "[ChatLog] Error in monitor thread: " << e.what() << std::endl;
        }

        // Sleep before next poll
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[ChatLog] Monitor thread ended" << std::endl;
}
