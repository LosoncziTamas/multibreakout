#ifndef Brick_hpp
#define Brick_hpp

#include <vector>

#include "Vec2.hpp"
#include "Ball.hpp"

struct Brick {
    Vec2 center;
    int width;
    int height;
    bool active;
};

void initBricks(std::vector<Brick>& bricks);
void collideWithBrick(Ball& ball, Brick& brick);

#endif
