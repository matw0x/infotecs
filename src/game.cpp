#include "game.h"

bool GameField::isPathExists(const Position& currentPos, std::queue<Position>& path, std::vector<std::vector<bool>>& visited) {
    if (currentPos == GAME_END) return true;

    const Position directions[DIRECTION_SIZE] = { RIGHT_POS, UP_POS, LEFT_POS, DOWN_POS };

    for (int i = 0; i != DIRECTION_SIZE; ++i) {
        Position next = { currentPos.x + directions[i].x, currentPos.y + directions[i].y };

        auto isInBound = [&]() -> bool {
            return next.x >= 0 && next.x < ROWS 
            && next.y >= 0 && next.y < COLUMNS; 
        };

        auto isFree = [&]() -> bool {
            return field_[next.x][next.y] == NOTHING;
        };

        if (isInBound() && isFree() && !visited[next.x][next.y]) {
            visited[next.x][next.y] = true;
            path.push(next);

            if (isPathExists(next, path, visited)) return true;

            path.pop();
            visited[currentPos.x][currentPos.y] = false;
        }
    }

    return false;
}

void GameField::generateBlocks() {
    srand(static_cast<unsigned>(time(nullptr)));
    for (size_t i = 1; i != ROWS - 1; ++i) {
        for (size_t j = 1; j != COLUMNS - 1; ++j) {
            field_[i][j] = BLOCK;
        }
    }

    auto isInBounds = [&](const int& x, const int& y) -> bool {
        return x > 0 && x < ROWS - 1 && y > 0 && y < COLUMNS - 1;
    };

    std::vector<Position> walls;
    int startX = 1 + rand() % (ROWS - 2);
    int startY = 1 + rand() % (COLUMNS - 2);
    field_[startX][startY] = NOTHING;
    
    // соседи
    auto addWalls = [&](const int& x, const int& y) -> void {
        if (isInBounds(x - 1, y) && field_[x - 1][y] == BLOCK) walls.push_back({x - 1, y});
        if (isInBounds(x + 1, y) && field_[x + 1][y] == BLOCK) walls.push_back({x + 1, y});
        if (isInBounds(x, y - 1) && field_[x][y - 1] == BLOCK) walls.push_back({x, y - 1});
        if (isInBounds(x, y + 1) && field_[x][y + 1] == BLOCK) walls.push_back({x, y + 1});
    };
    
    addWalls(startX, startY);
    
    while (!walls.empty()) {
        int randomIndex = rand() % walls.size();
        Position wall = walls[randomIndex];
        walls.erase(walls.begin() + randomIndex);
        
        int x = wall.x;
        int y = wall.y;
        
        int adjCount = 0;

        if (isInBounds(x - 1, y) && field_[x - 1][y] == NOTHING) ++adjCount;
        if (isInBounds(x + 1, y) && field_[x + 1][y] == NOTHING) ++adjCount;
        if (isInBounds(x, y - 1) && field_[x][y - 1] == NOTHING) ++adjCount;
        if (isInBounds(x, y + 1) && field_[x][y + 1] == NOTHING) ++adjCount;
        
        if (adjCount <= 1) { // проверяем, что только один пустой сосед
            field_[x][y] = NOTHING;
            addWalls(x, y);
        }
    }

    // MAYBE THIS WILL DELETE.. IDK
    for (int i = 0; i != 3;) {
        int deadEndX = 1 + rand() % (ROWS - 2);
        int deadEndY = 1 + rand() % (COLUMNS - 2);

        if (field_[deadEndX][deadEndY] == NOTHING) {
            ++i;
            int direction = rand() % DIRECTION_SIZE;
            switch (direction) {
                case UP:
                    if (isInBounds(deadEndX - 1, deadEndY)) field_[deadEndX - 1][deadEndY] = BLOCK;
                    break;
                case DOWN:
                    if (isInBounds(deadEndX + 1, deadEndY)) field_[deadEndX + 1][deadEndY] = BLOCK;
                    break;
                case LEFT:
                    if (isInBounds(deadEndX, deadEndY - 1)) field_[deadEndX][deadEndY - 1] = BLOCK;
                    break;
                case RIGHT:
                    if (isInBounds(deadEndX, deadEndY + 1)) field_[deadEndX][deadEndY + 1] = BLOCK;
                    break;
            }
        }
    }

    if (isInBounds(GAME_BEGIN.x, GAME_BEGIN.y + 1)) field_[GAME_BEGIN.x][GAME_BEGIN.y + 1] = NOTHING;
    if (isInBounds(GAME_END.x, GAME_END.y - 1)) field_[GAME_END.x][GAME_END.y - 1] = NOTHING;
}


GameField::GameField() {
    std::vector<std::vector<char>> field(ROWS, std::vector<char>(COLUMNS));
    for (int i = 0; i != ROWS; ++i) {
        for (int j = 0; j != COLUMNS; ++j) {
            if (i == 0 || i == ROWS - 1) {
                if (j == 0 || j == COLUMNS - 1) field[i][j] = WALL_CORNER;
                else field[i][j] = WALL_HORIZONTAL;
            } else {
                if (j == 0 || j == COLUMNS - 1) field[i][j] = WALL_VERTICAL;
                else field[i][j] = NOTHING;
            }
        }
    }

    field[GAME_BEGIN.x][GAME_BEGIN.y] = PLAYER;
    field[GAME_END.x][GAME_END.y] = NOTHING;

    field_ = std::move(field);

    int countGen = 1;
    while (true) {
        std::queue<Position> path;
        path.push(GAME_BEGIN);

        std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLUMNS, false));
        visited[GAME_BEGIN.x][GAME_BEGIN.y] = true;

        generateBlocks();

        if (isPathExists(GAME_BEGIN, path, visited)) break;
        std::cout << "GENERATING #" << countGen++ << "...\n";
    }
}

void GameField::display() const {
    for (int i = 0; i != ROWS; ++i) {
        for (int j = 0; j != COLUMNS; ++j) {
            std::cout << field_[i][j] << " ";

            if (i == GAME_END.x && j == GAME_END.y) std::cout << "<- FINISH";
        }

        std::cout << "\n";
    }
}

void GameField::clearPlayerPosition(const Position& position) {
    field_[position.x][position.y] = NOTHING;
}

void GameField::clearScreen() const {
    auto res = system("clear");
    if (res) return;
}

bool GameField::isWalkable(const int& x, const int& y) const {
    return field_[x][y] == NOTHING;
}

void GameField::setPlayerPosition(const Position& position) {
    field_[position.x][position.y] = PLAYER;
}