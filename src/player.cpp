#include "player.h"

void Player::play(const std::function<void()>& anyPlay) {
    std::cout << "Nice choice!\n";
    std::cout << "Control keys:\n"
            << "\tW - up\n"
            << "\tA - left\n"
            << "\tS - down\n"
            << "\tD - right\n";
    
    std::cout << "These messages will delete!\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    anyPlay();
}

void Player::processMove(const char& move) {
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

void Player::playGameLoopWithoutDB() {
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

void Player::playGameLoopWithDB() {
    playGameLoopWithoutDB();

    //auto time = gameDuration_.count();
    
}

void Player::readme() {
    std::cout << "Welcome in a simple game! Before starting:\n"
                << "[0] - play with authorization\n"
                << "[1] - play without authorization (your progress will not be save!)\n"
                << "[2] - check the leaderboard\n"
                << "[3] - settings\n"; 
}

bool Player::handleChoice(const short& choice) {
    switch (choice) {
        case PLAY_WITH_DB:
            std::cout << "DATABASE IN DEVELOPING\n";
            break;
        case PLAY_WITHOUT_DB:
            play([this]() -> void {
                this->playGameLoopWithoutDB(); 
            });
            break;
        case LEADERBOARD:
            std::cout << "LEADERBOARD IN DEVELOPING\n";
            break;
        case SETTINGS:
            std::cout << "SETTINGS IN DEVELOPING\n";
            break;
        default:
            std::cout << "Unknown choice! Please enter the correct data.\n";
            return false;
    }

    return true;
}