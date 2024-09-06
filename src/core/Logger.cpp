#include "Logger.h"

#include <iostream>

namespace nex {
    Logger::LogLevel Logger::m_current_log_level = Logger::DEBUG;

    void Logger::Log(const std::string& message, LogLevel level) {
        if (level <= m_current_log_level) {
            std::cout << message << std::endl;
        }
    }

    void Logger::LogRelease(const std::string& message) {
        Log(message, RELEASE);
    }

    void Logger::LogDebug(const std::string& message) {
        Log(message, DEBUG);
    }
}  // namespace nex
