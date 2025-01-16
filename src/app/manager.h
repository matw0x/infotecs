#pragma once

#include "game.h"
#include "player.h"
#include <logger/logger.h>
#include <thread>
#include <memory>

class MultithreadAppManager {
private:
    std::unique_ptr<GameField> gameField_;
    std::unique_ptr<Player> player_;
    std::unique_ptr<Logger> logger_;

    void runGameMulti() const;
    void logMulti(const std::string& message, LogLevel logLevel = INFO) const;

public:
    MultithreadAppManager(const std::string& logFilename = "game_log.txt", 
    LogLevel logLevel = INFO, LogType logType = SAFELY);

    void writeLog(const std::string& message, LogLevel logLevel = INFO) const;
    void run();
};

extern MultithreadAppManager app;