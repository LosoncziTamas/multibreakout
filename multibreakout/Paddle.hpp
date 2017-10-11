#ifndef Paddle_hpp
#define Paddle_hpp

#include "Vec2.hpp"
#include "Ball.hpp"
#include "MultiBreakout.hpp"

struct GameState;

enum Orientation {upper, lower};

struct Paddle {
    float width;
    float height;
    float speed;
    Vec2 velocity;
    Vec2 movementDelta;
    Vec2 oldPos;
    Vec2 newPos;
    Orientation orientation;
};

void initEnemy(Paddle& enemy);
void updateEnemy(Paddle& enemy, std::vector<Ball>& balls, float delta, float leftBoundary, float rightBoundary);
void initPaddle(Paddle &paddle);
void updatePaddle(GameState& gameState);
void resolveCollision(std::vector<Ball>& balls, Paddle& paddle, float delta);

#endif