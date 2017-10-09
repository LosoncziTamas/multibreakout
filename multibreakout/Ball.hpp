#ifndef Ball_hpp
#define Ball_hpp

#include <vector>

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

void updateBalls(GameState& gameState);
void collideBalls(std::vector<Ball>& balls);

#endif