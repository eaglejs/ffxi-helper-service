#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>

#include "Player/Player.h"

int main()
{
	// Create the player manager
	Player player;

	// Display all player data
	player.displayAllPlayerData();

	// Keep the application running to monitor player data
	std::cout << "Press Enter to exit..." << std::endl;
	std::cin.get();

	return 0;
}
