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
    std::string login_; 
    Position position_;
    GameField* gameField_;
    std::chrono::duration<double> gameDuration_;

    void play(const std::function<void()>& anyPlay); // anyPlay - с БД или без
    void processMove(const char& move);
    void playGameLoopWithoutDB();
    void playGameLoopWithDB();

public:
    Player(GameField* gameField);

    void readme();
    bool handleChoice(const short& choice);
};