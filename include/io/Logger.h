#pragma once

#include <fstream>
#include <mutex>
#include <string>

namespace cfd {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error
};

class Logger {
public:
    static Logger& instance();

    void setLevel(LogLevel level);
    void setLogFile(const std::string& path);

    void log(LogLevel level, const std::string& message);
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

private:
    Logger() = default;

    std::string levelToString(LogLevel level) const;
    std::string timestamp() const;

    LogLevel level_ = LogLevel::Info;
    std::ofstream logFile_;
    mutable std::mutex mutex_;
};

} // namespace cfd
