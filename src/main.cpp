#include <iostream>
#include "game.h"
#include "player.h"

int main() {
    GameField* pGameField = new GameField();
    Player* pPlayer = new Player(pGameField);

    pPlayer->readme();
    short input; std::cin >> input;

    if (pPlayer->handleChoice(input)) {

    }

    delete pGameField;
    delete pPlayer;

    return 0;
}