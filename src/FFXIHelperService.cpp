#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

#include "Player/Player.h"
#include "helpers/logger.h"

// Global flag for handling Ctrl+C
std::atomic<bool> g_running(true);

void signalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		LOG("SIGNAL", "Shutdown signal received");
		std::cout << "\nShutting down gracefully..." << std::endl;
		g_running = false;
	}
}

int main()
{
	// Initialize logger FIRST
	LOG_INIT();
	LOG("MAIN", "==========================================");
	LOG("MAIN", "FFXI Helper Service Starting");
	LOG("MAIN", "==========================================");
	LOG_FLUSH();

	// Register signal handlers
	LOG("MAIN", "Registering signal handlers");
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	LOG_FLUSH();

	try
	{
		LOG("MAIN", "Starting FFXI Helper Service...");
		std::cout << "Starting FFXI Helper Service..." << std::endl;
		LOG_FLUSH();

		// Create the player manager
		LOG("MAIN", "Creating Player instance");
		LOG_FLUSH();
		Player player;
		LOG("MAIN", "Player instance created successfully");
		LOG_FLUSH();

		// Enable chat monitoring
		LOG("MAIN", "Enabling chat monitoring...");
		std::cout << "Enabling chat monitoring..." << std::endl;
		std::cout.flush();
		LOG_FLUSH();
		
		player.enableChatMonitoring();
		
		LOG("MAIN", "Chat monitoring enabled successfully");
		std::cout << "Chat monitoring ENABLED for all game instances" << std::endl;
		std::cout.flush();
		LOG_FLUSH();

	// Display all player data
	LOG("MAIN", "Displaying player data...");
	std::cout << "Displaying player data..." << std::endl;
	std::cout.flush();
	LOG_FLUSH();
	
	player.displayAllPlayerData();
	
	LOG("MAIN", "Player data displayed successfully");
	std::cout << "Player data displayed!" << std::endl;
	std::cout.flush();
	LOG_FLUSH();

	// Interactive loop to allow commands
	LOG("MAIN", "Starting interactive loop");
	std::cout << "\nFFXI Helper Service is running..." << std::endl;
	std::cout.flush();
	std::cout << "Commands:" << std::endl;
	std::cout << "  r - Refresh player data" << std::endl;
	std::cout << "  d - Display current data" << std::endl;
	std::cout << "  c - Toggle chat monitoring" << std::endl;
	std::cout << "  q - Quit" << std::endl;
	std::cout << "\nPress Ctrl+C to exit" << std::endl;
	std::cout << "\n[MAIN] Entering infinite loop..." << std::endl;
	std::cout.flush();
	LOG_FLUSH();

	// Keep the service running indefinitely
	bool chatMonitoringEnabled = true;
	int loopCount = 0;
	LOG("MAIN", "g_running = " + std::to_string(g_running.load()));
	std::cout << "[MAIN] g_running = " << g_running << std::endl;
	std::cout.flush();
	LOG_FLUSH();

	// Give Elite API instances time to fully initialize before first poll
	LOG("MAIN", "Waiting 1 second before starting chat polling...");
	std::cout << "[MAIN] Waiting 1 second before starting chat polling..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	LOG("MAIN", "Starting chat polling loop...");
	std::cout << "[MAIN] Starting chat polling loop..." << std::endl;
	std::cout.flush();
	LOG_FLUSH();

	while (g_running)
	{
		// Poll chat messages from all Elite API instances (main-thread polling to avoid DLL thread-safety issues)
		try
		{
			if (loopCount % 10 == 0) // Log every 10th poll
			{
				LOG("MAIN", "Polling chat messages (loop " + std::to_string(loopCount) + ")");
				LOG_FLUSH();
			}
			
			player.pollChatMessages();
			
			if (loopCount % 10 == 0)
			{
				LOG("MAIN", "Poll completed successfully");
				LOG_FLUSH();
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("MAIN", "chat poll", std::string(e.what()));
			std::cout << "[MAIN] Exception during chat poll: " << e.what() << std::endl;
			std::cout.flush();
			LOG_FLUSH();
		}
		catch (...)
		{
			LOG_ERROR("MAIN", "chat poll", "Unknown exception");
			std::cout << "[MAIN] Unknown exception during chat poll" << std::endl;
			std::cout.flush();
			LOG_FLUSH();
		}

		// Sleep to prevent CPU spinning (100ms = 10 polls per second)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		loopCount++;

		if (loopCount % 100 == 0) // Every 10 seconds
		{
			LOG("MAIN", "Service running (" + std::to_string(loopCount / 10) + "s)");
			std::cout << "[MAIN] Service running (" << (loopCount / 10) << "s)" << std::endl;
			std::cout.flush();
			LOG_FLUSH();
		}
	}

	LOG("MAIN", "Exiting main loop (g_running = " + std::to_string(g_running.load()) + ")");
	std::cout << "\n[MAIN] Exiting main loop (g_running = " << g_running << ")..." << std::endl;
	std::cout.flush();
	LOG_FLUSH();
	
	std::cout << "Shutting down..." << std::endl;
	LOG("MAIN", "Main function completed successfully");
	LOG_FLUSH();
	Logger::Close();
	return 0;
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("MAIN", "main", std::string(e.what()));
		LOG_FLUSH();
		std::cerr << "FATAL ERROR: " << e.what() << std::endl;
		std::cerr << "Press Enter to exit..." << std::endl;
		LOG("MAIN", "Waiting for user input before exit");
		LOG_FLUSH();
		Logger::Close();
		std::cin.get();
		return 1;
	}
	catch (...)
	{
		LOG_ERROR("MAIN", "main", "Unknown fatal exception");
		LOG_FLUSH();
		std::cerr << "FATAL ERROR: Unknown exception caught" << std::endl;
		std::cerr << "Press Enter to exit..." << std::endl;
		LOG("MAIN", "Waiting for user input before exit");
		LOG_FLUSH();
		Logger::Close();
		std::cin.get();
		return 1;
	}
}
