#pragma once

#include "Player/Player.h"
#include "helpers/memory.h"
#include "helpers/http.h"
#include <map>

// Forward declaration of global player instance
extern Player* g_playerInstance;

class TacticalPointsProperty : public PlayerProperty {
private:
    DWORD offsetToBaseAddress = 0x000012BC;
    std::vector<unsigned int> offsets = {0xD38};

    // Storage for tactical points by process ID
    std::map<DWORD, int> tacticalPoints;

    // Change tracking
    std::map<DWORD, int> previousTP;
    std::map<DWORD, bool> changedFlags;

    // HTTP client for sending TP updates
    mutable HttpClient httpClient;
    static const std::string API_ENDPOINT;

    // Helper method for sending TP data to API
    void sendTPUpdate(const std::string& playerName, DWORD playerId, int tp) const;

    // Helper method to sanitize player name for JSON
    std::string sanitizePlayerName(const std::string& rawName) const;

public:
    TacticalPointsProperty();
    ~TacticalPointsProperty();

    // Implementation of base class abstract methods
    virtual void refresh(const PlayerProcessInfo& process) override;
    virtual const char* getPropertyName() const override;
    virtual void displayValue(DWORD procId) const override;

    // Change detection implementation
    virtual bool hasChanged(DWORD procId) const override;
    virtual void acknowledgeChange(DWORD procId) override;
    virtual void reportChange(DWORD procId) const override;

    // Property-specific methods
    int getTP(DWORD procId) const;
};
