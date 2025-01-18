#pragma once

#include <ctime>
#include <iostream>
#include <queue>
#include <vector>

#include "position.h"

class GameField {
   private:
    int                            ROWS_, COLUMNS_;         // размеры игрового поля
    Position                       GAME_BEGIN_, GAME_END_;  // стартовая позиция и финиш
    std::vector<std::vector<char>> field_;                  // игровое поле

    bool isPathExists(Position currentPos, std::queue<Position>& path,
                      std::vector<std::vector<bool>>& visited);  // хотя бы один путь существует
    void generateBlocks();  // генерация блоков в игровом поле

   public:
    GameField(const int _ROWS, const int _COLUMNS);

    void calculateGameField();  // основной метод для генерации игрового поля
    void display() const;       // вывод всего поля в консоль
    void clearScreen() const;   // очистка консоли
    bool isWalkable(int x, int y) const;          // можно ли сходить в эту позицию
    void clearPlayerPosition(Position position);  // очистка позиции игрока
    void setPlayerPosition(Position position);    // установка позиции игрока
};