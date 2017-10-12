#ifndef Paddle_hpp
#define Paddle_hpp

#include "Vec2.hpp"
#include "Ball.hpp"
#include "MultiBreakout.hpp"

struct GameState;

enum Orientation {upper, lower};
enum State {none, steering, defending};

struct Brain {
    State state;
    Vec2 steeringPos;
};

const int INVALID_INDEX = -1;

struct Paddle {
    float width;
    float height;
    float speed;
    int ballIndex;
    Vec2 velocity;
    Vec2 movementDelta;
    Vec2 oldPos;
    Vec2 newPos;
    Orientation orientation;
    Brain brain;
};

void initEnemy(Paddle& enemy);
void updateEnemy(Paddle& enemy, std::vector<Ball>& balls, float delta, float leftBoundary, float rightBoundary);
void initPaddle(Paddle &paddle);
void updatePaddle(GameState& gameState);
void resolveCollision(std::vector<Ball>& balls, Paddle& paddle, float delta);

#endif