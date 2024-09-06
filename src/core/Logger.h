#pragma once

#include <string>

namespace nex {
    class Logger {
      public:
        enum LogLevel { RELEASE, DEBUG };

        static void Log(const std::string& message, LogLevel level);

        static void LogRelease(const std::string& message);
        static void LogDebug(const std::string& message);

      private:
        static LogLevel m_current_log_level;
    };
}  // namespace nex
