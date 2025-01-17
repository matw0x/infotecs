#include "player.h"
#include "manager.h"
#include <numeric>
#include <mutex>
#include <condition_variable>

void Player::processMove(char move) {
    app->writeLog("Player::processMove | data = " + std::string(1, move));
    Position newPosition = position_;
    bool moveSuccessful = false;

    switch (tolower(move)) {
        case 'w':
            if (newPosition.x - 1 >= 0 && gameField_->isWalkable(newPosition.x - 1, newPosition.y)) {
                --newPosition.x;
                moveSuccessful = true;
                app->writeLog("Player::processMove | moving up. New coordinates: " + std::to_string(newPosition.y) + ';' + std::to_string(newPosition.x));
            } else {
                app->writeLog("Player::processMove | failed to move up.");
            }
            break;
        case 's':
            if (newPosition.x + 1 < ROWS && gameField_->isWalkable(newPosition.x + 1, newPosition.y)) {
                ++newPosition.x;
                moveSuccessful = true;
                app->writeLog("Player::processMove | moving down. New coordinates: " + std::to_string(newPosition.y) + ';' + std::to_string(newPosition.x));
            } else {
                app->writeLog("Player::processMove | failed to move down.");
            }
            break;
        case 'a':
            if (newPosition.y - 1 >= 0 && gameField_->isWalkable(newPosition.x, newPosition.y - 1)) {
                --newPosition.y;
                moveSuccessful = true;
                app->writeLog("Player::processMove | moving left. New coordinates: " + std::to_string(newPosition.y) + ';' + std::to_string(newPosition.x));
            } else {
                app->writeLog("Player::processMove | failed to move left.");
            }
            break;
        case 'd':
            if (newPosition.y + 1 < COLUMNS && gameField_->isWalkable(newPosition.x, newPosition.y + 1)) {
                ++newPosition.y;
                moveSuccessful = true;
                app->writeLog("Player::processMove | moving right. New coordinates: " + std::to_string(newPosition.y) + ';' + std::to_string(newPosition.x));
            } else {
                app->writeLog("Player::processMove | failed to move right.");
            }
            break;
        default:
            std::cout << "Unknown movement! Please enter the correct data.\n";
            app->writeLog("Player::processMove | incorrect data.", LogLevel::WARNING);
            return;
    }

    if (moveSuccessful) {
        gameField_->clearPlayerPosition(position_);
        position_ = newPosition;
        gameField_->setPlayerPosition(position_);
    }
}

Player::Player(GameField* gameField) : position_(GAME_BEGIN), gameField_(gameField) {}

void Player::printBeforePlay() const {
    app->writeLog("Player::printBeforePlay | received information before starting.");
    std::cout << "Nice choice!\n";
    std::cout << "Control keys:\n"
            << "\tW - up\n"
            << "\tA - left\n"
            << "\tS - down\n"
            << "\tD - right\n";
    
    std::cout << "These messages will disappear!\n";
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
            std::cout << "Your time: " << gameDuration_.count() << " seconds!\n";
            
            app->writeLog("Player::play | finished the game. Time = " + 
                            std::to_string(gameDuration_.count()) + " seconds.");
            break;
        }

        std::cin >> move;
        processMove(move);
    }
}

void Player::readme() const {
    app->writeLog("Player::readme | received instructions.");
    std::cout << "Welcome in a simple game! Before starting:\n"
                << "[0] - play\n"
                << "[1] - settings\n"; 
}

void Player::handleChoice(short choice) {
    app->writeLog("Player::handleChoice | data = " + std::to_string(choice));

    switch (choice) {
        case PLAY:
            app->writeLog("Player::handleChoice | decided to play!");
            printBeforePlay();
            play();
            break;
        case SETTINGS:
            app->writeLog("Player::handleChoice | entered settings.");
            std::cout << "SETTINGS IN DEVELOPMENT\n";
            break;
        default:
            app->writeLog("Player::handleChoice | selected something unclear...", LogLevel::ERROR);
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