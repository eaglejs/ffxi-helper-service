#include "Player/TacticalPointsProperty.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Global reference to the player instance - will be set during initialization
Player *g_playerInstance = nullptr;

// Static API endpoint definition
const std::string TacticalPointsProperty::API_ENDPOINT = "http://192.168.5.30:8080/set_tp";

TacticalPointsProperty::TacticalPointsProperty()
{
	// Set up HTTP client with JSON headers
	httpClient.setHeader("Content-Type", "application/json")
	          .setHeader("Accept", "application/json");
}

TacticalPointsProperty::~TacticalPointsProperty()
{
	// Clean up if needed
}

void TacticalPointsProperty::refresh(const PlayerProcessInfo &process)
{
	// Get TP address
	uintptr_t tpAddress = FindDMAAddyInDLL(
			process.hProcess,
			process.procId,
			dllName,
			offsetToBaseAddress,
			offsets);

	if (tpAddress == 0)
	{
		std::cout << "Failed to find TP address for process " << process.procId << std::endl;
		return;
	}

	std::lock_guard<std::mutex> lock(propertyMutex);

	// Store previous value for change detection
	int previousValue = 0;
	auto it = tacticalPoints.find(process.procId);
	if (it != tacticalPoints.end())
	{
		previousValue = it->second;
		previousTP[process.procId] = previousValue;
	}

	// Read TP value from memory
	int tpValue = 0;
	if (ReadProcessMemory(process.hProcess, (BYTE *)tpAddress, &tpValue, sizeof(tpValue), nullptr))
	{
		tacticalPoints[process.procId] = tpValue;

		// Check if value changed
		if (previousValue != tpValue)
		{
			changedFlags[process.procId] = true;
		}
	}
	else
	{
		std::cout << "Failed to read TP for process " << process.procId << std::endl;
	}
}

const char *TacticalPointsProperty::getPropertyName() const
{
	return "Tactical Points";
}

void TacticalPointsProperty::displayValue(DWORD procId) const
{
	auto it = tacticalPoints.find(procId);
	if (it != tacticalPoints.end())
	{
		std::cout << it->second;
	}
	else
	{
		std::cout << "0";
	}
}

int TacticalPointsProperty::getTP(DWORD procId) const
{
	std::lock_guard<std::mutex> lock(propertyMutex);
	auto it = tacticalPoints.find(procId);
	return (it != tacticalPoints.end()) ? it->second : 0;
}

bool TacticalPointsProperty::hasChanged(DWORD procId) const
{
	std::lock_guard<std::mutex> lock(propertyMutex);
	auto it = changedFlags.find(procId);
	return (it != changedFlags.end()) && it->second;
}

void TacticalPointsProperty::acknowledgeChange(DWORD procId)
{
	std::lock_guard<std::mutex> lock(propertyMutex);
	changedFlags[procId] = false;
}

std::string TacticalPointsProperty::sanitizePlayerName(const std::string& rawName) const
{
	std::string sanitized;
	sanitized.reserve(rawName.length());

	for (char c : rawName)
	{
		// Only include printable ASCII characters (space to tilde: 32-126)
		// and exclude control characters
		if (c >= 32 && c <= 126)
		{
			sanitized += c;
		}
	}

	// Trim whitespace from beginning and end
	size_t start = sanitized.find_first_not_of(" \t");
	if (start == std::string::npos)
	{
		return "Unknown"; // String was all whitespace or empty
	}

	size_t end = sanitized.find_last_not_of(" \t");
	sanitized = sanitized.substr(start, end - start + 1);

	// If resulting string is empty or too short, return default
	if (sanitized.empty() || sanitized.length() < 2)
	{
		return "Unknown";
	}

	return sanitized;
}

void TacticalPointsProperty::sendTPUpdate(const std::string& playerName, DWORD playerId, int tp) const
{
	try
	{
		// Create JSON payload
		std::ostringstream jsonPayload;
		jsonPayload << "{"
		           << "\"playerName\":\"" << playerName << "\","
		           << "\"playerId\":" << playerId << ","
		           << "\"tp\":" << tp
		           << "}";

		// Send POST request
		HttpClient::HttpResponse response = httpClient.post(API_ENDPOINT, jsonPayload.str());

		if (response.isSuccess())
		{
			std::cout << "Successfully sent TP update for " << playerName << " (TP: " << tp << ")" << std::endl;
		}
		else
		{
			std::cout << "Failed to send TP update for " << playerName << ". HTTP " << response.statusCode
			         << ": " << response.body << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error sending TP update for " << playerName << ": " << e.what() << std::endl;
	}
}

void TacticalPointsProperty::reportChange(DWORD procId) const
{
	std::lock_guard<std::mutex> lock(propertyMutex);

	auto currentIt = tacticalPoints.find(procId);
	auto previousIt = previousTP.find(procId);

	int currentValue = (currentIt != tacticalPoints.end()) ? currentIt->second : 0;
	int prevValue = (previousIt != previousTP.end()) ? previousIt->second : 0;

	// Get the player name if possible
	std::string playerName = "Unknown";
	if (g_playerInstance)
	{
		std::string rawName = g_playerInstance->getPlayerName(procId);
		playerName = sanitizePlayerName(rawName);
	}

	std::cout << "Player [" << playerName << "] (PID: " << procId << ") - TP changed from " << prevValue
						<< " to " << currentValue << std::endl;

	// Get the player ID (different from process ID)
	DWORD playerId = 0;
	if (g_playerInstance)
	{
		playerId = g_playerInstance->getPlayerId(procId);
	}

	// Send HTTP update to API endpoint
	sendTPUpdate(playerName, playerId, currentValue);
}
