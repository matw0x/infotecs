#pragma once

#include <string>
#include <fstream>
#include <mutex>

enum LogLevel { INFO, WARNING, ERROR };

class Logger {
private:
    std::string filename_;
    LogLevel logLevel_;
    std::mutex logMutex_;

    void validateFile(const std::ofstream& file) const;
    void validateFileWriteSuccess(const std::ofstream& file) const;
    std::string getCurrentTime() const;
    std::string getLogLevelString(LogLevel logLevel) const;

public:
    explicit Logger(const std::string& filename, LogLevel logLevel = INFO) noexcept;

    void log(const std::string& message, LogLevel logLevel = INFO);
};