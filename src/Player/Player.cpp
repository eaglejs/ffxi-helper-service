#include <Windows.h>
#include "Player/Player.h"
#include "Player/TacticalPointsProperty.h"
#include "Player/ChatLogProperty.h"
#include "Player/EliteAPI.h"
#include "helpers/memory.h"
#include "helpers/http.h"
#include "helpers/logger.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

// Helper function to escape strings for JSON
std::string escapeJsonString(const std::string &input)
{
	std::ostringstream escaped;
	for (unsigned char c : input)
	{
		// Skip or escape control characters (0x00-0x1F and 0x7F-0xFF)
		if (c < 0x20 || c >= 0x7F)
		{
			// Skip FFXI control/color codes
			continue;
		}
		else if (c == '"')
		{
			escaped << "\\\"";
		}
		else if (c == '\\')
		{
			escaped << "\\\\";
		}
		else if (c == '\b')
		{
			escaped << "\\b";
		}
		else if (c == '\f')
		{
			escaped << "\\f";
		}
		else if (c == '\n')
		{
			escaped << "\\n";
		}
		else if (c == '\r')
		{
			escaped << "\\r";
		}
		else if (c == '\t')
		{
			escaped << "\\t";
		}
		else
		{
			escaped << c;
		}
	}
	return escaped.str();
}

// Initialize static members
std::map<DWORD, PlayerProcessInfo> Player::processes;

// Static property offset definitions
const std::vector<unsigned int> Player::PLAYER_NAME_OFFSETS = {0xA4};
const std::vector<unsigned int> Player::PLAYER_ID_OFFSETS = {0x4E0};
const std::vector<unsigned int> Player::PLAYER_CONQUEST_OFFSETS = {0x8C};

// For TacticalPointsProperty to access player names
extern Player *g_playerInstance;

Player::Player() : monitoringActive(false), chatMonitoringEnabled(false), shutdownChatMonitoring(false)
{
	// Set the global instance for properties to access
	g_playerInstance = this;
	// Initialize processes first
	initializeProcesses();

	// Read static properties (name and ID) once
	readStaticProperties();

	// TEMPORARILY DISABLED: Register tactical points for continuous monitoring (update every 100ms)
	// registerProperty(std::make_shared<TacticalPointsProperty>(), 100);

	// Refresh all dynamic properties initially
	refreshAllProperties();

	// TEMPORARILY DISABLED: Start the monitoring thread
	// startMonitoring();
	std::cout << "[Player] TP monitoring disabled for testing" << std::endl;
}

Player::~Player()
{
	std::cout << "[Player] Destructor called - cleaning up..." << std::endl;
	std::cout.flush();

	// Clear the global instance
	if (g_playerInstance == this)
	{
		g_playerInstance = nullptr;
	}

	// Stop monitoring thread
	std::cout << "[Player] Stopping monitoring..." << std::endl;
	std::cout.flush();
	stopMonitoring();
	std::cout << "[Player] Monitoring stopped" << std::endl;
	std::cout.flush();

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

		processes[currentProcId] = std::move(info);

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

void Player::readPlayerName(const PlayerProcessInfo &process)
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
	if (ReadProcessMemory(process.hProcess, (BYTE *)nameAddress, nameBuffer, 16, nullptr))
	{
		nameBuffer[16] = '\0'; // Ensure null termination

		// Validate that we got a reasonable player name
		std::string rawName(nameBuffer);

		// Check if the name contains only printable characters and isn't empty
		bool validName = !rawName.empty() && rawName.length() > 1;
		for (char c : rawName)
		{
			if (c != 0 && (c < 32 || c > 126)) // Non-printable character
			{
				validName = false;
				break;
			}
			if (c == 0)
				break; // Null terminator, stop checking
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

void Player::readPlayerId(const PlayerProcessInfo &process)
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
	if (ReadProcessMemory(process.hProcess, (BYTE *)idAddress, &playerId, sizeof(playerId), nullptr))
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

		// Cleanup Elite API instance for this process
		auto eliteIt = eliteAPIInstances.find(procId);
		if (eliteIt != eliteAPIInstances.end())
		{
			if (eliteIt->second)
			{
				eliteIt->second->StopChatMonitoring();
				eliteIt->second->Cleanup();
			}
			eliteAPIInstances.erase(eliteIt);
			std::cout << "Cleaned up Elite API for dead process " << procId << std::endl;
		}

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
	for (const auto &pair : processes)
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
			processes[procId] = std::move(info);
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
	std::cout.flush();

	// Track time for process lifecycle checks
	auto lastProcessCheckTime = std::chrono::steady_clock::now();
	const auto processCheckInterval = std::chrono::seconds(2); // Check for dead/new processes every 2 seconds

	try
	{
		while (monitoringActive)
		{
			try
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
								try
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
								catch (const std::exception &e)
								{
									std::cout << "[Monitoring] Exception refreshing property for process " << pair.first << ": " << e.what() << std::endl;
									std::cout.flush();
								}
								catch (...)
								{
									std::cout << "[Monitoring] Unknown exception refreshing property for process " << pair.first << std::endl;
									std::cout.flush();
								}
							}
						}
					}
				}

				// Sleep for a short time to avoid CPU overuse
				// Use a shorter interval than any property's update interval to ensure timely updates
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			catch (const std::exception &e)
			{
				std::cout << "[Monitoring] Exception in monitoring loop: " << e.what() << std::endl;
				std::cout.flush();
			}
			catch (...)
			{
				std::cout << "[Monitoring] Unknown exception in monitoring loop" << std::endl;
				std::cout.flush();
			}
		}
	}
	catch (...)
	{
		std::cout << "[Monitoring] Fatal exception in monitoring thread" << std::endl;
		std::cout.flush();
	}

	std::cout << "Monitoring thread stopped" << std::endl;
	std::cout.flush();
}

// Chat monitoring implementations
void Player::enableChatMonitoring()
{
	chatMonitoringEnabled = true;

	std::cout << "[Player] Enabling chat monitoring using Elite API..." << std::endl;

	// Initialize Elite API instances for all active processes
	// Stagger initialization to avoid overload
	std::lock_guard<std::mutex> lock(processMutex);
	int initCount = 0;

	for (const auto &pair : processes)
	{
		if (pair.second.isValid)
		{
			DWORD procId = pair.first;

			// Check if Elite API instance already exists for this process
			if (eliteAPIInstances.find(procId) == eliteAPIInstances.end())
			{
				try
				{
					std::cout << "[Player] Creating Elite API instance for process " << procId << "..." << std::endl;
					std::cout.flush();

					auto eliteAPI = std::make_shared<EliteAPI>();

					std::cout << "[Player] Calling Initialize for process " << procId << "..." << std::endl;
					std::cout.flush();
					LOG("PLAYER", "About to call eliteAPI->Initialize(" + std::to_string(procId) + ")");
					LOG_FLUSH();

					bool initResult = false;
					try {
						initResult = eliteAPI->Initialize(procId);
						LOG("PLAYER", "Initialize returned: " + std::string(initResult ? "true" : "false"));
						LOG_FLUSH();
					}
					catch (const std::exception& e) {
						LOG_ERROR("PLAYER", "Initialize", "Exception: " + std::string(e.what()));
						LOG_FLUSH();
						initResult = false;
					}
					catch (...) {
						LOG_ERROR("PLAYER", "Initialize", "Unknown exception");
						LOG_FLUSH();
						initResult = false;
					}

					LOG("PLAYER", "After Initialize() try-catch block");
					LOG_FLUSH();
					LOG("PLAYER", "About to print initResult to console, value = " + std::string(initResult ? "true" : "false"));
					LOG_FLUSH();
					std::cout << "[Player] After Initialize call, result = " << initResult << std::endl;
					std::cout.flush();
					LOG("PLAYER", "Console output completed");
					LOG_FLUSH();

					if (initResult)
					{
						LOG("PLAYER", "initResult is true, entering success block");
						LOG_FLUSH();
						std::cout << "[Player] Initialize succeeded for process " << procId << std::endl;
						std::cout.flush();
						LOG("PLAYER", "About to register chat callback");
						LOG_FLUSH();

						// Register chat callback
						eliteAPI->RegisterChatCallback([this, procId](const ChatMessage &msg)
																					 { this->onChatMessage(procId, msg); });

						LOG("PLAYER", "Chat callback registered, about to start monitoring");
						LOG_FLUSH();
						std::cout << "[Player] Calling StartChatMonitoring for process " << procId << "..." << std::endl;
						std::cout.flush();
						LOG("PLAYER", "About to call StartChatMonitoring");
						LOG_FLUSH();

						// Start chat monitoring
						eliteAPI->StartChatMonitoring();

						std::cout << "[Player] StartChatMonitoring completed for process " << procId << std::endl;
						std::cout.flush();

						eliteAPIInstances[procId] = eliteAPI;
						std::cout << "[Player] Elite API chat monitoring started for process " << procId << std::endl;
						std::cout.flush();

						// Stagger initialization - small delay between processes
						initCount++;
						std::cout << "[Player] Initialized " << initCount << " Elite API instances" << std::endl;
						std::cout.flush();

						// Small delay to avoid overwhelming the DLL
						std::cout << "[Player] Sleeping 200ms before next process..." << std::endl;
						std::cout.flush();
						std::this_thread::sleep_for(std::chrono::milliseconds(200));
						std::cout << "[Player] Sleep completed" << std::endl;
						std::cout.flush();
					}
					else
					{
						std::cout << "[Player] Failed to initialize Elite API for process " << procId << std::endl;
					}
				}
				catch (const std::exception &e)
				{
					std::cout << "[Player] Exception initializing Elite API for process " << procId << ": " << e.what() << std::endl;
					std::cout.flush();
				}
				catch (...)
				{
					std::cout << "[Player] Unknown exception initializing Elite API for process " << procId << std::endl;
					std::cout.flush();
				}
			}
		}
	}

	std::cout << "[Player] Chat monitoring enabled for " << eliteAPIInstances.size() << " processes!" << std::endl;
	std::cout.flush(); // Force output to be written
}

void Player::pollChatMessages()
{
	if (!chatMonitoringEnabled)
	{
		return;
	}

	LOG("PLAYER", "pollChatMessages: Creating instance copy");
	LOG_FLUSH();

	// Create a copy of the map to avoid holding the lock during DLL calls
	std::map<DWORD, std::shared_ptr<EliteAPI>> instancesCopy;
	{
		std::lock_guard<std::mutex> lock(processMutex);
		instancesCopy = eliteAPIInstances;
	}

	LOG("PLAYER", "pollChatMessages: Polling " + std::to_string(instancesCopy.size()) + " instances");
	LOG_FLUSH();

	// Poll all Elite API instances for new messages (without holding the lock)
	int totalMessages = 0;
	for (auto &[procId, eliteAPI] : instancesCopy)
	{
		if (eliteAPI)
		{
			try
			{
				LOG("PLAYER", "Polling process " + std::to_string(procId));
				LOG_FLUSH();
				
				int count = eliteAPI->PollChatMessages();
				totalMessages += count;
				
				if (count > 0)
				{
					LOG("PLAYER", "Process " + std::to_string(procId) + " returned " + std::to_string(count) + " messages");
					LOG_FLUSH();
				}
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("PLAYER", "pollChatMessages", "Exception polling process " + std::to_string(procId) + ": " + std::string(e.what()));
				std::cout << "[Player] Exception polling process " << procId << ": " << e.what() << std::endl;
				LOG_FLUSH();
			}
			catch (...)
			{
				LOG_ERROR("PLAYER", "pollChatMessages", "Unknown exception polling process " + std::to_string(procId));
				std::cout << "[Player] Unknown exception polling process " << procId << std::endl;
				LOG_FLUSH();
			}
		}
	}

	// Optional: Log only if messages were processed
	// if (totalMessages > 0)
	// {
	//     std::cout << "[Player] Polled " << totalMessages << " new chat messages" << std::endl;
	// }
}

void Player::disableChatMonitoring()
{
	std::lock_guard<std::mutex> lock(processMutex);
	chatMonitoringEnabled = false;
	shutdownChatMonitoring = true;

	std::cout << "[Player] Disabling chat monitoring..." << std::endl;

	// Stop Elite API monitoring for all processes
	for (auto &[procId, eliteAPI] : eliteAPIInstances)
	{
		if (eliteAPI)
		{
			eliteAPI->StopChatMonitoring();
			eliteAPI->Cleanup();
			std::cout << "[Player] Stopped Elite API monitoring for process " << procId << std::endl;
		}
	}
	eliteAPIInstances.clear();

	// Unregister chat callback from legacy method
	if (chatLogProperty)
	{
		chatLogProperty->UnregisterCallback();
	}

	// Wait for all debounce threads to finish
	for (auto &[procId, thread] : chatDebounceThreads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
	chatDebounceThreads.clear();

	processChats.clear();
	lastChatTime.clear();
	shutdownChatMonitoring = false;

	std::cout << "[Player] Chat monitoring disabled" << std::endl;
}

void Player::onChatMessage(DWORD procId, const ChatMessage &msg)
{
	// Skip UNKNOWN message types - don't store or send them
	if (msg.type == ChatMessageType::Unknown)
	{
		std::cout << "[Chat] Skipping UNKNOWN message type: " << msg.message << std::endl;
		return;
	}

	std::lock_guard<std::mutex> lock(chatMutex);

	// Store message
	processChats[procId].push_back(msg);

	// Keep only last 100 messages per process
	if (processChats[procId].size() > 100)
	{
		processChats[procId].pop_front();
	}

	// Get player name for better logging
	std::string playerName = getPlayerName(procId);
	std::cout << "[Chat][" << playerName << " PID:" << procId << "] "
						<< msg.sender << " (" << msg.getMessageTypeString()
						<< "): " << msg.message << std::endl;

	// Update last chat time for debouncing
	lastChatTime[procId] = std::chrono::steady_clock::now();

	// Start debounce thread if not already running
	if (!debounceThreadRunning[procId])
	{
		// Start new debounce thread
		std::cout << "[Chat] Starting new debounce thread for process " << procId << std::endl;
		debounceThreadRunning[procId] = true;

		std::thread([this, procId]()
								{ this->chatDebounceThread(procId); })
				.detach(); // Detach so we don't need to manage thread lifecycle
	}
	else
	{
		std::cout << "[Chat] Debounce thread already running for process " << procId << std::endl;
	}
}

std::vector<ChatMessage> Player::getRecentChatMessages(DWORD procId, int count)
{
	std::lock_guard<std::mutex> lock(chatMutex);

	std::vector<ChatMessage> messages;

	if (processChats.find(procId) != processChats.end())
	{
		auto &deque = processChats[procId];
		int start = std::max(0, static_cast<int>(deque.size()) - count);

		for (size_t i = start; i < deque.size(); i++)
		{
			messages.push_back(deque[i]);
		}
	}

	return messages;
}

void Player::chatDebounceThread(DWORD procId)
{
	// Debounce period: wait for messages to stop arriving
	const auto debounceDelay = std::chrono::milliseconds(500); // 500ms after last message

	while (!shutdownChatMonitoring)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Check if we have messages and if enough time has passed
		bool shouldSend = false;
		{
			std::lock_guard<std::mutex> lock(chatMutex);

			if (processChats.find(procId) == processChats.end() || processChats[procId].empty())
			{
				// No messages, clear flag and exit thread
				debounceThreadRunning[procId] = false;
				return;
			}

			auto now = std::chrono::steady_clock::now();
			auto timeSinceLastMsg = now - lastChatTime[procId];

			if (timeSinceLastMsg >= debounceDelay)
			{
				shouldSend = true;
			}
		}

		if (shouldSend)
		{
			// Send the batch
			std::vector<ChatMessage> batch;
			{
				std::lock_guard<std::mutex> lock(chatMutex);
				for (const auto &msg : processChats[procId])
				{
					batch.push_back(msg);
				}
				processChats[procId].clear();
			}

			std::cout << "[Chat] Debounce complete, sending " << batch.size()
								<< " messages for process " << procId << std::endl;

			sendChatBatch(procId, batch);

			// Clear flag and exit thread after sending
			debounceThreadRunning[procId] = false;
			return;
		}
	}

	// If we exit due to shutdown, clear the flag
	debounceThreadRunning[procId] = false;
}

void Player::sendChatBatch(DWORD procId, const std::vector<ChatMessage> &messages)
{
	if (messages.empty())
	{
		return;
	}

	// Get player info
	std::string playerName = getPlayerName(procId);
	DWORD playerId = getPlayerId(procId);

	if (playerName.empty() || playerName == "Unknown" || playerId == 0)
	{
		std::cout << "[Chat] Skipping chat batch - player info not available for process " << procId << std::endl;
		return;
	}

	try
	{
		// Group messages by type
		std::map<std::string, std::vector<std::string>> messagesByType;

		for (const auto &msg : messages)
		{
			std::string typeKey = msg.getMessageTypeString();
			// Use the original raw content to preserve formatting
			messagesByType[typeKey].push_back(msg.rawContent);
		}

		// Send each message type as a separate request
		for (const auto &[messageType, msgList] : messagesByType)
		{
			// Build JSON payload matching your endpoint structure
			// messages is an object with string keys starting from "1" (Lua convention)
			std::ostringstream json;
			json << "{"
					 << "\"playerId\":" << playerId << ","
					 << "\"playerName\":\"" << escapeJsonString(playerName) << "\","
					 << "\"messageType\":\"" << escapeJsonString(messageType) << "\","
					 << "\"messages\":{";

			for (size_t i = 0; i < msgList.size(); i++)
			{
				if (i > 0)
					json << ",";
				// Use 1-based indexing (Lua convention)
				json << "\"" << (i + 1) << "\":\"" << escapeJsonString(msgList[i]) << "\"";
			}

			json << "}}";

			// Send to your endpoint
			HttpClient client;
			client.setHeader("Content-Type", "application/json")
					.setHeader("Accept", "application/json")
					.setTimeout(10);

			std::string endpoint = "http://192.168.5.30:8080/set_messages";

			std::cout << "[Chat][" << playerName << " PID:" << procId << "] Sending "
								<< msgList.size() << " " << messageType << " messages" << std::endl;
			std::cout << "[Chat] JSON: " << json.str() << std::endl;

			HttpClient::HttpResponse response = client.post(endpoint, json.str());

			if (response.isSuccess())
			{
				std::cout << "[Chat] Successfully sent messages" << std::endl;
			}
			else
			{
				std::cout << "[Chat] Failed to send messages. HTTP " << response.statusCode
									<< ": " << response.body << std::endl;
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cout << "[Chat] Error sending chat batch: " << e.what() << std::endl;
	}
}

void Player::sendChatMessagesToServer(DWORD procId)
{
	std::vector<ChatMessage> messages;

	{
		std::lock_guard<std::mutex> lock(chatMutex);

		if (processChats.find(procId) == processChats.end() || processChats[procId].empty())
		{
			std::cout << "[Chat] No pending messages for process " << procId << std::endl;
			return;
		}

		// Copy messages
		for (const auto &msg : processChats[procId])
		{
			messages.push_back(msg);
		}

		processChats[procId].clear();
	}

	std::cout << "[Chat] Manually sending " << messages.size()
						<< " messages for process " << procId << std::endl;

	// Send immediately (not in background thread since this is manual)
	sendChatBatch(procId, messages);
}
