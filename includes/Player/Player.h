#pragma once

#include <Windows.h>
#include <vector>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include "memory.h"

// Forward declarations for property classes
class PlayerProperty;

// Core player data structure
struct PlayerProcessInfo {
    DWORD procId;
    HANDLE hProcess;
    uintptr_t moduleBase;
    uintptr_t dllBase;
    bool isValid;
};

class Player {
private:
    // Process management
    const wchar_t* procName = L"pol.exe";
    const wchar_t* dllName = L"FFXiMain.dll";

    // Storage for all detected FFXI processes
    static std::map<DWORD, PlayerProcessInfo> processes;    // Property storage with monitoring configuration
    struct PropertyConfig {
        std::shared_ptr<PlayerProperty> property;
        unsigned int monitoringIntervalMs;
        std::chrono::steady_clock::time_point lastUpdateTime;
    };

    std::vector<PropertyConfig> propertyConfigs;

    // Static properties (read once, don't change during gameplay)
    std::map<DWORD, std::string> playerNames;
    std::map<DWORD, DWORD> playerIds;

    // Static property memory addresses
    static const DWORD PLAYER_NAME_OFFSET_BASE = 0x004DBA94;
    static const std::vector<unsigned int> PLAYER_NAME_OFFSETS;
    static const DWORD PLAYER_ID_OFFSET_BASE = 0x000106BC;
    static const std::vector<unsigned int> PLAYER_ID_OFFSETS;
		static const DWORD PLAYER_CONQUEST_OFFSET_BASE = 0x001E646C;
		static const std::vector<unsigned int> PLAYER_CONQUEST_OFFSETS;

    // Monitoring thread control
    std::thread monitorThread;
    std::atomic<bool> monitoringActive;
    std::mutex processMutex;
    unsigned int defaultMonitoringIntervalMs = 100; // Default check every 100ms

    // Process initialization
    void initializeProcesses();

    // Process lifecycle management
    void checkForDeadProcesses();
    void checkForNewProcesses();
    void cleanupDeadProcess(DWORD procId);
    bool isProcessAlive(DWORD procId) const;

    // Static property reading
    void readStaticProperties();
    void readPlayerName(const PlayerProcessInfo& process);
    void readPlayerId(const PlayerProcessInfo& process);

    // Thread function for continuous monitoring
    void monitorPropertiesThread();

public:
    Player();
    ~Player();

    // Process management
    std::vector<DWORD> getProcessIds() const;
    bool isValidProcess(DWORD procId) const;    // Property management
    void registerProperty(std::shared_ptr<PlayerProperty> property, unsigned int intervalMs = 0);
    void setPropertyRefreshInterval(const char* propertyName, unsigned int intervalMs);
    void refreshAllProperties();
    void refreshProperty(const char* propertyName);
    void forceRefreshStaticProperties(); // Force refresh player names and IDs for all processes

    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    void setMonitoringInterval(unsigned int intervalMs);
    bool isMonitoring() const;

    // Name property access (implemented directly for convenience)
    std::string getPlayerName(DWORD procId) const;

    // PlayerId property access (implemented directly for convenience)
    DWORD getPlayerId(DWORD procId) const;

    // TP property access (implemented directly for convenience)
    int getTacticalPoints(DWORD procId) const;

    // Debug/display
    void displayAllPlayerData() const;

    // Expose process info for properties to use
    static PlayerProcessInfo* getProcessInfo(DWORD procId);
};

// Abstract base class for player properties
class PlayerProperty {
protected:
    const wchar_t* dllName = L"FFXiMain.dll";
    mutable std::mutex propertyMutex;

public:
    virtual ~PlayerProperty() = default;

    // Core property methods
    virtual void refresh(const PlayerProcessInfo& process) = 0;
    virtual const char* getPropertyName() const = 0;
    virtual void displayValue(DWORD procId) const = 0;

    // Change detection
    virtual bool hasChanged(DWORD procId) const = 0;
    virtual void acknowledgeChange(DWORD procId) = 0;
    virtual void reportChange(DWORD procId) const = 0;
};
