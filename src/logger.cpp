#include "logger.h"
#include <chrono>
#include <sstream>
#include <iomanip>

Logger::Logger(const std::string& filename, LogLevel logLevel) : 
filename_(filename), logLevel_(logLevel) {}

void Logger::validateFile(const std::ofstream& file) const {
    if (!file.is_open()) throw std::runtime_error("Error: opening file!");
}

void Logger::validateFileWriteSuccess(const std::ofstream& file) const {
    if (file.fail()) throw std::runtime_error("Error: failed to write to file!");
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

constexpr char SPACE = ' ';
constexpr char END = '\n';

void Logger::log(const std::string& message, LogLevel logLevel) {
    if (logLevel < logLevel_) return; // сообщения с уровнем ниже не записываются

    std::lock_guard<std::mutex> lock(logMutex_);

    std::ofstream logFile(filename_, std::ios::app);
    validateFile(logFile);

    logFile << getCurrentTime() << SPACE << getLogLevelString(logLevel) << SPACE << message << END;
    validateFileWriteSuccess(logFile);
}