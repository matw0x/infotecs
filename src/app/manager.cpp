#include "manager.h"
#include <vector>

MultithreadAppManager::MultithreadAppManager
(const std::string& logFilename, LogLevel logLevel, LogType logType) : 
    gameField_(std::make_unique<GameField>()),
    player_(std::make_unique<Player>(gameField_.get())),
    logger_(std::make_unique<Logger>(logFilename, logLevel, logType)),
    logThreadRunning_(true)
    {}

void MultithreadAppManager::run() {
    std::thread gameThread([this]() { runGameMulti(); });
    std::thread logThread([this]() { logMulti(); });

    gameThread.join();
    stopLogging();
    logThread.join();
}

void MultithreadAppManager::runGameMulti() const {
    player_->letsgo();
}

void MultithreadAppManager::logMulti() {
    while (logThreadRunning_.load() || !logQueue_.empty()) {
        std::unique_lock<std::mutex> lock(logQueueMutex_);
        
        logCondVar_.wait_for(lock, std::chrono::milliseconds(100), [this]() { return !logQueue_.empty() || !logThreadRunning_.load(); });
        
        while (!logQueue_.empty()) {
            const auto& logMessage = logQueue_.front();
            logger_->log(logMessage.first, logMessage.second);
            logQueue_.pop();
        }

        lock.unlock();
    }
}

void MultithreadAppManager::writeLog(const std::string& message, LogLevel logLevel) {
    logQueue_.push({message, logLevel});
}

void MultithreadAppManager::stopLogging() {
    logThreadRunning_.store(false);
    logCondVar_.notify_all();
}