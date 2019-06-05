#include "game.h"

Game::Game()
{

}
void Game::init(int length, int width, int booms) {
    this->length = length;
    this->width = width;
    this->booms = booms;

    std::random_device random_device;
    std::default_random_engine random_engine = std::default_random_engine(random_device());
    std::uniform_int_distribution<int> random(1, (length * width));


    int Mines[length+2][width+2];
    std::vector<int> booms_location;
    for(int i = 0; i < booms; i++) {
        booms_location.push_back(random(random_engine));
    }
    for(int i = 0; i < booms; i++) {

    }
}
