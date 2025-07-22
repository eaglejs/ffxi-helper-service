#include "Player/EliteAPI.h"
#include <iostream>

// Private implementation class to hide Elite API details
class EliteAPIImpl
{
private:
    HMODULE eliteApiDll;
    DWORD processId;

public:
    EliteAPIImpl() : eliteApiDll(nullptr), processId(0) {}

    ~EliteAPIImpl()
    {
        Cleanup();
    }

    bool Initialize(DWORD procId)
    {
        processId = procId;

        std::cout << "[EliteAPI] Initializing Elite API for process " << procId << std::endl;

        // For now, simulate successful initialization
        // In a real implementation, you would load the Elite API DLL here
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
        if (eliteApiDll)
        {
            std::cout << "[EliteAPI] Cleaning up Elite API for process " << processId << std::endl;
            FreeLibrary(eliteApiDll);
            eliteApiDll = nullptr;
        }
        processId = 0;
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
