#include "logger.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

constexpr char SPACE = ' ', END = '\n';

Logger::Logger(const std::string& filename, LogLevel logLevel, LogType logType) : 
filename_(filename), logLevel_(logLevel), logType_(logType), logFile_(filename_, std::ios::app) {
    validateFile();
}

bool Logger::hasValidExtension() {
    std::filesystem::path filePath(filename_);
    return filePath.extension() == ".txt";
}

void Logger::validateFile() {
    if (!hasValidExtension()) throw std::runtime_error("Error: file has invalid extension!");
    if (!logFile_.is_open()) throw std::runtime_error("Error: opening file!");
}

void Logger::validateFileWriteSuccess() const {
    if (logFile_.fail()) throw std::runtime_error("Error: failed to write to file!");
}

std::string Logger::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream timeStream;
    timeStream << 
    '[' << std::put_time(std::localtime(&in_time_t), "%d-%m-%Y %H:%M:%S") << ']';

    return timeStream.str();
}

std::string Logger::getLogLevelString(LogLevel logLevel) const {
    switch (logLevel) {
        case INFO: return "[INFO]";
        case WARNING: return "[WARNING]";
        case ERROR: return "[ERROR]";
        default: return "[UNKNOWN]";
    }
}

void Logger::log(const std::string& message, LogLevel logLevel) {
    if (logLevel < logLevel_ || message.empty()) return; // сообщения с уровнем ниже не записываются

    std::lock_guard<std::mutex> lock(logMutex_);
    validateFile();

    logFile_ << getCurrentTime() << SPACE << getLogLevelString(logLevel) << SPACE << message << END;

    if (logType_ == SAFELY) {
        logFile_.flush();
        validateFileWriteSuccess();
    }
}

void Logger::changeLogLevel(LogLevel newLogLevel) { logLevel_ = newLogLevel; }

void Logger::changeLogType(LogType newLogType) { logType_ = newLogType; }

LogLevel Logger::getLogLevel() const { return logLevel_; }

LogType Logger::getLogType() const { return logType_; }