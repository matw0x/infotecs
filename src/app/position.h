#pragma once

struct Position {
    int x, y;

    bool operator == (const Position& other) const noexcept {
        return this->x == other.x && this->y == other.y;
    }
};

#define WALL_CORNER '+'
#define WALL_HORIZONTAL '-'
#define WALL_VERTICAL '|'
#define NOTHING ' '
#define PLAYER '$'
#define BLOCK '#'

const int DIRECTION_SIZE = 4;

const int ROWS = 15, COLUMNS = 45;
const Position GAME_BEGIN = { ROWS / 2, 0 }, GAME_END = { ROWS / 2, COLUMNS - 1 };

const Position UP_POS = { -1, 0 }, 
               DOWN_POS = { 1, 0 }, 
               LEFT_POS = { 0, -1 }, 
               RIGHT_POS = { 0, 1 };
               

enum Choice { PLAY, SETTINGS };
enum Direction { UP, DOWN, LEFT, RIGHT };