#ifndef GameState_hpp
#define GameState_hpp

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
    
    Brick brick;
    Ball ball;
    Paddle paddle;
    
    GameInput input;
};

#endif
