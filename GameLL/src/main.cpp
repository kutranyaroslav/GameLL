#include "Game.h"

int main()
{
    Game game;
    while (!game.getWindow()->IsDone()) {
        game.Update();
        game.Render();
        game.LateUpdate();
    }

    return 0;
}