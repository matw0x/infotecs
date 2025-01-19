#include "player.h"

#include "manager.h"

LogLevel parseLogLevelStringWithDefault(const std::string& logLevel, LogLevel defaultForLog) {
    if (logLevel == "INFO")
        return INFO;
    else if (logLevel == "WARNING")
        return WARNING;
    else if (logLevel == "ERROR")
        return ERROR;
    else
        return defaultForLog;
}

void Player::processMove(char move, const std::string& logLevel) {
    // обрабатываем движение: смотрим, можно ли ходить игроку
    app->writeLog("Player::processMove | data = " + std::string(1, move),
                  parseLogLevelStringWithDefault(logLevel, INFO));
    Position newPosition    = position_;
    bool     moveSuccessful = false;

    switch (tolower(move)) {
        case 'w':
            if (newPosition.x - 1 >= 0 && gameField_->isWalkable(newPosition.x - 1, newPosition.y)) {
                --newPosition.x;
                moveSuccessful = true;
                app->writeLog("Player::processMove | moving up. New coordinates: " + std::to_string(newPosition.y) +
                                  ';' + std::to_string(newPosition.x),
                              parseLogLevelStringWithDefault(logLevel, INFO));
            } else {
                app->writeLog("Player::processMove | failed to move up.",
                              parseLogLevelStringWithDefault(logLevel, WARNING));
            }
            break;
        case 's':
            if (newPosition.x + 1 < ROWS && gameField_->isWalkable(newPosition.x + 1, newPosition.y)) {
                ++newPosition.x;
                moveSuccessful = true;
                app->writeLog("Player::processMove | moving down. New coordinates: " + std::to_string(newPosition.y) +
                                  ';' + std::to_string(newPosition.x),
                              parseLogLevelStringWithDefault(logLevel, INFO));
            } else {
                app->writeLog("Player::processMove | failed to move down.",
                              parseLogLevelStringWithDefault(logLevel, WARNING));
            }
            break;
        case 'a':
            if (newPosition.y - 1 >= 0 && gameField_->isWalkable(newPosition.x, newPosition.y - 1)) {
                --newPosition.y;
                moveSuccessful = true;
                app->writeLog("Player::processMove | moving left. New coordinates: " + std::to_string(newPosition.y) +
                                  ';' + std::to_string(newPosition.x),
                              parseLogLevelStringWithDefault(logLevel, INFO));
            } else {
                app->writeLog("Player::processMove | failed to move left.",
                              parseLogLevelStringWithDefault(logLevel, WARNING));
            }
            break;
        case 'd':
            if (newPosition.y + 1 < COLUMNS && gameField_->isWalkable(newPosition.x, newPosition.y + 1)) {
                ++newPosition.y;
                moveSuccessful = true;
                app->writeLog("Player::processMove | moving right. New coordinates: " + std::to_string(newPosition.y) +
                                  ';' + std::to_string(newPosition.x),
                              parseLogLevelStringWithDefault(logLevel, INFO));
            } else {
                app->writeLog("Player::processMove | failed to move right.",
                              parseLogLevelStringWithDefault(logLevel, WARNING));
            }
            break;
        case '1':
            printAboutChangingDLL();
            processDLL();
            break;
        default:
            std::cout << "Unknown movement! Please enter the correct data.\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            app->writeLog("Player::processMove | incorrect data = " + std::to_string(move),
                          parseLogLevelStringWithDefault(logLevel, ERROR));
            return;
    }

    // если игрок все же сходил, то меняем его позицию, но сначала очистив
    if (moveSuccessful) {
        gameField_->clearPlayerPosition(position_);
        position_ = newPosition;
        gameField_->setPlayerPosition(position_);
    }
}

Player::Player(GameField* gameField) : gameField_(gameField), position_(GAME_BEGIN) {}

void Player::printBeforePlay() const {
    app->writeLog("Player::printBeforePlay | received information before starting.");
    std::cout << "Control keys:\n"
              << "\tW - up\n"
              << "\tA - left\n"
              << "\tS - down\n"
              << "\tD - right\n";
    std::cout << "[1] - change default log level\n";
    std::cout << "These messages will disappear!\n";
}

void Player::printWhileMazeGenerating() const {
    // вывод крутящегося спиннера, чтобы пользователь не скучал, если поток генерации запаздывает
    const std::string spinner = "/-\\|";
    int               idx     = 0;

    while (!app->isMazeGenerated()) {  // атомарная переменная в помощь
        std::cout << "\rGenerating maze, please wait..." << spinner[idx++ % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    std::cout << std::endl << "Maze is ready! You can start the game now." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void Player::play() {
    // проверяем, достиг ли игрок финиша
    // высчитываем его время прохождения
    // также обрабатываем введенные клавиши необычным образом
    // это сделано ради работы тестов
    printWhileMazeGenerating();

    auto        begin = std::chrono::high_resolution_clock::now();
    char        move;
    std::string userInput;
    while (true) {
        gameField_->clearScreen();
        gameField_->display();

        if (position_ == GAME_END) {
            const auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Congratulations! You've reached the finish!\n";
            gameDuration_ = end - begin;
            std::cout << "Your time: " << gameDuration_.count() << " seconds!\n";

            app->writeLog("Player::play | finished the game. Time = " + std::to_string(gameDuration_.count()) +
                          " seconds.");
            break;
        }

        if (std::cin.eof()) break;

        std::getline(std::cin, userInput);
        if (userInput.empty()) continue;

        move                    = userInput[0];
        std::string logLevelStr = (userInput.size() > 2) ? userInput.substr(2) : "ANY";

        if (!std::cin.fail()) {
            processMove(move, logLevelStr);
        } else {
            break;
        }

        if (std::cin.eof()) break;  // чтобы не дублировался последний символ
    }
}

void Player::readme() const {
    app->writeLog("Player::readme | received instructions.");
    std::cout << "Welcome in a simple game! Before starting:\n"
              << "[0] - play\n"
              << "[1] - change default log level and play (also can do this while running)\n";
}

void Player::printAboutChangingDLL() const {
    app->writeLog("Player::printAboutChangingDLL | received instructions.");
    std::cout << "Just select default log level, which you want:\n";

    std::string INFO = "[0] - INFO", WARNING = "[1] - WARNING", ERROR = "[2] - ERROR";

    if (app->logger_->getLogLevel() == LogLevel::INFO)
        INFO += " <= current";
    else if (app->logger_->getLogLevel() == LogLevel::WARNING)
        WARNING += " <= current";
    else
        ERROR += " <= current";

    std::cout << INFO << std::endl << WARNING << std::endl << ERROR << std::endl;
}

void Player::processDLL() const {
    short level;
    std::cin >> level;

    if (level > LogLevel::ERROR || level < LogLevel::INFO) {
        std::cout << "Not accepted!\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return;
    }

    app->logger_->changeLogLevel(static_cast<LogLevel>(level));
    app->writeLog("Player::processDLL | changed default log level!");
    std::cout << "Settings saved. Go play!\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Player::handleChoice(short choice) {
    // тут обрабатываем выбор игрока
    app->writeLog("Player::handleChoice | data = " + std::to_string(choice));
    switch (choice) {
        case PLAY:
            app->writeLog("Player::handleChoice | decided to play!");
            printBeforePlay();
            play();
            break;
        case CHANGE_DLL:
            app->writeLog("Player::handleChoice | open menu to change default log level.");
            printAboutChangingDLL();
            processDLL();
            handleChoice(Choice::PLAY);
            break;
        default:
            app->writeLog("Player::handleChoice | selected something unclear...", LogLevel::ERROR);
            std::cout << "Unknown choice! Please enter the correct data.\n";
            break;
    }
}

void Player::letsgo() {
    // поехали!
    readme();
    short choice;
    std::cin >> choice;

    handleChoice(choice);
}