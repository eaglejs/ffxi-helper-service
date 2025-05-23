#include <Windows.h>
#include "Player/Player.h"
#include "Player/PlayerNameProperty.h"
#include "Player/TacticalPointsProperty.h"
#include "helpers/memory.h"
#include <iostream>
#include <chrono>
#include <thread>

// Initialize static members
std::map<DWORD, PlayerProcessInfo> Player::processes;

// For TacticalPointsProperty to access player names
extern Player* g_playerInstance;

Player::Player() : monitoringActive(false) {
    // Set the global instance for properties to access
    g_playerInstance = this;
    // Initialize processes first
    initializeProcesses();

    // Create name property but don't add it to monitoring (name is static)
    playerNameProperty = std::make_shared<PlayerNameProperty>();

    // Register tactical points for continuous monitoring (update every 100ms)
    registerProperty(std::make_shared<TacticalPointsProperty>(), 100);

    // Refresh all properties initially including the name property
    refreshAllProperties();

    // Do a one-time refresh of the name property for all processes
    for (const auto& pair : processes) {
        if (pair.second.isValid) {
            playerNameProperty->refresh(pair.second);
        }
    }

    // Start the monitoring thread
    startMonitoring();
}

Player::~Player() {
    // Clear the global instance
    if (g_playerInstance == this) {
        g_playerInstance = nullptr;
    }

    // Stop monitoring thread
    stopMonitoring();

    // Clean up process handles
    for (auto& pair : processes) {
        if (pair.second.hProcess != NULL) {
            CloseHandle(pair.second.hProcess);
            pair.second.hProcess = NULL;
        }
    }
}

void Player::initializeProcesses() {
    // Clear existing processes
    for (auto& pair : processes) {
        if (pair.second.hProcess != NULL) {
            CloseHandle(pair.second.hProcess);
        }
    }
    processes.clear();

    // Find all pol.exe processes
    std::vector<DWORD> procIds = FindProcesses(false, procName);
    if (procIds.empty()) {
        std::cout << "No pol.exe processes found!" << std::endl;
        return;
    }

    std::cout << "Found " << procIds.size() << " pol.exe processes" << std::endl;

    // Initialize each process
    for (DWORD currentProcId : procIds) {
        PlayerProcessInfo info;
        info.procId = currentProcId;
        info.isValid = false;

        // Open process handle
        info.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, currentProcId);
        if (info.hProcess == NULL) {
            std::cout << "Failed to open process " << currentProcId << "! Skipping..." << std::endl;
            continue;
        }

        // Get module base address
        info.moduleBase = GetModuleBaseAddress(currentProcId, procName);
        if (info.moduleBase == 0) {
            std::cout << "Module base address not found for process " << currentProcId << "! Skipping..." << std::endl;
            CloseHandle(info.hProcess);
            info.hProcess = NULL;
            continue;
        }

        // Get DLL base address
        info.dllBase = GetDLLBaseAddress(currentProcId, dllName);
        if (info.dllBase == 0) {
            std::cout << "DLL base address not found for process " << currentProcId << "! Skipping..." << std::endl;
            CloseHandle(info.hProcess);
            info.hProcess = NULL;
            continue;
        }

        // Process is valid
        info.isValid = true;
        processes[currentProcId] = info;

        std::cout << "Successfully initialized process " << currentProcId << std::endl;
    }
}

std::vector<DWORD> Player::getProcessIds() const {
    std::vector<DWORD> ids;
    for (const auto& pair : processes) {
        if (pair.second.isValid) {
            ids.push_back(pair.first);
        }
    }
    return ids;
}

bool Player::isValidProcess(DWORD procId) const {
    auto it = processes.find(procId);
    return (it != processes.end() && it->second.isValid);
}

void Player::registerProperty(std::shared_ptr<PlayerProperty> property, unsigned int intervalMs) {
    PropertyConfig config;
    config.property = property;
    config.monitoringIntervalMs = (intervalMs > 0) ? intervalMs : defaultMonitoringIntervalMs;
    config.lastUpdateTime = std::chrono::steady_clock::now();
    propertyConfigs.push_back(config);
}

void Player::refreshAllProperties() {
    auto currentTime = std::chrono::steady_clock::now();

    // Refresh static player name property
    if (playerNameProperty) {
        for (const auto& pair : processes) {
            if (pair.second.isValid) {
                playerNameProperty->refresh(pair.second);
            }
        }
    }

    // Refresh monitored properties
    for (auto& config : propertyConfigs) {
        config.lastUpdateTime = currentTime; // Reset update time for all properties

        // Refresh for all valid processes
        for (const auto& pair : processes) {
            if (pair.second.isValid) {
                config.property->refresh(pair.second);
            }
        }
    }
}

PlayerProcessInfo* Player::getProcessInfo(DWORD procId) {
    auto it = processes.find(procId);
    if (it != processes.end() && it->second.isValid) {
        return &it->second;
    }
    return nullptr;
}

void Player::displayAllPlayerData() const {
    for (const auto& procId : getProcessIds()) {
        std::cout << "--- Process ID: " << procId << " ---" << std::endl;

        // Display static player name property (no monitoring interval)
        std::cout << playerNameProperty->getPropertyName() << ": ";
        playerNameProperty->displayValue(procId);
        std::cout << " (Static)" << std::endl;

        // Display monitored properties
        for (const auto& config : propertyConfigs) {
            std::cout << config.property->getPropertyName() << ": ";
            config.property->displayValue(procId);
            std::cout << " (Updates every " << config.monitoringIntervalMs << "ms)";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

// Convenience methods for common properties
std::string Player::getPlayerName(DWORD procId) const {
    // Use the dedicated playerNameProperty instead of searching through configs
    if (playerNameProperty) {
        const char* name = playerNameProperty->getName(procId);
        return name ? name : "";
    }
    return "";
}

int Player::getTacticalPoints(DWORD procId) const {
    for (const auto& config : propertyConfigs) {
        auto tpProperty = dynamic_cast<TacticalPointsProperty*>(config.property.get());
        if (tpProperty) {
            return tpProperty->getTP(procId);
        }
    }
    return 0;
}

void Player::setPropertyRefreshInterval(const char* propertyName, unsigned int intervalMs) {
    for (auto& config : propertyConfigs) {
        if (strcmp(config.property->getPropertyName(), propertyName) == 0) {
            config.monitoringIntervalMs = intervalMs;
            break;
        }
    }
}

void Player::refreshProperty(const char* propertyName) {
    // Check if it's the player name property
    if (playerNameProperty && strcmp(playerNameProperty->getPropertyName(), propertyName) == 0) {
        // Refresh player name property for all valid processes
        for (const auto& pair : processes) {
            if (pair.second.isValid) {
                playerNameProperty->refresh(pair.second);
            }
        }
        return;
    }

    // Otherwise, look for the property in the monitored properties
    std::shared_ptr<PlayerProperty> targetProperty = nullptr;

    for (auto& config : propertyConfigs) {
        if (strcmp(config.property->getPropertyName(), propertyName) == 0) {
            targetProperty = config.property;
            // Reset last update time
            config.lastUpdateTime = std::chrono::steady_clock::now();
            break;
        }
    }

    if (!targetProperty) {
        std::cout << "Property not found: " << propertyName << std::endl;
        return;
    }

    // Refresh for all valid processes
    for (const auto& pair : processes) {
        if (pair.second.isValid) {
            targetProperty->refresh(pair.second);
        }
    }
}

void Player::startMonitoring() {
    if (monitoringActive) return; // Already monitoring

    monitoringActive = true;
    monitorThread = std::thread(&Player::monitorPropertiesThread, this);
}

void Player::stopMonitoring() {
    if (!monitoringActive) return; // Not monitoring

    monitoringActive = false;
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
}

void Player::setMonitoringInterval(unsigned int intervalMs) {
    if (intervalMs > 0) {
        defaultMonitoringIntervalMs = intervalMs;
    }
}

bool Player::isMonitoring() const {
    return monitoringActive;
}

void Player::monitorPropertiesThread() {
    std::cout << "Monitoring thread started" << std::endl;

    while (monitoringActive) {
        auto currentTime = std::chrono::steady_clock::now();

        // Check each property if it's time to update
        for (auto& config : propertyConfigs) {
            // Calculate elapsed time since last update
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - config.lastUpdateTime
            ).count();

            // Check if it's time to refresh this property
            if (elapsedMs >= config.monitoringIntervalMs) {
                // Update last refresh time
                config.lastUpdateTime = currentTime;

                // Refresh for all valid processes
                for (const auto& pair : processes) {
                    if (pair.second.isValid) {
                        // Refresh the property
                        config.property->refresh(pair.second);

                        // Check if the property has changed
                        if (config.property->hasChanged(pair.first)) {
                            // Report the change
                            config.property->reportChange(pair.first);

                            // Acknowledge the change
                            config.property->acknowledgeChange(pair.first);
                        }
                    }
                }
            }
        }

        // Sleep for a short time to avoid CPU overuse
        // Use a shorter interval than any property's update interval to ensure timely updates
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Monitoring thread stopped" << std::endl;
}
