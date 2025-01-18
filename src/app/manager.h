#pragma once

#include "game.h"
#include "player.h"
#include <logger/logger.h>
#include <thread>
#include <memory>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <utility>

class MultithreadAppManager {
private:
    std::unique_ptr<GameField> gameField_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<Logger> logger_;

    std::thread mazeGenerateThread_, gameThread_, logThread_;

    std::atomic<bool> logThreadRunning_, mazeGeneratedThread_;
    std::queue<std::pair<std::string, LogLevel>> logQueue_;
    std::condition_variable logCondVar_;
    std::mutex logQueueMutex_;

    void runGameMulti() const;
    void logMulti();
    void stopLogging();
    void runMazeGenMulti() const;

public:
    MultithreadAppManager(const std::string& logFilename = "game_log.txt", 
    LogLevel logLevel = INFO, LogType logType = SAFELY);

    void writeLog(const std::string& message, LogLevel logLevel = INFO);
    void run();
    
    void stopMazeGenerated(); // для генерации лабиринта
    bool isMazeGenerated() const;
    void resetMazeGenThread();
};

extern std::unique_ptr<MultithreadAppManager> app;