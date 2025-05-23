#pragma once

#include "Player/Player.h"
#include <map>
#include <string>

class PlayerNameProperty : public PlayerProperty {
private:
    DWORD offsetToBaseAddress = 0x000EA53C;
    std::vector<unsigned int> offsets = {0x314};

    // Storage for player names by process ID - only this map is needed since names are static
    std::map<DWORD, char*> playerNames;

public:
    PlayerNameProperty();
    ~PlayerNameProperty();

    // Implementation of base class abstract methods
    virtual void refresh(const PlayerProcessInfo& process) override;
    virtual const char* getPropertyName() const override;
    virtual void displayValue(DWORD procId) const override;    // Change detection implementation - simplified for static property
    virtual bool hasChanged(DWORD procId) const override;
    virtual void acknowledgeChange(DWORD procId) override;
    virtual void reportChange(DWORD procId) const override;

    // Property-specific methods
    const char* getName(DWORD procId) const;
};
