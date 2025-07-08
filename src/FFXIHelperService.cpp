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

	// Keep the application running to monitor player data
	std::cout << "Press Enter to exit..." << std::endl;
	std::cin.get();

	std::cout << "Shutting down..." << std::endl; // Breakpoint to catch shutdown
	return 0;
}
