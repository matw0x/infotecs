#pragma once

#include <string>
#include <iostream>
#include <thread> //  for sleep_for
#include <chrono> // for seconds
#include <functional> // лямбда-функции в связке с коллбеками
#include "position.h"
#include "game.h"

class Player {
private:
    Position position_;
    GameField* gameField_;
    std::chrono::duration<double> gameDuration_;

    void processMove(char move);
    void play();
    void printBeforePlay() const;

public:
    Player(GameField* gameField);

    void readme() const;
    bool handleChoice(short choice);
};