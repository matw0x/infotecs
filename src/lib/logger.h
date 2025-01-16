#pragma once

#include <string>
#include <fstream>
#include <mutex>

enum LogLevel { INFO, WARNING, ERROR };
enum LogType { SAFELY, FAST };

class Logger {
private:
    std::string filename_;
    LogLevel logLevel_;
    LogType logType_;
    std::mutex logMutex_;
    std::ofstream logFile_;

    bool hasValidExtension();
    void validateFile();
    void validateFileWriteSuccess() const;
    std::string getCurrentTime() const;
    std::string getLogLevelString(LogLevel logLevel) const;

public:
    explicit Logger(const std::string& filename, LogLevel logLevel = INFO, LogType logType = SAFELY);

    void log(const std::string& message, LogLevel logLevel = INFO);
    void changeLogLevel(LogLevel newLogLevel);
    void changeLogType(LogType newLogType);
    LogLevel getLogLevel() const;
    LogType getLogType() const;
};