#ifndef GameState_hpp
#define GameState_hpp

#include <vector>

#include "Ball.hpp"
#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "Brick.hpp"
#include "Enemy.hpp"
#include "Obstacle.hpp"

enum Screen {
    menu, game
};

struct World {
    bool init;
    
    int leftBoundary;
    int rightBoundary;
    
    std::vector<Brick> bricks;
    std::vector<Ball> balls;
    
    Obstacles obstacles;
    Paddle paddle;
    Enemy enemyUpper;
    Enemy enemyLeft;
    Enemy enemyRight;
};

struct GameState {
    float delta;
    bool assetsLoaded;
    bool paused;
    Screen currScreen;
    World world;
    GameInput input;
};

#endif
