#pragma once

struct Position {
    int x, y;

    bool operator == (const Position& other) const noexcept {
        return this->x == other.x && this->y == other.y;
    }
};

const int ROWS = 15;
const int COLUMNS = 45;

const Position GAME_BEGIN = { ROWS / 2, 0 };
const Position GAME_END = { ROWS / 2, COLUMNS - 1 };

#define WALL_CORNER '+'
#define WALL_HORIZONTAL '-'
#define WALL_VERTICAL '|'
#define NOTHING ' '
#define PLAYER '$'
#define BLOCK '#'

const int DIRECTION_SIZE = 4;

const Position UP_POS = { -1, 0 };
const Position DOWN_POS = { 1, 0 };
const Position LEFT_POS = { 0, -1 };
const Position RIGHT_POS = { 0, 1 };

enum Choice { PLAY, SETTINGS };
enum Direction { UP, DOWN, LEFT, RIGHT };