#include "Player/TacticalPointsProperty.h"
#include "Player/PlayerNameProperty.h"
#include <iostream>

// Global reference to the player instance - will be set during initialization
Player* g_playerInstance = nullptr;

TacticalPointsProperty::TacticalPointsProperty() {
    // Constructor
}

TacticalPointsProperty::~TacticalPointsProperty() {
    // Clean up if needed
}

void TacticalPointsProperty::refresh(const PlayerProcessInfo& process) {
    // Get TP address
    uintptr_t tpAddress = FindDMAAddyInDLL(
        process.hProcess,
        process.procId,
        dllName,
        offsetToBaseAddress,
        offsets
    );

    if (tpAddress == 0) {
        std::cout << "Failed to find TP address for process " << process.procId << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(propertyMutex);

    // Store previous value for change detection
    int previousValue = 0;
    auto it = tacticalPoints.find(process.procId);
    if (it != tacticalPoints.end()) {
        previousValue = it->second;
        previousTP[process.procId] = previousValue;
    }

    // Read TP value from memory
    int tpValue = 0;
    if (ReadProcessMemory(process.hProcess, (BYTE*)tpAddress, &tpValue, sizeof(tpValue), nullptr)) {
        tacticalPoints[process.procId] = tpValue;

        // Check if value changed
        if (previousValue != tpValue) {
            changedFlags[process.procId] = true;
        }
    } else {
        std::cout << "Failed to read TP for process " << process.procId << std::endl;
    }
}

const char* TacticalPointsProperty::getPropertyName() const {
    return "Tactical Points";
}

void TacticalPointsProperty::displayValue(DWORD procId) const {
    auto it = tacticalPoints.find(procId);
    if (it != tacticalPoints.end()) {
        std::cout << it->second;
    } else {
        std::cout << "0";
    }
}

int TacticalPointsProperty::getTP(DWORD procId) const {
    std::lock_guard<std::mutex> lock(propertyMutex);
    auto it = tacticalPoints.find(procId);
    return (it != tacticalPoints.end()) ? it->second : 0;
}

bool TacticalPointsProperty::hasChanged(DWORD procId) const {
    std::lock_guard<std::mutex> lock(propertyMutex);
    auto it = changedFlags.find(procId);
    return (it != changedFlags.end()) && it->second;
}

void TacticalPointsProperty::acknowledgeChange(DWORD procId) {
    std::lock_guard<std::mutex> lock(propertyMutex);
    changedFlags[procId] = false;
}

void TacticalPointsProperty::reportChange(DWORD procId) const {
    std::lock_guard<std::mutex> lock(propertyMutex);

    auto currentIt = tacticalPoints.find(procId);
    auto previousIt = previousTP.find(procId);

    int currentValue = (currentIt != tacticalPoints.end()) ? currentIt->second : 0;
    int prevValue = (previousIt != previousTP.end()) ? previousIt->second : 0;

    // Get the player name if possible
    std::string playerName = "Unknown";
    if (g_playerInstance) {
        playerName = g_playerInstance->getPlayerName(procId);
        if (playerName.empty()) {
            playerName = "Unknown";
        }
    }

    std::cout << "Player [" << playerName << "] (PID: " << procId << ") - TP changed from " << prevValue
              << " to " << currentValue << std::endl;
}
