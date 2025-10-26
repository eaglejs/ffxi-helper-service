#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>

#include "Player/Player.h"

int main()
{
	std::cout << "Starting FFXI Helper Service..." << std::endl; // Good breakpoint location

	// Create the player manager
	Player player;

	// Enable chat monitoring
	std::cout << "Enabling chat monitoring..." << std::endl;
	player.enableChatMonitoring();
	std::cout << "Chat monitoring enabled! Messages will be sent to http://192.168.5.30:8080/set_messages" << std::endl;

	// Display all player data
	std::cout << "Displaying player data..." << std::endl; // Another good breakpoint location
	player.displayAllPlayerData();

	// Interactive loop to allow commands
	std::cout << "\nFFXI Helper Service is running..." << std::endl;
	std::cout << "Commands:" << std::endl;
	std::cout << "  r - Refresh player data" << std::endl;
	std::cout << "  d - Display current data" << std::endl;
	std::cout << "  c - Toggle chat monitoring" << std::endl;
	std::cout << "  q - Quit" << std::endl;
	std::cout << "Enter command: ";

	char command;
	bool chatMonitoringEnabled = true;
	while (std::cin >> command)
	{
		switch (command)
		{
		case 'r':
		case 'R':
			std::cout << "Forcing refresh of all player data..." << std::endl;
			player.forceRefreshStaticProperties();
			player.displayAllPlayerData();
			break;
		case 'd':
		case 'D':
			std::cout << "Current player data:" << std::endl;
			player.displayAllPlayerData();
			break;
		case 'c':
		case 'C':
			if (chatMonitoringEnabled)
			{
				std::cout << "Disabling chat monitoring..." << std::endl;
				player.disableChatMonitoring();
				chatMonitoringEnabled = false;
				std::cout << "Chat monitoring disabled." << std::endl;
			}
			else
			{
				std::cout << "Enabling chat monitoring..." << std::endl;
				player.enableChatMonitoring();
				chatMonitoringEnabled = true;
				std::cout << "Chat monitoring enabled." << std::endl;
			}
			break;
		case 'q':
		case 'Q':
			std::cout << "Shutting down..." << std::endl;
			return 0;
		default:
			std::cout << "Unknown command. Use r (refresh), d (display), or q (quit)" << std::endl;
			break;
		}
		std::cout << "Enter command: ";
	}

	std::cout << "Shutting down..." << std::endl; // Breakpoint to catch shutdown
	return 0;
}
