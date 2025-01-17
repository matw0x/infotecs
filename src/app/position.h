#pragma once

struct Position {
    int x, y;

    bool operator == (const Position& other) const noexcept {
        return this->x == other.x && this->y == other.y;
    }
};

extern int ROWS, COLUMNS;

#define WALL_CORNER '+'
#define WALL_HORIZONTAL '-'
#define WALL_VERTICAL '|'
#define NOTHING ' '
#define PLAYER '$'
#define BLOCK '#'

const int DIRECTION_SIZE = 4;

extern int ROWS, COLUMNS;
extern Position GAME_BEGIN, GAME_END;

const Position UP_POS = { -1, 0 }, 
               DOWN_POS = { 1, 0 }, 
               LEFT_POS = { 0, -1 }, 
               RIGHT_POS = { 0, 1 };
               

enum Choice { PLAY, SETTINGS };
enum Direction { UP, DOWN, LEFT, RIGHT };
enum Difficulty { EASY, MEDIUM, HARD, FUNNY };