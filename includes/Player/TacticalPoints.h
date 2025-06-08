#pragma once

#include <Windows.h>
#include <vector>
#include <iostream>
#include <TlHelp32.h>
#include <thread>
#include <mutex>
#include <string>

#include "helpers/memory.h"
#include "Player/PlayerName.h"

// extend ProcessInfo with uintptr_t tpAddress; int lastTpValue;
struct ProcessInfo
{
		DWORD procId;
		HANDLE hProcess;
		uintptr_t moduleBase;
		uintptr_t tpAddress; // Address for Tactical Points
		int lastTpValue;     // Last known TP value for change detection
};

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
