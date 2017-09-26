#ifndef Paddle_hpp
#define Paddle_hpp

#include "Vec2.hpp"
#include "Ball.hpp"
#include "MultiBreakout.hpp"

struct Paddle {
    float width;
    float height;
    Vec2 centerPos;
    Vec2 velocity;
    Vec2 movementDelta;
    Vec2 oldPos;
    Vec2 newPos;
};

std::ostream& operator<< (std::ostream& stream, const Paddle& paddle);

void initPaddle(Paddle &paddle);

void updatePaddle(Paddle& paddle, Ball& ball, const GameInput& input, float delta, const SDL_Rect& leftWall, const SDL_Rect& rightWall);

#endif