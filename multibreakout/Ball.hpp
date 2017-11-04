#ifndef Ball_hpp
#define Ball_hpp

#include <vector>

#include "Vec2.hpp"
#include "Common.hpp"

struct World;
struct GameInput;
struct Paddle;

struct Ball {
    Vec2 velocity;
    Vec2 oldPos;
    Vec2 newPos;
    Vec2 movementDelta;
    PowerUp powerUp;
    float radius;
    float speed;
    Paddle *assignedPaddle;
    int textureIndex;
};

void updateBalls(World& world, GameInput& input, float delta);
void collideBalls(std::vector<Ball>& balls);
void initBall(Ball& ball, std::vector<Ball>& balls, Paddle& paddle);

#endif