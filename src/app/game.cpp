#include "game.h"

bool GameField::isPathExists(Position currentPos, std::queue<Position>& path, std::vector<std::vector<bool>>& visited) {
    if (currentPos == GAME_END_) return true;

    const Position directions[DIRECTION_SIZE] = { RIGHT_POS, UP_POS, LEFT_POS, DOWN_POS };

    for (int i = 0; i != DIRECTION_SIZE; ++i) {
        Position next = { currentPos.x + directions[i].x, currentPos.y + directions[i].y };

        auto isInBound = [&]() -> bool {
            return next.x >= 0 && next.x < ROWS_ && next.y >= 0 && next.y < COLUMNS_; 
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
    for (int i = 1; i != ROWS_ - 1; ++i) {
        for (int j = 1; j != COLUMNS_ - 1; ++j) {
            field_[i][j] = BLOCK;
        }
    }

    auto isInBounds = [&](int x, int y) -> bool {
        return x > 0 && x < ROWS_ - 1 && y > 0 && y < COLUMNS_ - 1;
    };

    std::vector<Position> walls;
    int startX = 1 + rand() % (ROWS_ - 2);
    int startY = 1 + rand() % (COLUMNS_ - 2);
    field_[startX][startY] = NOTHING;
    
    // соседи
    auto addWalls = [&](int x, int y) -> void {
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
        int deadEndX = 1 + rand() % (ROWS_ - 2);
        int deadEndY = 1 + rand() % (COLUMNS_ - 2);

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

    if (isInBounds(GAME_BEGIN_.x, GAME_BEGIN_.y + 1)) field_[GAME_BEGIN_.x][GAME_BEGIN_.y + 1] = NOTHING;
    if (isInBounds(GAME_END_.x, GAME_END_.y - 1)) field_[GAME_END_.x][GAME_END_.y - 1] = NOTHING;
}

void GameField::recalculateField() {
    if (ROWS_ != ROWS && COLUMNS_ != COLUMNS) {
        ROWS_ = ROWS;
        COLUMNS_ = COLUMNS;
        GAME_BEGIN_ = GAME_BEGIN;
        GAME_END_ = GAME_END;

        calculateGameField();
    }
}

void GameField::calculateGameField() {
    std::vector<std::vector<char>> field(ROWS_, std::vector<char>(COLUMNS_));
    for (int i = 0; i != ROWS_; ++i) {
        for (int j = 0; j != COLUMNS_; ++j) {
            if (i == 0 || i == ROWS_ - 1) {
                if (j == 0 || j == COLUMNS_ - 1) field[i][j] = WALL_CORNER;
                else field[i][j] = WALL_HORIZONTAL;
            } else {
                if (j == 0 || j == COLUMNS_ - 1) field[i][j] = WALL_VERTICAL;
                else field[i][j] = NOTHING;
            }
        }
    }

    field[GAME_BEGIN_.x][GAME_BEGIN_.y] = PLAYER;
    field[GAME_END_.x][GAME_END_.y] = NOTHING;

    field_ = std::move(field);

    int countGen = 1;
    while (true) {
        std::queue<Position> path;
        path.push(GAME_BEGIN_);

        std::vector<std::vector<bool>> visited(ROWS_, std::vector<bool>(COLUMNS_, false));
        visited[GAME_BEGIN_.x][GAME_BEGIN_.y] = true;

        generateBlocks();

        if (isPathExists(GAME_BEGIN_, path, visited)) break;
        std::cout << "GENERATING #" << countGen++ << "...\n";
    }
}

GameField::GameField(const int ROWS, const int COLUMNS) : 
    ROWS_(ROWS), COLUMNS_(COLUMNS), GAME_BEGIN_({ ROWS / 2, 0 }), GAME_END_({ ROWS / 2, COLUMNS - 1 }) {
    calculateGameField();
}

void GameField::display() const {
    for (int i = 0; i != ROWS_; ++i) {
        for (int j = 0; j != COLUMNS_; ++j) {
            std::cout << field_[i][j] << " ";

            if (i == GAME_END_.x && j == GAME_END_.y) std::cout << "<- FINISH";
        }

        std::cout << "\n";
    }
}

void GameField::clearPlayerPosition(Position position) {
    field_[position.x][position.y] = NOTHING;
}

void GameField::clearScreen() const {
    system("clear");
}

bool GameField::isWalkable(int x, int y) const {
    return field_[x][y] == NOTHING;
}

void GameField::setPlayerPosition(Position position) {
    field_[position.x][position.y] = PLAYER;
}