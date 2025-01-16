#include "manager.h"

MultithreadAppManager::MultithreadAppManager
(const std::string& logFilename, LogLevel logLevel, LogType logType) : 
    gameField_(std::make_unique<GameField>()),
    player_(std::make_unique<Player>(gameField_.get())),
    logger_(std::make_unique<Logger>(logFilename, logLevel, logType)) {}

Logger* MultithreadAppManager::getLogger() const { return logger_.get(); }