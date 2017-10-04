#ifndef Paddle_hpp
#define Paddle_hpp

#include "Vec2.hpp"
#include "Ball.hpp"
#include "MultiBreakout.hpp"

struct GameState;

struct Paddle {
    float width;
    float height;
    float speed;
    Vec2 velocity;
    Vec2 movementDelta;
    Vec2 oldPos;
    Vec2 newPos;
};

void initPaddle(Paddle &paddle);
void updatePaddle(GameState& gameState);
void resolveCollision(Ball& ball, Paddle& paddle, const Renderer& renderer, float delta);

#endif