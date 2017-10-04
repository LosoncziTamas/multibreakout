#ifndef Ball_hpp
#define Ball_hpp

#include "Vec2.hpp"

struct GameInput;

struct Ball {
    Vec2 velocity;
    Vec2 oldPos;
    Vec2 newPos;
    Vec2 movementDelta;
    float radius;
    float speed;
};

void initBall(Ball &ball);
void updateBall(Ball& ball, float delta, const GameInput& input);

#endif