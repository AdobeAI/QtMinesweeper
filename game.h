#ifndef GAME_H
#define GAME_H

#include <vector>
#include <random>
#include "cell.h"
class Game
{
public:
    Game();
    void init(int length, int width, int booms);


private:
    int length, width, booms;
};

#endif // GAME_H
