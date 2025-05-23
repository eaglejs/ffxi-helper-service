#pragma once

#include <Windows.h>
#include <vector>
#include <iostream>
#include <TlHelp32.h>
#include <thread>
#include <mutex>
#include <string>

#include "helpers/memory.h"
#include "helpers/common.h"
#include "Player/PlayerName.h"

// Function declaration (not definition)
void MonitorProcess(ProcessInfo &process);

class TacticalPoints
{
private:
    TacticalPoints(DWORD procId, HANDLE hProcess);
    DWORD procId;
    HANDLE hProcess;
		PlayerName *playerName;
    const wchar_t *dllName = L"FFXiMain.dll";
    const wchar_t *procName = L"pol.exe";

public:
    DWORD offsetGameToBaseAddress = 0x000012BC;
    std::vector<unsigned int> tpOffsets = {0xD38};

    TacticalPoints();
    void fetchTP();
};
