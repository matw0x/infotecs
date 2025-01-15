#pragma once

#include <vector>
#include <queue>
#include <ctime> // for srand / rand
#include <fstream>
#include <iostream>
#include "position.h"

class GameField {
private:
    std::vector<std::vector<char>> field_;

    bool isPathExists(Position currentPos, std::queue<Position>& path, std::vector<std::vector<bool>>& visited);
    void generateBlocks();

public:
    GameField();

    void display() const;
    void clearScreen() const;
    bool isWalkable(int x, int y) const;
    void clearPlayerPosition(Position position);
    void setPlayerPosition(Position position);
};