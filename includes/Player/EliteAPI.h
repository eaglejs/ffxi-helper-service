#pragma once

#include <Windows.h>
#include <memory>
#include <functional>
#include <vector>
#include "Player/PlayerStats.h"
#include "Player/ChatMessage.h"

// Forward declaration
class EliteAPIImpl;

/**
 * Elite API wrapper class for FFXI process interaction
 * Based on Elite API documentation from elitemmonetwork.com
 */
class EliteAPI
{
private:
    std::unique_ptr<EliteAPIImpl> impl;
    DWORD processId;
    bool initialized;

public:
    EliteAPI();
    ~EliteAPI();

    /**
     * Initialize Elite API for a specific FFXI process
     * @param procId The process ID to attach to
     * @return true if initialization succeeded, false otherwise
     */
    bool Initialize(DWORD procId);

    /**
     * Check if Elite API is initialized and ready to use
     * @return true if initialized, false otherwise
     */
    bool IsInitialized() const;

    /**
     * Get the process ID this Elite API instance is attached to
     * @return Process ID, or 0 if not initialized
     */
    DWORD GetProcessId() const;

    /**
     * Get player base stats
     * @return PlayerStats structure with current player stats
     */
    PlayerStats GetPlayerStats();

    /**
     * Cleanup and detach from process
     */
    void Cleanup();

    // Chat monitoring methods
    using ChatCallback = std::function<void(const ChatMessage&)>;

    /**
     * Register a callback function to be called when chat messages are received
     * @param callback Function to call with each chat message
     * @return true if callback was registered successfully
     */
    bool RegisterChatCallback(ChatCallback callback);

    /**
     * Unregister the chat callback
     */
    void UnregisterChatCallback();

    /**
     * Get recent chat messages
     * @param count Maximum number of messages to retrieve
     * @return Vector of recent chat messages
     */
    std::vector<ChatMessage> GetRecentChatMessages(int count = 50);

    /**
     * Start monitoring chat packets
     */
    void StartChatMonitoring();

    /**
     * Stop monitoring chat packets
     */
    void StopChatMonitoring();

    /**
     * Poll for new chat messages (call from main thread periodically)
     * This should be called instead of using background threads to avoid DLL thread-safety issues
     * @return Number of messages processed
     */
    int PollChatMessages();
};
