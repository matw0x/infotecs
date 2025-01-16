#include "manager.h"

MultithreadAppManager::MultithreadAppManager
(const std::string& logFilename, LogLevel logLevel, LogType logType) : 
    gameField_(std::make_unique<GameField>()),
    player_(std::make_unique<Player>(gameField_.get())),
    logger_(std::make_unique<Logger>(logFilename, logLevel, logType)) {}

void MultithreadAppManager::run() {
    std::thread gameThread([this]() { runGameMulti(); });
    std::thread logThread([this]() { logMulti("Some log message", INFO); });

    gameThread.join();
    logThread.join();
}

void MultithreadAppManager::runGameMulti() const {
    player_->letsgo();
}

void MultithreadAppManager::logMulti(const std::string& message, LogLevel logLevel) const {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        logger_->log(message, logLevel);
    }
}

void MultithreadAppManager::writeLog(const std::string& message, LogLevel logLevel) const { logMulti(message, logLevel); }