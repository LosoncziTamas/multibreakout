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
    Vec2 centerPos;
    Vec2 velocity;
    Vec2 movementDelta;
    Vec2 oldPos;
    Vec2 newPos;
};

void initPaddle(Paddle &paddle);
void updatePaddle(GameState& gameState, const Renderer& renderer);
void collide(GameState& gameState, const Renderer& renderer);
void debugCollision(GameState& gameState, const Renderer& renderer);

#endif