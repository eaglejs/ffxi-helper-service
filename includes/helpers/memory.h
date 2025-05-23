#pragma once

#include <Windows.h>
#include <vector>
#include <TlHelp32.h>
#include <string>

// Function declarations only (no implementations)
std::vector<DWORD> GetAllProcIds(const wchar_t* procName);
DWORD GetProcId(const wchar_t* procName);
std::vector<DWORD> GetProcIdsByWindowTitle(const std::wstring& windowTitle);
std::vector<DWORD> FindProcesses(bool searchByWindow, const std::wstring& searchTerm);
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
uintptr_t GetDLLBaseAddress(DWORD procId, const wchar_t* dllName);
uintptr_t GetAddressFromDLL(HANDLE hProcess, DWORD procId, const wchar_t* dllName, uintptr_t offset);
uintptr_t FindDMAAddyInDLL(HANDLE hProc, DWORD procId, const wchar_t* dllName,
                          uintptr_t baseOffset, std::vector<unsigned int> offsets);
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
