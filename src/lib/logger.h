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
    std::ofstream logFile_;

    void validateFile() const;
    void validateFileWriteSuccess() const;
    std::string getCurrentTime() const;
    std::string getLogLevelString(LogLevel logLevel) const;

public:
    explicit Logger(const std::string& filename, LogLevel logLevel = INFO);

    void log(const std::string& message, LogLevel logLevel = INFO);
    void changeLogLevel(LogLevel newLogLevel);
};