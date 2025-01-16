#include "manager.h"

MultithreadAppManager::MultithreadAppManager
(const std::string& logFilename, LogLevel logLevel, LogType logType) : 
    gameField_(std::make_unique<GameField>()),
    player_(std::make_unique<Player>(gameField_.get())),
    logger_(std::make_unique<Logger>(logFilename, logLevel, logType)) {}

void MultithreadAppManager::run() {
    std::thread gameThread(&MultithreadAppManager::runMulti, this);
    std::thread logThread(&MultithreadAppManager::logMulti, this);

    gameThread.join();
    logThread.join();
}

void MultithreadAppManager::runMulti() const {
    
}

void MultithreadAppManager::logMulti(const std::string& message) const {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        logger_->log(message, logger_->getLogLevel());
    }
}

void MultithreadAppManager::writeLog(const std::string& message) const { logMulti(message); }