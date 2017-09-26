#ifndef Ball_hpp
#define Ball_hpp

#include "Vec2.hpp"

struct Ball {
    Vec2 center;
    Vec2 velocity;
    Vec2 oldPos;
    Vec2 newPos;
    float radius;
};

void initBall(Ball &ball);
void update(Ball& ball, float delta);

#endif