#ifndef Brick_hpp
#define Brick_hpp

#include <vector>
#include <SDL2/SDL_pixels.h>

#include "Vec2.hpp"
#include "Ball.hpp"


enum PowerUp {neutral, speedUp, slowDown, strech, shrink};

struct Brick {
    Vec2 center;
    int width;
    int height;
    bool active;
    void (*callback)(Brick& brick, Ball& ball);
    PowerUp powerUp;
};

void initBricks(std::vector<Brick>& bricks);
void collideWithBrick(Ball& ball, Brick& brick);

#endif
