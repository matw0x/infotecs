#include "manager.h"

MultithreadAppManager::MultithreadAppManager(const std::string& logFilename, LogLevel logLevel, LogType logType)
    : logger_(std::make_unique<Logger>(logFilename, logLevel, logType)),
      gameField_(std::make_unique<GameField>(ROWS, COLUMNS)),
      player_(std::make_unique<Player>(gameField_.get())),
      logThreadRunning_(true),
      mazeGeneratedThread_(false) {}

void MultithreadAppManager::run() {
    // инициализация потоков через лямбды-функции
    mazeGenerateThread_ = std::thread([this]() { runMazeGenMulti(); });
    gameThread_         = std::thread([this]() { runGameMulti(); });
    logThread_          = std::thread([this]() { logMulti(); });

    // ожидание завершения
    gameThread_.join();
    stopLogging();
    logThread_.join();
    mazeGenerateThread_.join();
}

void MultithreadAppManager::stopMazeGenerated() {
    mazeGeneratedThread_.store(true);  // поток сгенерирован, поэтому работаем с атомарной переменной
}

void MultithreadAppManager::runMazeGenMulti() {
    // генерируем лабиринт и отправляем поток поспать
    app->writeLog("APP | START THREAD runMazeGenMulti");
    gameField_->calculateGameField();
    app->writeLog("APP | END THREAD runMazeGenMulti");
    stopMazeGenerated();
}

void MultithreadAppManager::runGameMulti() const {
    // запускаем игровой поток через точку входа в игровую логику
    app->writeLog("APP | START THREAD runGameMulti");
    player_->letsgo();
    app->writeLog("APP | END THREAD runGameMulti");
}

void MultithreadAppManager::logMulti() {
    // работаем с записью сообщений в журнал через очередь, а также условную переменную
    // еще используем мьютекс для очереди
    app->writeLog("APP | START THREAD logMulti");
    while (logThreadRunning_.load() || !logQueue_.empty()) {
        std::unique_lock<std::mutex> lock(logQueueMutex_);

        logCondVar_.wait_for(lock, std::chrono::milliseconds(100),
                             [this]() { return !logQueue_.empty() || !logThreadRunning_.load(); });

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
    // через атомарную переменную завершаем поток записи в журнал
    // уведомляем всем, что закончили
    app->writeLog("APP | END THREAD logMulti");
    logThreadRunning_.store(false);
    logCondVar_.notify_all();
}

bool MultithreadAppManager::isMazeGenerated() const { return mazeGeneratedThread_.load(); }