#include "player.h"
#include "manager.h"

void Player::processMove(char move) {
    Position newPosition = position_;
    switch (tolower(move)) {
        case 'w':
            if (newPosition.x - 1 >= 0 && gameField_->isWalkable(newPosition.x - 1, newPosition.y)) {
                --newPosition.x;
            }
            break;
        case 's':
            if (newPosition.x + 1 < ROWS && gameField_->isWalkable(newPosition.x + 1, newPosition.y)) {
                ++newPosition.x;
            }
            break;
        case 'a':
            if (newPosition.y - 1 >= 0 && gameField_->isWalkable(newPosition.x, newPosition.y - 1)) {
                --newPosition.y;
            }
            break;
        case 'd':
            if (newPosition.y + 1 < COLUMNS && gameField_->isWalkable(newPosition.x, newPosition.y + 1)) {
                ++newPosition.y;
            }
            break;
        default:
            std::cout << "Unknown movement! Please enter the correct data.\n";
            return;
    }

    gameField_->clearPlayerPosition(position_);
    position_ = std::move(newPosition);
    gameField_->setPlayerPosition(position_);
}

Player::Player(GameField* gameField) : position_(GAME_BEGIN), gameField_(gameField) {}

void Player::printBeforePlay() const {
    app.writeLog("Игрок получил информацию перед началом прохождения!");
    std::cout << "Nice choice!\n";
    std::cout << "Control keys:\n"
            << "\tW - up\n"
            << "\tA - left\n"
            << "\tS - down\n"
            << "\tD - right\n";
    
    std::cout << "These messages will delete!\n";
}

void Player::play() {
    std::this_thread::sleep_for(std::chrono::seconds(3));

    auto begin = std::chrono::high_resolution_clock::now();
    char move;
    while (true) {
        gameField_->clearScreen();
        gameField_->display();

        if (position_ == GAME_END) {
            const auto end = std::chrono::high_resolution_clock::now();
            std::cout << "Congratulations! You've reached the finish!\n";
            gameDuration_ = end - begin;
            std::cout << "Your time: " << gameDuration_.count()<< " seconds!\n";
            break;
        }

        std::cin >> move;
        processMove(move);
    }
}

void Player::readme() const {
    app.writeLog("Игрок получил инструкцию.");
    std::cout << "Welcome in a simple game! Before starting:\n"
                << "[0] - play\n"
                << "[1] - settings\n"; 
}

void Player::handleChoice(short choice) {
    switch (choice) {
        case PLAY:
            app.writeLog("Игрок решил поиграть!");
            printBeforePlay();
            play();
            break;
        case SETTINGS:
            app.writeLog("Игрок зашёл в настройки.");
            std::cout << "SETTINGS IN DEVELOPING\n";
            break;
        default:
            app.writeLog("Игрок выбрал что-то непонятное...", LogLevel::ERROR);
            std::cout << "Unknown choice! Please enter the correct data.\n";
            break;
    }
}

void Player::letsgo() {
    readme();
    short choice;
    std::cin >> choice;

    handleChoice(choice);
}