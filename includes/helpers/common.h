#pragma once

#include <Windows.h>
#include <vector>

// Structure to hold process information
struct ProcessInfo
{
    DWORD procId;
    HANDLE hProcess;
    uintptr_t moduleBase;
    uintptr_t tpAddress;
    int lastTpValue;
};
