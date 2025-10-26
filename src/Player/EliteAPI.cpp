#include "Player/EliteAPI.h"
#include "Player/ChatMessage.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>

// Elite API function type definitions
typedef void* (*CreateInstanceFunc)();
typedef void (*DeleteInstanceFunc)(void*);
typedef bool (*ReinitializeFunc)(void*);
typedef int (*GetChatLineCountFunc)(void*);
typedef const char* (*GetChatLineRawFunc)(void*, int);
typedef void (*FlushCommandsFunc)(void*);
typedef void (*SendStringFunc)(void*, const char*);

// Private implementation class to hide Elite API details
class EliteAPIImpl
{
private:
    HMODULE eliteApiDll;
    void* eliteApiInstance;
    DWORD processId;
    bool chatMonitoringActive;
    std::thread chatMonitorThread;
    std::mutex chatMutex;
    std::deque<ChatMessage> recentMessages;
    EliteAPI::ChatCallback chatCallback;
    int lastProcessedChatLine;

    // Elite API function pointers
    CreateInstanceFunc CreateInstance;
    DeleteInstanceFunc DeleteInstance;
    ReinitializeFunc Reinitialize;
    GetChatLineCountFunc GetChatLineCount;
    GetChatLineRawFunc GetChatLineRaw;
    FlushCommandsFunc FlushCommands;
    SendStringFunc SendString;

public:
    EliteAPIImpl()
        : eliteApiDll(nullptr)
        , eliteApiInstance(nullptr)
        , processId(0)
        , chatMonitoringActive(false)
        , lastProcessedChatLine(0)
        , CreateInstance(nullptr)
        , DeleteInstance(nullptr)
        , Reinitialize(nullptr)
        , GetChatLineCount(nullptr)
        , GetChatLineRaw(nullptr)
        , FlushCommands(nullptr)
        , SendString(nullptr)
    {
    }

    ~EliteAPIImpl()
    {
        Cleanup();
    }

    bool Initialize(DWORD procId)
    {
        processId = procId;

        std::cout << "[EliteAPI] Initializing Elite API for process " << procId << std::endl;

        // Try to load Elite API DLL from multiple locations
        const char* dllPaths[] = {
            "EliteAPI.dll",                           // Current directory
            "libs/elite/EliteAPI.dll",                // Project libs directory
            "./libs/elite/EliteAPI.dll",              // Relative path
            "../libs/elite/EliteAPI.dll",             // One level up
            "build/EliteAPI.dll"                      // Build output
        };

        for (const char* path : dllPaths)
        {
            eliteApiDll = LoadLibraryA(path);
            if (eliteApiDll)
            {
                std::cout << "[EliteAPI] Successfully loaded DLL from: " << path << std::endl;
                break;
            }
        }

        if (!eliteApiDll)
        {
            std::cout << "[EliteAPI] Failed to load Elite API DLL from any location" << std::endl;
            std::cout << "[EliteAPI] Tried paths: EliteAPI.dll, libs/elite/EliteAPI.dll, etc." << std::endl;
            return false;
        }

        // Load Elite API function pointers
        CreateInstance = (CreateInstanceFunc)GetProcAddress(eliteApiDll, "CreateInstance");
        DeleteInstance = (DeleteInstanceFunc)GetProcAddress(eliteApiDll, "DeleteInstance");
        Reinitialize = (ReinitializeFunc)GetProcAddress(eliteApiDll, "Reinitialize");
        GetChatLineCount = (GetChatLineCountFunc)GetProcAddress(eliteApiDll, "GetChatLineCount");
        GetChatLineRaw = (GetChatLineRawFunc)GetProcAddress(eliteApiDll, "GetChatLineRaw");
        FlushCommands = (FlushCommandsFunc)GetProcAddress(eliteApiDll, "FlushCommands");
        SendString = (SendStringFunc)GetProcAddress(eliteApiDll, "SendString");

        if (!CreateInstance || !DeleteInstance || !GetChatLineCount || !GetChatLineRaw)
        {
            std::cout << "[EliteAPI] Failed to load required Elite API functions" << std::endl;
            FreeLibrary(eliteApiDll);
            eliteApiDll = nullptr;
            return false;
        }

        // Create Elite API instance
        eliteApiInstance = CreateInstance();
        if (!eliteApiInstance)
        {
            std::cout << "[EliteAPI] Failed to create Elite API instance" << std::endl;
            FreeLibrary(eliteApiDll);
            eliteApiDll = nullptr;
            return false;
        }

        std::cout << "[EliteAPI] Successfully initialized Elite API instance" << std::endl;
        std::cout << "[EliteAPI] Initialization successful for process " << procId << std::endl;
        return true;
    }

    bool IsInitialized() const
    {
        return processId != 0;
    }

    DWORD GetProcessId() const
    {
        return processId;
    }

    PlayerStats GetPlayerStats()
    {
        if (!IsInitialized())
        {
            std::cout << "[EliteAPI] ERROR: Elite API not initialized" << std::endl;
            return PlayerStats();
        }

        std::cout << "[EliteAPI] Fetching player stats for process " << processId << std::endl;

        // Mock stats for testing - replace with actual Elite API calls
        PlayerStats stats;
        stats.Strength = 85;
        stats.Dexterity = 76;
        stats.Vitality = 92;
        stats.Agility = 68;
        stats.Intelligence = 45;
        stats.Mind = 58;
        stats.Charisma = 72;

        return stats;
    }

    void Cleanup()
    {
        StopChatMonitoring();

        if (eliteApiInstance && DeleteInstance)
        {
            std::cout << "[EliteAPI] Deleting Elite API instance for process " << processId << std::endl;
            DeleteInstance(eliteApiInstance);
            eliteApiInstance = nullptr;
        }

        if (eliteApiDll)
        {
            std::cout << "[EliteAPI] Cleaning up Elite API for process " << processId << std::endl;
            FreeLibrary(eliteApiDll);
            eliteApiDll = nullptr;
        }

        processId = 0;
        CreateInstance = nullptr;
        DeleteInstance = nullptr;
        Reinitialize = nullptr;
        GetChatLineCount = nullptr;
        GetChatLineRaw = nullptr;
        FlushCommands = nullptr;
        SendString = nullptr;
    }

    // Chat monitoring methods
    bool RegisterChatCallback(EliteAPI::ChatCallback callback)
    {
        if (!IsInitialized())
        {
            return false;
        }

        chatCallback = callback;
        std::cout << "[EliteAPI] Chat callback registered for process " << processId << std::endl;
        return true;
    }

    void UnregisterChatCallback()
    {
        chatCallback = nullptr;
        std::cout << "[EliteAPI] Chat callback unregistered" << std::endl;
    }

    std::vector<ChatMessage> GetRecentChatMessages(int count)
    {
        std::lock_guard<std::mutex> lock(chatMutex);

        std::vector<ChatMessage> messages;
        int start = std::max(0, static_cast<int>(recentMessages.size()) - count);

        for (size_t i = start; i < recentMessages.size(); i++)
        {
            messages.push_back(recentMessages[i]);
        }

        return messages;
    }

    void StartChatMonitoring()
    {
        if (chatMonitoringActive || !IsInitialized() || !eliteApiInstance || !GetChatLineCount || !GetChatLineRaw)
        {
            return;
        }

        chatMonitoringActive = true;
        lastProcessedChatLine = GetChatLineCount(eliteApiInstance);

        std::cout << "[EliteAPI] Started chat monitoring for process " << processId << std::endl;
        std::cout << "[EliteAPI] Current chat line count: " << lastProcessedChatLine << std::endl;

        // Start monitoring thread
        chatMonitorThread = std::thread([this]() {
            std::cout << "[EliteAPI] Chat monitoring thread started" << std::endl;

            while (chatMonitoringActive)
            {
                try
                {
                    if (!eliteApiInstance || !GetChatLineCount || !GetChatLineRaw)
                    {
                        break;
                    }

                    int currentCount = GetChatLineCount(eliteApiInstance);

                    // Debug: Show chat line count periodically
                    static int lastReportedCount = -1;
                    if (currentCount != lastReportedCount)
                    {
                        std::cout << "[EliteAPI DEBUG] Chat line count: " << currentCount
                                  << " (last processed: " << lastProcessedChatLine << ")" << std::endl;
                        lastReportedCount = currentCount;
                    }

                    // Process new chat lines
                    while (lastProcessedChatLine < currentCount && chatMonitoringActive)
                    {
                        const char* rawLine = GetChatLineRaw(eliteApiInstance, lastProcessedChatLine);
                        std::cout << "[EliteAPI DEBUG] Processing chat line " << lastProcessedChatLine << std::endl;
                        if (rawLine && strlen(rawLine) > 0)
                        {
                            std::cout << "[EliteAPI DEBUG] Raw chat line: " << rawLine << std::endl;
                            ParseChatLine(rawLine);
                        }
                        else
                        {
                            std::cout << "[EliteAPI DEBUG] Empty or null chat line" << std::endl;
                        }
                        lastProcessedChatLine++;
                    }

                    // Sleep briefly to avoid busy-waiting
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                catch (const std::exception& e)
                {
                    std::cout << "[EliteAPI] Error in chat monitoring: " << e.what() << std::endl;
                }
            }

            std::cout << "[EliteAPI] Chat monitoring thread stopped" << std::endl;
        });
    }

    void StopChatMonitoring()
    {
        if (!chatMonitoringActive)
        {
            return;
        }

        chatMonitoringActive = false;

        if (chatMonitorThread.joinable())
        {
            chatMonitorThread.join();
        }

        std::cout << "[EliteAPI] Stopped chat monitoring for process " << processId << std::endl;
    }

    // Parse a raw chat line from Elite API
    void ParseChatLine(const char* rawLine)
    {
        std::cout << "[EliteAPI DEBUG] ParseChatLine called with: " << (rawLine ? rawLine : "NULL") << std::endl;

        if (!rawLine || strlen(rawLine) == 0)
        {
            std::cout << "[EliteAPI DEBUG] Skipping empty/null line" << std::endl;
            return;
        }

        ChatMessage msg;
        msg.timestamp = std::time(nullptr);
        msg.type = ChatMessageType::Unknown;

        std::string line(rawLine);

        // Elite API raw chat format typically includes formatting codes
        // We need to parse these to extract sender, type, and message
        // Format varies but generally: [Type] Sender: Message

        // Try to detect chat type from format
        if (line.find("[Say]") != std::string::npos || line.find(": ") != std::string::npos)
        {
            msg.type = ChatMessageType::Say;
        }
        else if (line.find("[Shout]") != std::string::npos || line.find("!!") != std::string::npos)
        {
            msg.type = ChatMessageType::Shout;
        }
        else if (line.find("[Tell]") != std::string::npos || line.find(">>") != std::string::npos)
        {
            msg.type = ChatMessageType::Tell;
        }
        else if (line.find("[Party]") != std::string::npos || line.find("(") != std::string::npos)
        {
            msg.type = ChatMessageType::Party;
        }
        else if (line.find("[Linkshell]") != std::string::npos || line.find("<") != std::string::npos)
        {
            msg.type = ChatMessageType::Linkshell;
        }
        else if (line.find("[Yell]") != std::string::npos)
        {
            msg.type = ChatMessageType::Yell;
        }
        else if (line.find("[Emote]") != std::string::npos)
        {
            msg.type = ChatMessageType::Emote;
        }
        else
        {
            msg.type = ChatMessageType::System;
        }

        // Try to extract sender and message
        // Common format: "Sender: Message" or "[Type] Sender: Message"
        size_t colonPos = line.find(": ");
        if (colonPos != std::string::npos)
        {
            // Extract sender (text before the colon)
            std::string beforeColon = line.substr(0, colonPos);

            // Remove any bracketed prefixes like [Say], [Party], etc.
            size_t lastBracket = beforeColon.rfind(']');
            if (lastBracket != std::string::npos)
            {
                beforeColon = beforeColon.substr(lastBracket + 1);
            }

            // Trim whitespace
            size_t start = beforeColon.find_first_not_of(" \t\n\r");
            size_t end = beforeColon.find_last_not_of(" \t\n\r");
            if (start != std::string::npos && end != std::string::npos)
            {
                msg.sender = beforeColon.substr(start, end - start + 1);
            }

            // Extract message (text after the colon)
            msg.message = line.substr(colonPos + 2);
        }
        else
        {
            // No sender found, treat entire line as message
            msg.sender = "System";
            msg.message = line;
        }

        // Remove any remaining formatting codes (Elite API uses various codes)
        // This is a simple cleanup - may need refinement based on actual format
        msg.message = RemoveFormattingCodes(msg.message);
        msg.sender = RemoveFormattingCodes(msg.sender);

        if (!msg.message.empty())
        {
            // Store in recent messages
            {
                std::lock_guard<std::mutex> lock(chatMutex);
                recentMessages.push_back(msg);
                if (recentMessages.size() > 100)
                {
                    recentMessages.pop_front();
                }
            }

            // Call callback if registered
            if (chatCallback)
            {
                chatCallback(msg);
            }

            std::cout << "[Chat] " << msg.sender << " (" << msg.getMessageTypeString()
                      << "): " << msg.message << std::endl;
        }
    }

    // Remove FFXI/Elite API formatting codes from text
    std::string RemoveFormattingCodes(const std::string& text)
    {
        std::string result;
        result.reserve(text.length());

        for (size_t i = 0; i < text.length(); i++)
        {
            unsigned char c = static_cast<unsigned char>(text[i]);

            // FFXI uses various escape codes (0x1E, 0x1F, 0x7F, etc.)
            if (c == 0x1E || c == 0x1F || c == 0x7F)
            {
                // Skip the next few bytes (color codes, etc.)
                if (c == 0x1F && i + 1 < text.length())
                {
                    i++; // Skip the parameter byte
                }
                continue;
            }

            // Skip control characters
            if (c < 0x20 && c != '\n' && c != '\r' && c != '\t')
            {
                continue;
            }

            result += text[i];
        }

        return result;
    }
};

// EliteAPI wrapper implementation
EliteAPI::EliteAPI() : impl(std::make_unique<EliteAPIImpl>()), processId(0), initialized(false)
{
}

EliteAPI::~EliteAPI()
{
    Cleanup();
}

bool EliteAPI::Initialize(DWORD procId)
{
    if (impl->Initialize(procId))
    {
        processId = procId;
        initialized = true;
        return true;
    }
    return false;
}

bool EliteAPI::IsInitialized() const
{
    return initialized && impl->IsInitialized();
}

DWORD EliteAPI::GetProcessId() const
{
    return processId;
}

PlayerStats EliteAPI::GetPlayerStats()
{
    if (!IsInitialized())
    {
        std::cout << "[EliteAPI] ERROR: Cannot get player stats - Elite API not initialized" << std::endl;
        return PlayerStats();
    }

    return impl->GetPlayerStats();
}

void EliteAPI::Cleanup()
{
    if (impl)
    {
        impl->Cleanup();
    }
    initialized = false;
    processId = 0;
}

// Chat monitoring public interface
bool EliteAPI::RegisterChatCallback(ChatCallback callback)
{
    return impl->RegisterChatCallback(callback);
}

void EliteAPI::UnregisterChatCallback()
{
    impl->UnregisterChatCallback();
}

std::vector<ChatMessage> EliteAPI::GetRecentChatMessages(int count)
{
    return impl->GetRecentChatMessages(count);
}

void EliteAPI::StartChatMonitoring()
{
    impl->StartChatMonitoring();
}

void EliteAPI::StopChatMonitoring()
{
    impl->StopChatMonitoring();
}
