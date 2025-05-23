#include <Player/TacticalPoints.h>
#include <Player/PlayerName.h>

// Function to monitor a single process
void MonitorProcess(ProcessInfo &process)
{
	while (true)
	{
		int tpValue;
		bool success = ReadProcessMemory(process.hProcess, (BYTE *)process.tpAddress, &tpValue, sizeof(tpValue), 0);

		if (!success)
		{
			std::cout << "Process " << process.procId << " exited or cannot be read. Stopping process monitor." << std::endl;
			CloseHandle(process.hProcess);
			break;
		}

		if (tpValue != process.lastTpValue)
		{
			const char *playerName = PlayerName::playerNames[process.procId];
			std::cout << "Process ID " << process.procId << " (" << playerName << ") - TP Value changed: " << std::dec << tpValue << std::endl;
			process.lastTpValue = tpValue;
		}

		Sleep(100); // Check each 100ms @10FPS
	}
}

// Private constructor implementation
TacticalPoints::TacticalPoints(DWORD procId, HANDLE hProcess)
{
	this->procId = procId;
	this->hProcess = hProcess;
	this->playerName = new PlayerName();
}

// Public constructor implementation
TacticalPoints::TacticalPoints()
{
	fetchTP();
}

// Public method implementation
void TacticalPoints::fetchTP()
{
	std::vector<ProcessInfo> processes;
	std::vector<std::thread> monitorThreads;
	std::vector<DWORD> procIds = FindProcesses(false, procName);
	if (procIds.empty())
	{
		std::cout << "No pol.exe processes found!" << std::endl;
		return;
	}

	std::cout << "Found " << procIds.size() << " pol.exe processes" << std::endl;

	for (DWORD currentProcId : procIds) // Renamed loop variable to avoid confusion with member procId
	{
		HANDLE hCurrentProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, currentProcId); // Renamed loop variable
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

		uintptr_t tpAddress = FindDMAAddyInDLL(hCurrentProcess, currentProcId, dllName, offsetGameToBaseAddress, tpOffsets);

		ProcessInfo process = {
				currentProcId,
				hCurrentProcess,
				moduleBase,
				tpAddress,
				0 // Initialize lastTpValue to 0
		};

		// Read initial TP value
		ReadProcessMemory(hCurrentProcess, (BYTE *)tpAddress, &process.lastTpValue, sizeof(process.lastTpValue), 0);
		std::cout << "Initial TP Value for process " << currentProcId << ": " << process.lastTpValue << std::endl;

		processes.push_back(process);
	}

	for (auto &process : processes)
	{
		std::thread monitorThread(MonitorProcess, std::ref(process));
		monitorThreads.emplace_back(std::move(monitorThread));
	}
	for (auto &thread : monitorThreads)
	{
		thread.join();
	}
	std::cout << "All processes monitored. Exiting..." << std::endl;
	// Close all handles
	for (auto &process : processes)
	{
		CloseHandle(process.hProcess);
	}
}
