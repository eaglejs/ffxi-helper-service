#pragma once

#include <Windows.h>
#include <memory>
#include "Player/PlayerStats.h"

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
};
