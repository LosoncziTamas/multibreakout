#ifndef Ball_hpp
#define Ball_hpp

#include "Vec2.hpp"

struct GameInput;

struct Ball {
    Vec2 center;
    Vec2 velocity;
    Vec2 oldPos;
    Vec2 newPos;
    float radius;
};

void initBall(Ball &ball);
void updateBall(Ball& ball, float delta, const GameInput& input);

#endif