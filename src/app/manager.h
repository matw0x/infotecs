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

public:
    MultithreadAppManager(const std::string& logFilename = "game_log.txt", LogLevel logLevel = INFO, LogType logType = SAFELY);
    void run();
    Logger* getLogger() const;
};

extern MultithreadAppManager app;