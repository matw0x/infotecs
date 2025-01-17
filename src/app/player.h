#pragma once

#include <string>
#include <iostream>
#include <thread> //  for sleep_for
#include <chrono> // for seconds
#include "position.h"
#include "game.h"
#include <mutex>

class Player {
private:
    GameField* gameField_;
    Position position_;
    std::chrono::duration<double> gameDuration_;
    std::mutex playerMutex_;

    void processMove(char move);
    void play();
    void printBeforePlay() const;
    void readme() const;
    void handleChoice(short choice);
    void settings();

public:
    Player(GameField* gameField);

    void letsgo();
};