#include "Player/PlayerNameProperty.h"
#include "helpers/memory.h"
#include <iostream>
#include <string.h> // For strcmp

PlayerNameProperty::PlayerNameProperty()
{
	// Constructor
}

PlayerNameProperty::~PlayerNameProperty()
{
	// Clean up allocated memory
	for (auto &pair : playerNames)
	{
		delete[] pair.second;
	}
	playerNames.clear();
}

void PlayerNameProperty::refresh(const PlayerProcessInfo &process)
{
	// Get player name address
	uintptr_t playerNameAddress = FindDMAAddyInDLL(
			process.hProcess,
			process.procId,
			dllName,
			offsetToBaseAddress,
			offsets);

	if (playerNameAddress == 0)
	{
		std::cout << "Failed to find player name address for process " << process.procId << std::endl;
		return;
	}

	// Clean up old name if it exists
	auto it = playerNames.find(process.procId);
	if (it != playerNames.end())
	{
		delete[] it->second;
	}

	// Allocate memory for new name
	char *playerName = new char[256];

	// Read player name from memory
	if (ReadProcessMemory(process.hProcess, (BYTE *)playerNameAddress, playerName, 256, nullptr))
	{
		playerNames[process.procId] = playerName;
		std::cout << "Updated player name for process " << process.procId << ": " << playerName << std::endl;
	}
	else
	{
		std::cout << "Failed to read player name for process " << process.procId << std::endl;
		delete[] playerName;
	}
}

const char *PlayerNameProperty::getPropertyName() const
{
	return "Player Name";
}

void PlayerNameProperty::displayValue(DWORD procId) const
{
	const char *name = getName(procId);
	if (name)
	{
		std::cout << name;
	}
	else
	{
		std::cout << "<unknown>";
	}
}

const char *PlayerNameProperty::getName(DWORD procId) const
{
	auto it = playerNames.find(procId);
	if (it != playerNames.end())
	{
		return it->second;
	}
	return nullptr;
}

bool PlayerNameProperty::hasChanged(DWORD procId) const
{
	std::lock_guard<std::mutex> lock(propertyMutex);
	auto it = changedFlags.find(procId);
	return (it != changedFlags.end()) && it->second;
}

void PlayerNameProperty::acknowledgeChange(DWORD procId)
{
	std::lock_guard<std::mutex> lock(propertyMutex);
	changedFlags[procId] = false;
}

void PlayerNameProperty::reportChange(DWORD procId) const
{
	std::lock_guard<std::mutex> lock(propertyMutex);

	auto currentIt = playerNames.find(procId);
	auto previousIt = previousNames.find(procId);

	const char *currentName = (currentIt != playerNames.end() && currentIt->second) ? currentIt->second : "<unknown>";
	std::string prevName = (previousIt != previousNames.end()) ? previousIt->second : "<unknown>";

	std::cout << "Process " << procId << " - Player name changed from '"
						<< prevName << "' to '" << currentName << "'" << std::endl;
}
