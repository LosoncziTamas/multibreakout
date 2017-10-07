#ifndef Ball_hpp
#define Ball_hpp

#include "Vec2.hpp"
#include "Common.hpp"

struct GameInput;
struct GameState;

struct Ball {
    Vec2 velocity;
    Vec2 oldPos;
    Vec2 newPos;
    Vec2 movementDelta;
    PowerUp powerUp;
    float radius;
    float speed;
};

void initBall(Ball &ball);
void updateBall(GameState& gameState);

#endif