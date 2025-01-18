#pragma once

#include <vector>
#include <queue>
#include <ctime> // for srand / rand
#include <fstream>
#include <iostream>
#include "position.h"
#include <mutex>

class GameField {
private:
    int ROWS_, COLUMNS_;
    Position GAME_BEGIN_, GAME_END_;
    std::vector<std::vector<char>> field_;
    std::mutex gameFieldMutex_;

    bool isPathExists(Position currentPos, std::queue<Position>& path, std::vector<std::vector<bool>>& visited);
    void generateBlocks();

public:
    GameField(const int ROWS, const int COLUMNS);

    void calculateGameField();
    void display() const;
    void clearScreen() const;
    bool isWalkable(int x, int y) const;
    void clearPlayerPosition(Position position);
    void setPlayerPosition(Position position);
    void recalculateField();
};