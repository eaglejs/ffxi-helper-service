#include "Player/PlayerName.h"

// Define static member variable
std::map<DWORD, char*> PlayerName::playerNames;

// Private constructor implementation
PlayerName::PlayerName(DWORD procId, HANDLE hProcess)
{
	this->procId = procId;
	this->hProcess = hProcess;
	this->playerNames = {};
}

// Public constructor implementation
PlayerName::PlayerName()
{
	fetchPlayerName();
}

// Public method implementation
void PlayerName::fetchPlayerName()
{
	std::vector<PlayerNameInfo> processes;
	std::vector<std::thread> monitorThreads;
	std::vector<DWORD> procIds = FindProcesses(false, procName);
	if (procIds.empty())
	{
		std::cout << "No pol.exe processes found!" << std::endl;
		return;
	}

	std::cout << "Found " << procIds.size() << " pol.exe processes" << std::endl;

	for (DWORD currentProcId : procIds)
	{
		HANDLE hCurrentProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, currentProcId);
		if (hCurrentProcess == NULL)
		{
			std::cout << "Failed to open process " << currentProcId << "! Skipping..." << std::endl;
			continue;
		}

		uintptr_t moduleBase = GetModuleBaseAddress(currentProcId, dllName);

		if (moduleBase == 0)
		{
			std::cout << "Module base address not found for process " << currentProcId << "! Skipping..." << std::endl;
			CloseHandle(hCurrentProcess);
			continue;
		}
		uintptr_t playerNameAddress = FindDMAAddyInDLL(hCurrentProcess, currentProcId, dllName, offsetGameToBaseAddress, tpOffsets);
		std::string playerNameStr;
		PlayerNameInfo process = {
				currentProcId,
				hCurrentProcess,
				moduleBase,
				playerNameAddress
		};

		char* playerName = new char[256];

		// Read initial player name
		ReadProcessMemory(hCurrentProcess, (BYTE *)playerNameAddress, playerName, 256, 0);
		// Store the player name in the map
		playerNames[currentProcId] = playerName;
		std::cout << "Player Name for process " << currentProcId << ": " << playerNames[currentProcId] << std::endl;

		processes.push_back(process);
	}

	std::cout << "All processes monitored. Exiting..." << std::endl;
	// Close all handles
	for (auto &process : processes)
	{
		CloseHandle(process.hProcess);
	}
}
char* PlayerName::getPlayerName(DWORD procId)
{
	auto it = playerNames.find(procId);
	if (it != playerNames.end())
	{
		return it->second;
	}
	else
	{
		std::cout << "Player name not found for process ID: " << procId << std::endl;
		return nullptr;
	}
}
