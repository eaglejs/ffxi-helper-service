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

	// Display all player data
	std::cout << "Displaying player data..." << std::endl; // Another good breakpoint location
	player.displayAllPlayerData();

	// Interactive loop to allow commands
	std::cout << "\nFFXI Helper Service is running..." << std::endl;
	std::cout << "Commands:" << std::endl;
	std::cout << "  r - Refresh player data" << std::endl;
	std::cout << "  d - Display current data" << std::endl;
	std::cout << "  q - Quit" << std::endl;
	std::cout << "Enter command: ";

	char command;
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
