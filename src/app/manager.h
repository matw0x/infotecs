#pragma once

#include <logger/logger.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include "game.h"
#include "player.h"

class MultithreadAppManager {
   private:
    std::unique_ptr<GameField> gameField_;  // игровое поле
    std::unique_ptr<Player>    player_;     // игрок
    std::unique_ptr<Logger>    logger_;     // библиотека

    std::thread mazeGenerateThread_, gameThread_,
        logThread_;  // потоки: для генерации лабиринта, игровой, запись в журнал
    std::atomic<bool> logThreadRunning_,
        mazeGeneratedThread_;  // для обеспечения потокобезопасности и остановки потоков
    std::queue<std::pair<std::string, LogLevel>> logQueue_;  // для отправки сообщений
    std::condition_variable logCondVar_;     // для обеспечения потокобезопасности
    std::mutex              logQueueMutex_;  // для обеспечения потокобезопасности

    void runGameMulti() const;  // запуск игрового потока
    void logMulti();            // запуск потока записи в журнал
    void stopLogging();         // остановка потока записи в журнал
    void runMazeGenMulti();     // запуск потока генерации лабиринта
    void stopMazeGenerated();   // для остановки потока генерации лабиринта

   public:
    MultithreadAppManager(const std::string& logFilename = "game_log.txt", LogLevel logLevel = INFO,
                          LogType logType = SAFELY);

    void writeLog(const std::string& message, LogLevel logLevel = INFO);  // записать сообщение в журнал
    void run();                                                           // запуск приложения
    bool isMazeGenerated() const;  // для отслеживания работы потока генерации лабиринта
};

extern std::unique_ptr<MultithreadAppManager> app;  // само приложение
// оно сделано внешним и глобальным, чтобы использовать во всех классах