#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "memory.h"

std::vector<DWORD> GetAllProcIds(const wchar_t *procName)
{
	std::vector<DWORD> procIds;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32FirstW(hSnap, &procEntry))
		{
			do
			{
				if (_wcsicmp(procEntry.szExeFile, procName) == 0)
				{
					procIds.push_back(procEntry.th32ProcessID);
				}
			} while (Process32NextW(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);
	return procIds;
}

// Keep the original function for backward compatibility
DWORD GetProcId(const wchar_t *procName)
{
	std::vector<DWORD> procIds = GetAllProcIds(procName);
	return procIds.empty() ? 0 : procIds[0];
}

// Function to get process IDs by window title

// Helper structure for GetProcIdsByWindowTitle.
// Moved outside the function to be accessible by the static callback function.
struct EnumDataForGetProcIdsByWindowTitle {
	const std::wstring& searchTitle;
	std::vector<DWORD>& procIds;
};

// Static callback function for EnumWindows, ensuring correct calling convention.
static BOOL CALLBACK EnumWindowsCallbackForGetProcIds(HWND hwnd, LPARAM lParam) {
	// Safely cast lParam back to our data structure pointer
	EnumDataForGetProcIdsByWindowTitle* pData = reinterpret_cast<EnumDataForGetProcIdsByWindowTitle*>(lParam);

	// Get window text
	wchar_t title[256];
	if (GetWindowTextW(hwnd, title, sizeof(title)/sizeof(wchar_t)) == 0) {
		return TRUE; // Skip windows with no title or error
	}

	// Check if window is visible
	if (!IsWindowVisible(hwnd)) {
		return TRUE; // Skip invisible windows
	}

	// Check if the window title contains the search string
	if (wcsstr(title, pData->searchTitle.c_str()) != nullptr) {
		// Get process ID for this window
		DWORD procId;
		GetWindowThreadProcessId(hwnd, &procId);

		// Add the process ID if it's not already in the list (to avoid duplicates)
		if (std::find(pData->procIds.begin(), pData->procIds.end(), procId) == pData->procIds.end()) {
			pData->procIds.push_back(procId);
		}
	}

	return TRUE; // Continue enumeration
}

std::vector<DWORD> GetProcIdsByWindowTitle(const std::wstring& windowTitle) {
	std::vector<DWORD> procIds; // This vector will be filled by the callback

	// Prepare data to be passed to the callback
	EnumDataForGetProcIdsByWindowTitle data = { windowTitle, procIds };

	// Enumerate all top-level windows
	EnumWindows(EnumWindowsCallbackForGetProcIds, reinterpret_cast<LPARAM>(&data));

	return procIds;
}

std::vector<DWORD> FindProcesses(bool searchByWindow, const std::wstring& searchTerm) {
    if (searchByWindow) {
        return GetProcIdsByWindowTitle(searchTerm);
    } else {
        return GetAllProcIds(searchTerm.c_str());
    }
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t *modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32FirstW(hSnap, &modEntry))
		{
			do
			{
				if (_wcsicmp(modEntry.szModule, modName) == 0)
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32NextW(hSnap, &modEntry));
		}
	}

	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t GetDLLBaseAddress(DWORD procId, const wchar_t *dllName)
{
	uintptr_t dllBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32FirstW(hSnap, &modEntry))
		{
			do
			{
				if (_wcsicmp(modEntry.szModule, dllName) == 0)
				{
					dllBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32NextW(hSnap, &modEntry));
		}
	}

	CloseHandle(hSnap);
	return dllBaseAddr;
}

// Get memory address within a DLL
uintptr_t GetAddressFromDLL(DWORD procId, const wchar_t *dllName, uintptr_t offset)
{
	uintptr_t dllBase = GetDLLBaseAddress(procId, dllName);
	if (dllBase == 0)
		return 0;

	return dllBase + offset;
}

// Read memory from specific DLL using a chain of offsets
uintptr_t FindDMAAddyInDLL(HANDLE hProc, DWORD procId, const wchar_t *dllName,
                          uintptr_t baseOffset, std::vector<unsigned int> offsets)
{
    uintptr_t dllBase = GetDLLBaseAddress(procId, dllName);
    if (dllBase == 0) {
        std::cout << "ERROR: Could not find DLL base address" << std::endl;
        return 0;
    }

    // Start at DLL base + offset
    uintptr_t addr = dllBase + baseOffset;

    // For all offsets except the last one
    for (unsigned int i = 0; i < offsets.size(); i++) {
        if (!ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), nullptr)) {
            std::cout << "ERROR: Failed to read memory at 0x" << std::hex << addr << std::dec << std::endl;
            return 0;
        }

        addr += offsets[i];
    }
    return addr;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;

	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		if (!ReadProcessMemory(hProc, (BYTE *)addr, &addr, sizeof(addr), nullptr))
			return 0;

		addr += offsets[i];
	}

	return addr;
}
