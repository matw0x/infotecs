#pragma once

#include <fstream>
#include <mutex>
#include <string>

enum LogLevel { INFO, WARNING, ERROR };  // перечисление для уровня важности
enum LogType { SAFELY, FAST };  // безопасная и быстрая (данные могут потеряться) запись сообщений

class Logger {
   private:
    std::string   filename_;  // имя файла
    LogLevel      logLevel_;  // уровень важности
    LogType       logType_;   // тип записи
    std::mutex    logMutex_;  // мьютекс для обеспечения потокобезопасности
    std::ofstream logFile_;   // журнал сообщений

    void validateFileExtension() const;     // условие, что файл формата .txt
    void validateIsFileOpen() const;        // условие, что файл открыт
    void validateFile() const;              // для полной валидации файла
    void validateFileWriteSuccess() const;  // для обеспечения успешной записи в журнал
    std::string getCurrentTime() const;     // получение текущего времени
    std::string getLogLevelString(LogLevel logLevel) const;  // получение уровня важности (строка)

   public:
    explicit Logger(const std::string& filename, LogLevel logLevel = INFO, LogType logType = SAFELY);

    void log(const std::string& message, LogLevel logLevel = INFO);  // записать сообщение в журнал
    void changeLogLevel(LogLevel newLogLevel);  // поменять уровень важности по умолчанию
    void     changeLogType(LogType newLogType);  // поменять тип записи по умолчанию
    LogLevel getLogLevel() const;                // получение уровня важности
    LogType  getLogType() const;                 // получение типа записи
};