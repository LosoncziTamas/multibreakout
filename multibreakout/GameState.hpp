#ifndef GameState_hpp
#define GameState_hpp

#include <vector>

#include "Ball.hpp"
#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "Brick.hpp"

struct GameState {
    float delta;
    int leftBoundary;
    int rightBoundary;
    
    SDL_bool init;
    SDL_bool paused;
    
    std::vector<Brick> bricks;    
    Ball ball;
    Paddle paddle;
    Paddle enemy;
    
    GameInput input;
};

#endif
