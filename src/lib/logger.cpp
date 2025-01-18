#include "logger.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

constexpr char SPACE = ' ', END = '\n';  // для удобства

Logger::Logger(const std::string& filename, LogLevel logLevel, LogType logType)
    : filename_(filename), logLevel_(logLevel), logType_(logType), logFile_(filename_, std::ios::app) {
    validateFile();
    // после инициализации всех полей нужно удостовериться,
    // что файл соответствует требованиям.
    // если что-то не так, программа завершается
}

void Logger::validateFileExtension() const {
    std::filesystem::path filePath(filename_);
    if (filePath.extension() != ".txt") throw std::runtime_error("Error: file has invalid extension!");
}

void Logger::validateIsFileOpen() const {
    if (!logFile_.is_open()) throw std::runtime_error("Error: opening file!");
}

void Logger::validateFile() const {
    validateFileExtension();
    validateIsFileOpen();
}

void Logger::validateFileWriteSuccess() const {
    if (logFile_.fail()) throw std::runtime_error("Error: failed to write to file!");
}

std::string Logger::getCurrentTime() const {
    // для получения текущего времени используем chrono
    // записываем в строковый поток и
    // кладём время в формате день-месяц-год час-минута-секунда
    auto              now       = std::chrono::system_clock::now();
    auto              in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream timeStream;
    timeStream << '[' << std::put_time(std::localtime(&in_time_t), "%d-%m-%Y %H:%M:%S") << ']';

    return timeStream.str();
}

std::string Logger::getLogLevelString(LogLevel logLevel) const {
    // вспомогательный метод для получения времени, поскольку используем
    // перечисления
    switch (logLevel) {
        case INFO:
            return "[INFO]";
        case WARNING:
            return "[WARNING]";
        case ERROR:
            return "[ERROR]";
        default:
            return "[UNKNOWN]";
    }
}

void Logger::log(const std::string& message, LogLevel logLevel) {
    if (logLevel < logLevel_ || message.empty()) return;  // сообщения с уровнем ниже не записываются

    std::lock_guard<std::mutex> lock(logMutex_);  // предотвращаем гонку данных
    validateIsFileOpen();

    // записываем сообщение в красивом формате
    logFile_ << getCurrentTime() << SPACE << getLogLevelString(logLevel) << SPACE << message << END;

    if (logType_ == SAFELY) {
        logFile_.flush();  // сбрасываем буффер
        validateFileWriteSuccess();
    }  // для быстрой записи этого делать не будем
}

void Logger::changeLogLevel(LogLevel newLogLevel) { logLevel_ = newLogLevel; }

void Logger::changeLogType(LogType newLogType) { logType_ = newLogType; }

LogLevel Logger::getLogLevel() const { return logLevel_; }

LogType Logger::getLogType() const { return logType_; }