#include <Windows.h>
#include "Player/Player.h"
#include "Player/TacticalPointsProperty.h"
#include "helpers/memory.h"
#include <iostream>
#include <chrono>
#include <thread>

// Initialize static members
std::map<DWORD, PlayerProcessInfo> Player::processes;

// Static property offset definitions
const std::vector<unsigned int> Player::PLAYER_NAME_OFFSETS = {0xA4};
const std::vector<unsigned int> Player::PLAYER_ID_OFFSETS = {0x4E0};
const std::vector<unsigned int> Player::PLAYER_CONQUEST_OFFSETS = {0x8C};

// For TacticalPointsProperty to access player names
extern Player *g_playerInstance;

Player::Player() : monitoringActive(false)
{
	// Set the global instance for properties to access
	g_playerInstance = this;
	// Initialize processes first
	initializeProcesses();

	// Read static properties (name and ID) once
	readStaticProperties();

	// Register tactical points for continuous monitoring (update every 100ms)
	registerProperty(std::make_shared<TacticalPointsProperty>(), 100);

	// Refresh all dynamic properties initially
	refreshAllProperties();

	// Start the monitoring thread
	startMonitoring();
}

Player::~Player()
{
	// Clear the global instance
	if (g_playerInstance == this)
	{
		g_playerInstance = nullptr;
	}

	// Stop monitoring thread
	stopMonitoring();

	// Clean up process handles
	for (auto &pair : processes)
	{
		if (pair.second.hProcess != NULL)
		{
			CloseHandle(pair.second.hProcess);
			pair.second.hProcess = NULL;
		}
	}
}

void Player::initializeProcesses()
{
	// Clear existing processes
	for (auto &pair : processes)
	{
		if (pair.second.hProcess != NULL)
		{
			CloseHandle(pair.second.hProcess);
		}
	}
	processes.clear();

	// Find all pol.exe processes
	std::vector<DWORD> procIds = FindProcesses(false, procName);
	if (procIds.empty())
	{
		std::cout << "No pol.exe processes found!" << std::endl;
		return;
	}

	std::cout << "Found " << procIds.size() << " pol.exe processes" << std::endl;

	// Initialize each process
	for (DWORD currentProcId : procIds)
	{
		PlayerProcessInfo info;
		info.procId = currentProcId;
		info.isValid = false;

		// Open process handle
		info.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, currentProcId);
		if (info.hProcess == NULL)
		{
			std::cout << "Failed to open process " << currentProcId << "! Skipping..." << std::endl;
			continue;
		}

		// Get module base address
		info.moduleBase = GetModuleBaseAddress(currentProcId, procName);
		if (info.moduleBase == 0)
		{
			std::cout << "Module base address not found for process " << currentProcId << "! Skipping..." << std::endl;
			CloseHandle(info.hProcess);
			info.hProcess = NULL;
			continue;
		}

		// Get DLL base address
		info.dllBase = GetDLLBaseAddress(currentProcId, dllName);
		if (info.dllBase == 0)
		{
			std::cout << "DLL base address not found for process " << currentProcId << "! Skipping..." << std::endl;
			CloseHandle(info.hProcess);
			info.hProcess = NULL;
			continue;
		}

		// Process is valid
		info.isValid = true;
		processes[currentProcId] = info;

		std::cout << "Successfully initialized process " << currentProcId << std::endl;
	}
}

void Player::readStaticProperties()
{
	// Read static properties for all valid processes
	for (const auto &pair : processes)
	{
		if (pair.second.isValid)
		{
			readPlayerName(pair.second);
			readPlayerId(pair.second);
		}
	}

	// Debug output
	for (const auto &pair : processes)
	{
		if (pair.second.isValid)
		{
			DWORD procId = pair.first;
			std::cout << "Updated player name for process " << procId << ": " << getPlayerName(procId) << std::endl;
		}
	}
}

void Player::readPlayerName(const PlayerProcessInfo& process)
{
	// Get player name address
	uintptr_t nameAddress = FindDMAAddyInDLL(
		process.hProcess,
		process.procId,
		dllName,
		PLAYER_NAME_OFFSET_BASE,
		PLAYER_NAME_OFFSETS);

	if (nameAddress == 0)
	{
		std::cout << "Failed to find player name address for process " << process.procId << std::endl;
		playerNames[process.procId] = "Unknown";
		return;
	}

	// Read player name from memory (assuming max 16 chars for FFXI names)
	char nameBuffer[17] = {0}; // 16 chars + null terminator
	if (ReadProcessMemory(process.hProcess, (BYTE*)nameAddress, nameBuffer, 16, nullptr))
	{
		nameBuffer[16] = '\0'; // Ensure null termination

		// Validate that we got a reasonable player name
		std::string rawName(nameBuffer);

		// Check if the name contains only printable characters and isn't empty
		bool validName = !rawName.empty() && rawName.length() > 1;
		for (char c : rawName)
		{
			if (c != 0 && (c < 32 || c > 126))  // Non-printable character
			{
				validName = false;
				break;
			}
			if (c == 0) break; // Null terminator, stop checking
		}

		if (validName)
		{
			// Trim null characters and whitespace
			size_t actualLength = rawName.find('\0');
			if (actualLength != std::string::npos)
			{
				rawName = rawName.substr(0, actualLength);
			}

			// Trim whitespace
			size_t start = rawName.find_first_not_of(" \t");
			if (start != std::string::npos)
			{
				size_t end = rawName.find_last_not_of(" \t");
				rawName = rawName.substr(start, end - start + 1);
			}

			if (!rawName.empty() && rawName.length() > 1)
			{
				playerNames[process.procId] = rawName;
				std::cout << "Successfully read player name: '" << rawName << "' for process " << process.procId << std::endl;
			}
			else
			{
				playerNames[process.procId] = "Unknown";
				std::cout << "Player name was empty or too short for process " << process.procId << std::endl;
			}
		}
		else
		{
			playerNames[process.procId] = "Unknown";
			std::cout << "Invalid player name data for process " << process.procId << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to read player name memory for process " << process.procId << std::endl;
		playerNames[process.procId] = "Unknown";
	}
}

void Player::readPlayerId(const PlayerProcessInfo& process)
{
	// Get player ID address
	uintptr_t idAddress = FindDMAAddyInDLL(
		process.hProcess,
		process.procId,
		dllName,
		PLAYER_ID_OFFSET_BASE,
		PLAYER_ID_OFFSETS);

	if (idAddress == 0)
	{
		std::cout << "Failed to find player ID address for process " << process.procId << std::endl;
		playerIds[process.procId] = 0;
		return;
	}

	// Read player ID from memory
	DWORD playerId = 0;
	if (ReadProcessMemory(process.hProcess, (BYTE*)idAddress, &playerId, sizeof(playerId), nullptr))
	{
		// Validate that we got a reasonable player ID (should be non-zero)
		if (playerId > 0)
		{
			playerIds[process.procId] = playerId;
			std::cout << "Successfully read player ID: " << playerId << " for process " << process.procId << std::endl;
		}
		else
		{
			playerIds[process.procId] = 0;
			std::cout << "Player ID was zero for process " << process.procId << " (may not be logged in yet)" << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to read player ID memory for process " << process.procId << std::endl;
		playerIds[process.procId] = 0;
	}
}

// Process lifecycle management methods
bool Player::isProcessAlive(DWORD procId) const
{
	auto it = processes.find(procId);
	if (it == processes.end() || !it->second.isValid)
	{
		return false;
	}

	// Check if process is still running by trying to query its exit code
	DWORD exitCode;
	if (GetExitCodeProcess(it->second.hProcess, &exitCode))
	{
		return (exitCode == STILL_ACTIVE);
	}

	return false;
}

void Player::cleanupDeadProcess(DWORD procId)
{
	std::lock_guard<std::mutex> lock(processMutex);

	auto it = processes.find(procId);
	if (it != processes.end())
	{
		std::cout << "Cleaning up dead process " << procId << std::endl;

		// Close handle if it exists
		if (it->second.hProcess != NULL)
		{
			CloseHandle(it->second.hProcess);
		}

		// Remove from processes map
		processes.erase(it);

		// Remove from player names and IDs maps
		playerNames.erase(procId);
		playerIds.erase(procId);
	}
}

void Player::checkForDeadProcesses()
{
	std::vector<DWORD> deadProcesses;

	// First pass: identify dead processes
	for (const auto& pair : processes)
	{
		if (!isProcessAlive(pair.first))
		{
			deadProcesses.push_back(pair.first);
		}
	}

	// Second pass: clean up dead processes
	for (DWORD deadProcId : deadProcesses)
	{
		cleanupDeadProcess(deadProcId);
	}
}

void Player::checkForNewProcesses()
{
	// Find all current pol.exe processes
	std::vector<DWORD> currentProcIds = FindProcesses(false, procName);

	// Check each current process to see if it's new
	for (DWORD procId : currentProcIds)
	{
		// Skip if we already have this process
		if (processes.find(procId) != processes.end())
		{
			continue;
		}

		std::cout << "Found new process " << procId << ", initializing..." << std::endl;

		// Initialize new process
		PlayerProcessInfo info;
		info.procId = procId;
		info.isValid = false;

		// Open process handle
		info.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
		if (info.hProcess == NULL)
		{
			std::cout << "Failed to open new process " << procId << "! Skipping..." << std::endl;
			continue;
		}

		// Get module base address
		info.moduleBase = GetModuleBaseAddress(procId, procName);
		if (info.moduleBase == 0)
		{
			std::cout << "Module base address not found for new process " << procId << "! Skipping..." << std::endl;
			CloseHandle(info.hProcess);
			continue;
		}

		// Get DLL base address
		info.dllBase = GetDLLBaseAddress(procId, dllName);
		if (info.dllBase == 0)
		{
			std::cout << "DLL base address not found for new process " << procId << "! Skipping..." << std::endl;
			CloseHandle(info.hProcess);
			continue;
		}

		// Process is valid
		info.isValid = true;

		{
			std::lock_guard<std::mutex> lock(processMutex);
			processes[procId] = info;
		}

		// Wait a moment for the process to stabilize (game might still be loading)
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		// Read static properties for the new process with retry logic
		int retryCount = 0;
		const int maxRetries = 5;
		bool successfullyReadData = false;

		while (retryCount < maxRetries && !successfullyReadData)
		{
			readPlayerName(info);
			readPlayerId(info);

			// Check if we successfully read the data
			std::string playerName = getPlayerName(procId);
			DWORD playerId = getPlayerId(procId);

			if (playerName != "Unknown" && playerName.length() > 1 && playerId != 0)
			{
				successfullyReadData = true;
				std::cout << "Successfully read player data on attempt " << (retryCount + 1) << std::endl;
			}
			else
			{
				retryCount++;
				if (retryCount < maxRetries)
				{
					std::cout << "Failed to read player data, retrying in 2 seconds... (Attempt " << retryCount << "/" << maxRetries << ")" << std::endl;
					std::this_thread::sleep_for(std::chrono::milliseconds(2000));
				}
			}
		}

		if (!successfullyReadData)
		{
			std::cout << "WARNING: Failed to read player data after " << maxRetries << " attempts. Process " << procId << " may not be fully loaded yet." << std::endl;
		}

		// Force refresh all properties for the new process
		for (auto &config : propertyConfigs)
		{
			config.property->refresh(info);
		}

		std::cout << "Successfully initialized new process " << procId << std::endl;
		std::cout << "[DEBUG] New process player name: '" << getPlayerName(procId) << "'" << std::endl;
		std::cout << "[DEBUG] New process player ID: " << getPlayerId(procId) << std::endl;
	}
}

std::vector<DWORD> Player::getProcessIds() const
{
	std::vector<DWORD> ids;
	for (const auto &pair : processes)
	{
		if (pair.second.isValid)
		{
			ids.push_back(pair.first);
		}
	}
	return ids;
}

bool Player::isValidProcess(DWORD procId) const
{
	auto it = processes.find(procId);
	return (it != processes.end() && it->second.isValid);
}

void Player::registerProperty(std::shared_ptr<PlayerProperty> property, unsigned int intervalMs)
{
	PropertyConfig config;
	config.property = property;
	config.monitoringIntervalMs = (intervalMs > 0) ? intervalMs : defaultMonitoringIntervalMs;
	config.lastUpdateTime = std::chrono::steady_clock::now();
	propertyConfigs.push_back(config);
}

void Player::refreshAllProperties()
{
	auto currentTime = std::chrono::steady_clock::now();

	// Refresh monitored properties only (static properties are read once at initialization)
	for (auto &config : propertyConfigs)
	{
		config.lastUpdateTime = currentTime; // Reset update time for all properties

		// Refresh for all valid processes
		for (const auto &pair : processes)
		{
			if (pair.second.isValid)
			{
				config.property->refresh(pair.second);
			}
		}
	}
}

PlayerProcessInfo *Player::getProcessInfo(DWORD procId)
{
	auto it = processes.find(procId);
	if (it != processes.end() && it->second.isValid)
	{
		return &it->second;
	}
	return nullptr;
}

void Player::displayAllPlayerData() const
{
	for (const auto &procId : getProcessIds())
	{
		std::cout << "--- Process ID: " << procId << " ---" << std::endl;

		// Display static properties
		std::cout << "Player Name: " << getPlayerName(procId) << " (Static)" << std::endl;
		std::cout << "Player ID: " << getPlayerId(procId) << " (Static)" << std::endl;

		// Display monitored properties
		for (const auto &config : propertyConfigs)
		{
			std::cout << config.property->getPropertyName() << ": ";
			config.property->displayValue(procId);
			std::cout << " (Updates every " << config.monitoringIntervalMs << "ms)";
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

// Convenience methods for common properties
std::string Player::getPlayerName(DWORD procId) const
{
	auto it = playerNames.find(procId);
	return (it != playerNames.end()) ? it->second : "Unknown";
}

DWORD Player::getPlayerId(DWORD procId) const
{
	auto it = playerIds.find(procId);
	return (it != playerIds.end()) ? it->second : 0;
}

int Player::getTacticalPoints(DWORD procId) const
{
	for (const auto &config : propertyConfigs)
	{
		auto tpProperty = dynamic_cast<TacticalPointsProperty *>(config.property.get());
		if (tpProperty)
		{
			std::cout << "Getting Tactical Points for process ID: " << procId << std::endl;
			return tpProperty->getTP(procId);
		}
	}
	return 0;
}

void Player::setPropertyRefreshInterval(const char *propertyName, unsigned int intervalMs)
{
	for (auto &config : propertyConfigs)
	{
		if (strcmp(config.property->getPropertyName(), propertyName) == 0)
		{
			config.monitoringIntervalMs = intervalMs;
			break;
		}
	}
}

void Player::refreshProperty(const char *propertyName)
{
	// Check if it's a static property
	if (strcmp(propertyName, "Player Name") == 0)
	{
		// Refresh player name for all valid processes
		for (const auto &pair : processes)
		{
			if (pair.second.isValid)
			{
				readPlayerName(pair.second);
			}
		}
		return;
	}

	if (strcmp(propertyName, "Player ID") == 0)
	{
		// Refresh player ID for all valid processes
		for (const auto &pair : processes)
		{
			if (pair.second.isValid)
			{
				readPlayerId(pair.second);
			}
		}
		return;
	}

	// Otherwise, look for the property in the monitored properties
	std::shared_ptr<PlayerProperty> targetProperty = nullptr;

	for (auto &config : propertyConfigs)
	{
		if (strcmp(config.property->getPropertyName(), propertyName) == 0)
		{
			targetProperty = config.property;
			// Reset last update time
			config.lastUpdateTime = std::chrono::steady_clock::now();
			break;
		}
	}

	if (!targetProperty)
	{
		std::cout << "Property not found: " << propertyName << std::endl;
		return;
	}

	// Refresh for all valid processes
	for (const auto &pair : processes)
	{
		if (pair.second.isValid)
		{
			targetProperty->refresh(pair.second);
		}
	}
}

void Player::forceRefreshStaticProperties()
{
	std::cout << "Force refreshing static properties for all processes..." << std::endl;

	for (const auto &pair : processes)
	{
		if (pair.second.isValid)
		{
			std::cout << "Refreshing static properties for process " << pair.first << std::endl;

			// Re-read player name and ID with retry logic
			int retryCount = 0;
			const int maxRetries = 3;
			bool success = false;

			while (retryCount < maxRetries && !success)
			{
				readPlayerName(pair.second);
				readPlayerId(pair.second);

				// Check if we got valid data
				std::string playerName = getPlayerName(pair.first);
				DWORD playerId = getPlayerId(pair.first);

				if (playerName != "Unknown" && playerName.length() > 1)
				{
					success = true;
					std::cout << "Successfully refreshed data for process " << pair.first
					         << ": Name='" << playerName << "', ID=" << playerId << std::endl;
				}
				else
				{
					retryCount++;
					if (retryCount < maxRetries)
					{
						std::cout << "Retry " << retryCount << " for process " << pair.first << std::endl;
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					}
				}
			}

			if (!success)
			{
				std::cout << "WARNING: Failed to refresh data for process " << pair.first
				         << " after " << maxRetries << " attempts" << std::endl;
			}
		}
	}
}

void Player::startMonitoring()
{
	if (monitoringActive)
		return; // Already monitoring

	monitoringActive = true;
	monitorThread = std::thread(&Player::monitorPropertiesThread, this);
}

void Player::stopMonitoring()
{
	if (!monitoringActive)
		return; // Not monitoring

	monitoringActive = false;
	if (monitorThread.joinable())
	{
		monitorThread.join();
	}
}

void Player::setMonitoringInterval(unsigned int intervalMs)
{
	if (intervalMs > 0)
	{
		defaultMonitoringIntervalMs = intervalMs;
	}
}

bool Player::isMonitoring() const
{
	return monitoringActive;
}

void Player::monitorPropertiesThread()
{
	std::cout << "Monitoring thread started" << std::endl;

	// Track time for process lifecycle checks
	auto lastProcessCheckTime = std::chrono::steady_clock::now();
	const auto processCheckInterval = std::chrono::seconds(2); // Check for dead/new processes every 2 seconds

	while (monitoringActive)
	{
		auto currentTime = std::chrono::steady_clock::now();

		// Periodically check for dead processes and new processes
		if (currentTime - lastProcessCheckTime >= processCheckInterval)
		{
			checkForDeadProcesses();
			checkForNewProcesses();
			lastProcessCheckTime = currentTime;
		}

		// Check each property if it's time to update
		for (auto &config : propertyConfigs)
		{
			// Calculate elapsed time since last update
			auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
													 currentTime - config.lastUpdateTime)
													 .count();

			// Check if it's time to refresh this property
			if (elapsedMs >= config.monitoringIntervalMs)
			{
				// Update last refresh time
				config.lastUpdateTime = currentTime;

				// Refresh for all valid processes
				for (const auto &pair : processes)
				{
					if (pair.second.isValid)
					{
						// Refresh the property
						config.property->refresh(pair.second);

						// Check if the property has changed
						if (config.property->hasChanged(pair.first))
						{
							// Report the change
							config.property->reportChange(pair.first);

							// Acknowledge the change
							config.property->acknowledgeChange(pair.first);
						}
					}
				}
			}
		}

		// Sleep for a short time to avoid CPU overuse
		// Use a shorter interval than any property's update interval to ensure timely updates
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	std::cout << "Monitoring thread stopped" << std::endl;
}
