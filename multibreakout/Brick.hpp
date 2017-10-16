#ifndef Brick_hpp
#define Brick_hpp

#include <vector>
#include <SDL2/SDL_pixels.h>

#include "Vec2.hpp"
#include "Ball.hpp"
#include "Common.hpp"

struct Brick {
    Vec2 center;
    int width;
    int height;
    bool active;
    void (*callback)(Brick& brick, Ball& ball);
    PowerUp powerUp;
    int textureIndex;
};

void initBricks(std::vector<Brick>& bricks);
void collideWithBrick(std::vector<Ball>& balls, std::vector<Brick>& bricks);

#endif
