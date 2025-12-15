#include "helpers/logger.h"

// Static member initialization
std::ofstream Logger::logFile;
std::mutex Logger::logMutex;
bool Logger::initialized = false;
std::string Logger::logFilePath;
