#include "Player/EliteAPI.h"
#include "Player/ChatMessage.h"
#include "helpers/logger.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include "helpers/http.h"

// Elite API function type definitions
// FIXED: Elite API uses __stdcall convention (Windows standard)
typedef void* (__stdcall *CreateInstanceFunc)(DWORD);
typedef void (__stdcall *DeleteInstanceFunc)(void*);
typedef bool (__stdcall *ReinitializeFunc)(void*);
typedef int (__stdcall *GetChatLineCountFunc)(void*);
typedef bool (__stdcall *GetChatLineRawFunc)(void*, int, char*, int);
typedef void (__stdcall *FlushCommandsFunc)(void*);
typedef void (__stdcall *SendStringFunc)(void*, const char*);

// Shared DLL manager - ensures DLL is loaded only once and shared across all instances
class SharedDLLManager
{
private:
    static std::mutex dllMutex;
    static HMODULE sharedDll;
    static int refCount;

public:
    static CreateInstanceFunc CreateInstance;
    static DeleteInstanceFunc DeleteInstance;
    static ReinitializeFunc Reinitialize;
    static GetChatLineCountFunc GetChatLineCount;
    static GetChatLineRawFunc GetChatLineRaw;
    static FlushCommandsFunc FlushCommands;
    static SendStringFunc SendString;

    static bool LoadDLL()
    {
        std::lock_guard<std::mutex> lock(dllMutex);

        if (sharedDll != nullptr)
        {
            refCount++;
            return true; // Already loaded
        }

        // Try to load Elite API DLL from multiple locations
        const char* dllPaths[] = {
            "EliteAPI.dll",
            "libs/elite/EliteAPI.dll",
            "./libs/elite/EliteAPI.dll",
            "../libs/elite/EliteAPI.dll",
            "build/EliteAPI.dll"
        };

        for (const char* path : dllPaths)
        {
            sharedDll = LoadLibraryA(path);
            if (sharedDll)
            {
                std::cout << "[SharedDLL] Successfully loaded DLL from: " << path << std::endl;
                break;
            }
        }

        if (!sharedDll)
        {
            std::cout << "[SharedDLL] Failed to load Elite API DLL from any location" << std::endl;
            return false;
        }

        // Load function pointers
        CreateInstance = (CreateInstanceFunc)GetProcAddress(sharedDll, "CreateInstance");
        DeleteInstance = (DeleteInstanceFunc)GetProcAddress(sharedDll, "DeleteInstance");
        Reinitialize = (ReinitializeFunc)GetProcAddress(sharedDll, "Reinitialize");
        GetChatLineCount = (GetChatLineCountFunc)GetProcAddress(sharedDll, "GetChatLineCount");
        GetChatLineRaw = (GetChatLineRawFunc)GetProcAddress(sharedDll, "GetChatLineRaw");
        FlushCommands = (FlushCommandsFunc)GetProcAddress(sharedDll, "FlushCommands");
        SendString = (SendStringFunc)GetProcAddress(sharedDll, "SendString");

        if (!CreateInstance || !DeleteInstance || !GetChatLineCount || !GetChatLineRaw)
        {
            std::cout << "[SharedDLL] Failed to load required Elite API functions" << std::endl;
            FreeLibrary(sharedDll);
            sharedDll = nullptr;
            return false;
        }

        refCount = 1;
        std::cout << "[SharedDLL] DLL loaded successfully with all function pointers" << std::endl;
        return true;
    }

    static void UnloadDLL()
    {
        std::lock_guard<std::mutex> lock(dllMutex);

        if (sharedDll == nullptr)
            return;

        refCount--;
        if (refCount <= 0)
        {
            std::cout << "[SharedDLL] Unloading DLL (refCount = 0)" << std::endl;
            FreeLibrary(sharedDll);
            sharedDll = nullptr;
            CreateInstance = nullptr;
            DeleteInstance = nullptr;
            Reinitialize = nullptr;
            GetChatLineCount = nullptr;
            GetChatLineRaw = nullptr;
            FlushCommands = nullptr;
            SendString = nullptr;
        }
    }

    static std::mutex& GetDLLMutex() { return dllMutex; }
};

// Static member initialization
std::mutex SharedDLLManager::dllMutex;
HMODULE SharedDLLManager::sharedDll = nullptr;
int SharedDLLManager::refCount = 0;
CreateInstanceFunc SharedDLLManager::CreateInstance = nullptr;
DeleteInstanceFunc SharedDLLManager::DeleteInstance = nullptr;
ReinitializeFunc SharedDLLManager::Reinitialize = nullptr;
GetChatLineCountFunc SharedDLLManager::GetChatLineCount = nullptr;
GetChatLineRawFunc SharedDLLManager::GetChatLineRaw = nullptr;
FlushCommandsFunc SharedDLLManager::FlushCommands = nullptr;
SendStringFunc SharedDLLManager::SendString = nullptr;

// Private implementation class to hide Elite API details
class EliteAPIImpl
{
private:
    HMODULE eliteApiDll;  // Kept for compatibility but not used anymore
    void* eliteApiInstance;
    DWORD processId;
    bool chatMonitoringActive;
    std::thread chatMonitorThread;
    std::mutex chatMutex;
    std::mutex dllCallMutex;  // Protects all DLL function calls for thread safety
    std::deque<ChatMessage> recentMessages;
    EliteAPI::ChatCallback chatCallback;
    int lastProcessedChatLine;

public:
    EliteAPIImpl()
        : eliteApiDll(nullptr)
        , eliteApiInstance(nullptr)
        , processId(0)
        , chatMonitoringActive(false)
        , lastProcessedChatLine(0)
    {
    }

    ~EliteAPIImpl()
    {
        Cleanup();
    }

    bool Initialize(DWORD procId)
    {
        LOG_ENTER("ELITEAPI", "Initialize");
        LOG("ELITEAPI", "Initialize: Process ID = " + std::to_string(procId));
        LOG_FLUSH();

        processId = procId;

        std::cout << "[EliteAPI] Initializing Elite API for process " << procId << std::endl;

        // Load shared DLL (thread-safe, only loads once)
        LOG("ELITEAPI", "Initialize: Loading shared DLL");
        LOG_FLUSH();

        if (!SharedDLLManager::LoadDLL())
        {
            LOG_ERROR("ELITEAPI", "Initialize", "Failed to load shared DLL");
            LOG_FLUSH();
            std::cout << "[EliteAPI] Failed to load shared DLL" << std::endl;
            return false;
        }

        LOG("ELITEAPI", "Initialize: Shared DLL loaded successfully");
        LOG_FLUSH();

        // Create Elite API instance using shared function pointers (thread-safe)
        LOG("ELITEAPI", "Initialize: Creating Elite API instance");
        LOG_FLUSH();

        {
            std::lock_guard<std::mutex> lock(SharedDLLManager::GetDLLMutex());
            eliteApiInstance = SharedDLLManager::CreateInstance(processId);
        }

        if (!eliteApiInstance)
        {
            LOG_ERROR("ELITEAPI", "Initialize", "Failed to create Elite API instance");
            LOG_FLUSH();
            std::cout << "[EliteAPI] Failed to create Elite API instance" << std::endl;
            SharedDLLManager::UnloadDLL();
            return false;
        }

        LOG("ELITEAPI", "Initialize: Elite API instance created successfully");
        LOG_FLUSH();

        std::cout << "[EliteAPI] Successfully initialized Elite API instance" << std::endl;
        std::cout << "[EliteAPI] Initialization successful for process " << procId << std::endl;

        LOG_EXIT("ELITEAPI", "Initialize");
        LOG_FLUSH();

        LOG("ELITEAPI", "About to return true from Initialize");
        LOG_FLUSH();

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

        if (eliteApiInstance)
        {
            std::cout << "[EliteAPI] Deleting Elite API instance for process " << processId << std::endl;
            {
                std::lock_guard<std::mutex> lock(SharedDLLManager::GetDLLMutex());
                if (SharedDLLManager::DeleteInstance)
                {
                    SharedDLLManager::DeleteInstance(eliteApiInstance);
                }
            }
            eliteApiInstance = nullptr;
        }

        // Unload shared DLL (decrements ref count)
        SharedDLLManager::UnloadDLL();

        processId = 0;
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
        LOG_ENTER("ELITEAPI", "StartChatMonitoring");
        LOG_FLUSH();

        if (chatMonitoringActive || !IsInitialized() || !eliteApiInstance)
        {
            LOG("ELITEAPI", "StartChatMonitoring: Conditions not met, returning");
            LOG_FLUSH();
            return;
        }

        LOG("ELITEAPI", "StartChatMonitoring: Setting chatMonitoringActive = true");
        LOG_FLUSH();
        chatMonitoringActive = true;
        lastProcessedChatLine = 0;

        LOG("ELITEAPI", "StartChatMonitoring: About to get initial chat line count");
        LOG_FLUSH();

        // Get initial chat line count (thread-safe)
        int initialCount = 0;
        {
            LOG("ELITEAPI", "StartChatMonitoring: Acquiring mutex");
            LOG_FLUSH();
            std::lock_guard<std::mutex> lock(SharedDLLManager::GetDLLMutex());
            LOG("ELITEAPI", "StartChatMonitoring: Mutex acquired, calling GetChatLineCount");
            LOG_FLUSH();
            initialCount = SharedDLLManager::GetChatLineCount(eliteApiInstance);
            LOG("ELITEAPI", "StartChatMonitoring: GetChatLineCount returned " + std::to_string(initialCount));
            LOG_FLUSH();
        }
        LOG("ELITEAPI", "StartChatMonitoring: Mutex released");
        LOG_FLUSH();
        LOG("ELITEAPI", "StartChatMonitoring: About to print initialCount to console");
        LOG_FLUSH();
        std::cout << "[EliteAPI] GetChatLineCount returned: " << initialCount << std::endl;
        LOG("ELITEAPI", "StartChatMonitoring: Console print completed");
        LOG_FLUSH();

        // Test reading a recent chat line
        LOG("ELITEAPI", "StartChatMonitoring: Checking if initialCount > 0 (value=" + std::to_string(initialCount) + ")");
        LOG_FLUSH();
        if (initialCount > 0)
        {
            LOG("ELITEAPI", "StartChatMonitoring: initialCount > 0, testing GetChatLineRaw");
            LOG_FLUSH();
            std::cout << "[EliteAPI] Testing GetChatLineRaw..." << std::endl;

            // Try reading the most recent line (thread-safe)
            int testIndex = initialCount - 1;
            LOG("ELITEAPI", "StartChatMonitoring: testIndex = " + std::to_string(testIndex));
            LOG_FLUSH();

            char lineBuffer[2048] = {0};
            bool success = false;

            {
                LOG("ELITEAPI", "StartChatMonitoring: Acquiring mutex for GetChatLineRaw");
                LOG_FLUSH();
                std::lock_guard<std::mutex> lock(SharedDLLManager::GetDLLMutex());
                LOG("ELITEAPI", "StartChatMonitoring: Calling GetChatLineRaw");
                LOG_FLUSH();
                success = SharedDLLManager::GetChatLineRaw(eliteApiInstance, testIndex, lineBuffer, sizeof(lineBuffer));
                LOG("ELITEAPI", "StartChatMonitoring: GetChatLineRaw returned " + std::string(success ? "true" : "false"));
                LOG_FLUSH();
            }

            if (success)
            {
                LOG("ELITEAPI", "StartChatMonitoring: GetChatLineRaw succeeded");
                LOG_FLUSH();
                std::cout << "[EliteAPI] Successfully read chat line " << testIndex << std::endl;
            }
            else
            {
                LOG("ELITEAPI", "StartChatMonitoring: GetChatLineRaw failed");
                LOG_FLUSH();
                std::cout << "[EliteAPI] Failed to read chat line " << testIndex << std::endl;
            }
        }

        // Start from current count (won't process existing messages, only new ones)
        LOG("ELITEAPI", "StartChatMonitoring: Setting lastProcessedChatLine");
        LOG_FLUSH();
        lastProcessedChatLine = initialCount;
        std::cout << "[EliteAPI] Will start processing from line: " << lastProcessedChatLine << std::endl;

        LOG("ELITEAPI", "StartChatMonitoring: Printing final messages");
        LOG_FLUSH();
        std::cout << "[EliteAPI] Chat monitoring enabled for process " << processId << std::endl;
        std::cout << "[EliteAPI] Use PollChatMessages() from main thread to check for new messages" << std::endl;
        LOG_EXIT("ELITEAPI", "StartChatMonitoring");
        LOG_FLUSH();
    }

    // Poll for new chat messages - MUST be called from main thread to avoid DLL crashes
    int PollChatMessages()
    {
        if (!chatMonitoringActive || !eliteApiInstance)
        {
            return 0;
        }

        LOG("ELITEAPI", "PollChatMessages: Starting poll for process " + std::to_string(processId));
        LOG_FLUSH();

        int messagesProcessed = 0;

        try
        {
            // Get current chat count (thread-safe)
            LOG("ELITEAPI", "PollChatMessages: Getting chat line count");
            LOG_FLUSH();

            int currentCount = 0;
            {
                std::lock_guard<std::mutex> lock(SharedDLLManager::GetDLLMutex());
                LOG("ELITEAPI", "PollChatMessages: Calling GetChatLineCount");
                LOG_FLUSH();
                currentCount = SharedDLLManager::GetChatLineCount(eliteApiInstance);
                LOG("ELITEAPI", "PollChatMessages: GetChatLineCount returned " + std::to_string(currentCount));
                LOG_FLUSH();
            }

            // Sanity check
            if (currentCount < 0 || currentCount > 10000)
            {
                LOG("ELITEAPI", "PollChatMessages: Invalid count " + std::to_string(currentCount) + ", returning");
                LOG_FLUSH();
                return 0;
            }

            LOG("ELITEAPI", "PollChatMessages: lastProcessed=" + std::to_string(lastProcessedChatLine) + ", current=" + std::to_string(currentCount));
            LOG_FLUSH();

            // Process new chat lines (limit to 10 per poll to avoid blocking)
            const int MAX_MESSAGES_PER_POLL = 10;

            while (lastProcessedChatLine < currentCount && messagesProcessed < MAX_MESSAGES_PER_POLL)
            {
                LOG("ELITEAPI", "PollChatMessages: Processing line " + std::to_string(lastProcessedChatLine));
                LOG_FLUSH();

                char lineBuffer[2048] = {0};
                bool success = false;

                try
                {
                    // Thread-safe DLL call
                    {
                        std::lock_guard<std::mutex> lock(SharedDLLManager::GetDLLMutex());
                        // LOG("ELITEAPI", "PollChatMessages: Calling GetChatLineRaw for index " + std::to_string(lastProcessedChatLine));
                        // LOG_FLUSH();
                        success = SharedDLLManager::GetChatLineRaw(eliteApiInstance, lastProcessedChatLine, lineBuffer, sizeof(lineBuffer));
                        // LOG("ELITEAPI", "PollChatMessages: GetChatLineRaw returned " + std::string(success ? "true" : "false"));
                        // LOG_FLUSH();
                    }
                }
                catch (...)
                {
                    LOG_ERROR("ELITEAPI", "PollChatMessages", "Exception calling GetChatLineRaw at index " + std::to_string(lastProcessedChatLine));
                    LOG_FLUSH();
                    std::cout << "[EliteAPI] Exception calling GetChatLineRaw at index " << lastProcessedChatLine << std::endl;
                    lastProcessedChatLine++;
                    messagesProcessed++;
                    continue;
                }

                if (success)
                {
                    // Parse the line from the buffer
                    ParseChatLine(lineBuffer);
                }
                else
                {
                    LOG("ELITEAPI", "PollChatMessages: Failed to read line " + std::to_string(lastProcessedChatLine));
                    LOG_FLUSH();
                }

                lastProcessedChatLine++;
                messagesProcessed++;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "[EliteAPI] Error polling chat: " << e.what() << std::endl;
        }

        return messagesProcessed;
    }

    void StopChatMonitoring()
    {
        if (!chatMonitoringActive)
        {
            return;
        }

        chatMonitoringActive = false;
        std::cout << "[EliteAPI] Stopped chat monitoring for process " << processId << std::endl;
    }

    // Parse a raw chat line from Elite API
    void ParseChatLine(const char* rawLine)
    {
        try
        {
            if (!rawLine || strlen(rawLine) == 0)
            {
                return;
            }

            // DEBUG: Print raw bytes to inspect formatting codes
            std::cout << "[EliteAPI] DEBUG Raw Bytes: ";
            for (size_t i = 0; i < strlen(rawLine); i++) {
                printf("%02X ", (unsigned char)rawLine[i]);
            }
            std::cout << std::endl;
            std::cout << "[EliteAPI] DEBUG Raw String: " << rawLine << std::endl;

            ChatMessage msg;
            msg.timestamp = std::time(nullptr);
            msg.type = ChatMessageType::System; // Default to system
            msg.rawContent = rawLine;

            std::string line(rawLine);

            // Clean up the line - remove special characters
            std::string cleanLine;
            for (char c : line)
            {
                if (c >= 32 && c <= 126) // Printable ASCII only
                {
                    cleanLine += c;
                }
            }
            
            std::cout << "[EliteAPI] DEBUG Clean String: " << cleanLine << std::endl;

            if (cleanLine.empty())
            {
                return;
            }

            // Parse message type and sender
            // Format: (PlayerName) Message (Party)
            if (cleanLine.front() == '(')
            {
                size_t closeParen = cleanLine.find(") ");
                if (closeParen != std::string::npos)
                {
                    msg.type = ChatMessageType::Party;
                    msg.sender = cleanLine.substr(1, closeParen - 1);
                    msg.message = cleanLine.substr(closeParen + 2);
                }
            }
            // Format: <PlayerName> Message (Linkshell)
            else if (cleanLine.front() == '<')
            {
                size_t closeBracket = cleanLine.find("> ");
                if (closeBracket != std::string::npos)
                {
                    msg.type = ChatMessageType::Linkshell1;
                    msg.sender = cleanLine.substr(1, closeBracket - 1);
                    msg.message = cleanLine.substr(closeBracket + 2);
                }
            }
            // Format: PlayerName>> Message (Tell)
            else
            {
                size_t tellMarker = cleanLine.find(">> ");
                if (tellMarker != std::string::npos)
                {
                    msg.type = ChatMessageType::Tell;
                    msg.sender = cleanLine.substr(0, tellMarker);
                    msg.message = cleanLine.substr(tellMarker + 3);
                }
                else
                {
                    // Format: PlayerName : Message (Say/Shout)
                    size_t colonMarker = cleanLine.find(" : ");
                    if (colonMarker != std::string::npos)
                    {
                        msg.type = ChatMessageType::Say; // Default to Say, hard to distinguish Shout without color
                        msg.sender = cleanLine.substr(0, colonMarker);
                        msg.message = cleanLine.substr(colonMarker + 3);
                    }
                    else
                    {
                        // Fallback
                        msg.sender = "Unknown";
                        msg.message = cleanLine;
                    }
                }
            }
            
            std::cout << "[EliteAPI] DEBUG Parsed - Sender: " << msg.sender << ", Type: " << msg.getMessageTypeString() << ", Msg: " << msg.message << std::endl;

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
                    try
                    {
                        chatCallback(msg);
                    }
                    catch (...)
                    {
                        std::cout << "[EliteAPI] Exception in chat callback" << std::endl;
                    }
                }

                // Send to Server
                try
                {
                    HttpClient client;

                    // Simple JSON escaping
                    auto escapeJson = [](const std::string& s) {
                        std::string out;
                        for (char c : s) {
                            if (c == '"') out += "\\\"";
                            else if (c == '\\') out += "\\\\";
                            else if (c == '\b') out += "\\b";
                            else if (c == '\f') out += "\\f";
                            else if (c == '\n') out += "\\n";
                            else if (c == '\r') out += "\\r";
                            else if (c == '\t') out += "\\t";
                            else out += c;
                        }
                        return out;
                    };

                    std::string json = "{";
                    json += "\"sender\": \"" + escapeJson(msg.sender) + "\",";
                    json += "\"message\": \"" + escapeJson(msg.message) + "\",";
                    json += "\"type\": \"" + msg.getMessageTypeString() + "\",";
                    json += "\"timestamp\": " + std::to_string(msg.timestamp);
                    json += "}";

                    std::cout << "[EliteAPI] DEBUG JSON Payload: " << json << std::endl;

                    // Fire and forget - don't block too long
                    client.setTimeout(2);
                    auto response = client.post("http://192.168.5.30/chat", json);
                    std::cout << "[EliteAPI] DEBUG HTTP Response Code: " << response.statusCode << std::endl;
                }
                catch (const std::exception& e)
                {
                    std::cout << "[EliteAPI] Failed to send chat to server: " << e.what() << std::endl;
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "[EliteAPI] Exception in ParseChatLine: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "[EliteAPI] Unknown exception in ParseChatLine" << std::endl;
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

int EliteAPI::PollChatMessages()
{
    return impl->PollChatMessages();
}
