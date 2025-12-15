#pragma once

#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>

class Logger
{
private:
    static std::ofstream logFile;
    static std::mutex logMutex;
    static bool initialized;
    static std::string logFilePath;

    static std::string GetTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        std::tm tm;
        localtime_s(&tm, &time);
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

public:
    static void Initialize(const std::string& filename = "logs/debug_log.txt")
    {
        std::lock_guard<std::mutex> lock(logMutex);
        if (!initialized)
        {
            // Ensure logs directory exists
            std::filesystem::path logPath(filename);
            std::filesystem::path logDir = logPath.parent_path();
            if (!logDir.empty() && !std::filesystem::exists(logDir)) {
                std::error_code ec;
                std::filesystem::create_directories(logDir, ec);
                if (ec) {
                    std::cerr << "Failed to create log directory: " << ec.message() << std::endl;
                }
            }

            logFilePath = filename;
            logFile.open(filename, std::ios::out | std::ios::trunc);
            if (logFile.is_open())
            {
                initialized = true;
                logFile << "=== Log Started: " << GetTimestamp() << " ===" << std::endl;
                logFile.flush();
            }
        }
    }

    static void Log(const std::string& tag, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(logMutex);
        if (!initialized)
        {
            Initialize();
        }
        
        if (logFile.is_open())
        {
            logFile << "[" << GetTimestamp() << "] [" << tag << "] " << message << std::endl;
            logFile.flush(); // Force write immediately
        }
        
        // Also print to console
        std::cout << "[" << tag << "] " << message << std::endl;
        std::cout.flush();
    }

    static void LogFunction(const std::string& tag, const std::string& function, const std::string& message)
    {
        Log(tag, function + "() - " + message);
    }

    static void LogEnter(const std::string& tag, const std::string& function)
    {
        Log(tag, ">>> ENTER: " + function + "()");
    }

    static void LogExit(const std::string& tag, const std::string& function)
    {
        Log(tag, "<<< EXIT: " + function + "()");
    }

    static void LogError(const std::string& tag, const std::string& function, const std::string& error)
    {
        Log(tag, "ERROR in " + function + "(): " + error);
    }

    static void Close()
    {
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open())
        {
            logFile << "=== Log Ended: " << GetTimestamp() << " ===" << std::endl;
            logFile.close();
        }
        initialized = false;
    }

    static void Flush()
    {
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open())
        {
            logFile.flush();
        }
    }
};

// Macro helpers for easy logging
#define LOG_INIT() Logger::Initialize("logs/crash_debug_" + std::to_string(GetCurrentProcessId()) + ".log")
#define LOG(tag, msg) Logger::Log(tag, msg)
#define LOG_FUNC(tag, func, msg) Logger::LogFunction(tag, func, msg)
#define LOG_ENTER(tag, func) Logger::LogEnter(tag, func)
#define LOG_EXIT(tag, func) Logger::LogExit(tag, func)
#define LOG_ERROR(tag, func, err) Logger::LogError(tag, func, err)
#define LOG_FLUSH() Logger::Flush()
