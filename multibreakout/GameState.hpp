#ifndef GameState_hpp
#define GameState_hpp

#include <vector>

#include "Ball.hpp"
#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "Brick.hpp"
#include "Enemy.hpp"

struct GameState {
    float delta;
    int leftBoundary;
    int rightBoundary;
    
    bool init;
    bool paused;
    
    std::vector<Brick> bricks;
    std::vector<Ball> balls;

    Paddle paddle;

    Enemy enemyUpper;
    Enemy enemyLeft;
    Enemy enemyRight;

    GameInput input;
};

#endif
