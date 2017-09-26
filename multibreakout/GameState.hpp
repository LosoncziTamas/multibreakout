#ifndef GameState_hpp
#define GameState_hpp

#include "Ball.hpp"
#include "Paddle.hpp"
#include "MultiBreakout.hpp"

struct GameState {
    float delta;
    
    SDL_bool init;
    SDL_bool paused;
    
    SDL_Rect leftWall;
    SDL_Rect rightWall;
    
    Ball ball;
    Paddle paddle;
    GameInput input;
};

#endif
