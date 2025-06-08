#pragma once

#include <Windows.h>
#include <vector>
#include <iostream>
#include <TlHelp32.h>
#include <thread>
#include <mutex>
#include <string>
#include <map>

#include "helpers/memory.h"

// Structure to hold player name information
struct PlayerNameInfo
{
    DWORD procId;
    HANDLE hProcess;
    uintptr_t moduleBase;
    uintptr_t playerNameAddress;
};

class PlayerName
{
private:
    PlayerName(DWORD procId, HANDLE hProcess);
    DWORD procId;
    HANDLE hProcess;
    const wchar_t *dllName = L"FFXiMain.dll";
    const wchar_t *procName = L"pol.exe";

public:
    DWORD offsetGameToBaseAddress = 0x000EA53C;
    std::vector<unsigned int> tpOffsets = {0x314};
		static std::map<DWORD, char*> playerNames;

    PlayerName();
    void fetchPlayerName();
		char* getPlayerName(DWORD);
};
