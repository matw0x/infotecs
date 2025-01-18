#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "game.h"
#include "position.h"

class Player {
   private:
    GameField*                    gameField_;     // игровое поле
    Position                      position_;      // текущая позиция игрока
    std::chrono::duration<double> gameDuration_;  // время прохождения карты

    void processMove(char move);      // обработка движения игрока
    void play();                      // старт
    void printBeforePlay() const;     // вывод предыгровой информации
    void readme() const;              // инструкция, как играть
    void handleChoice(short choice);  // обработка выбора игрока
    void printWhileMazeGenerating() const;  // вывод информации с ожиданием, пока поток генерации активен

   public:
    Player(GameField* gameField);

    void letsgo();  // публичный старт игры
};