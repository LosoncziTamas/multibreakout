#ifndef Game_hpp
#define Game_hpp

#include <vector>

#include "Ball.hpp"
#include "Paddle.hpp"
#include "Obstacle.hpp"
#include "Enemy.hpp"
#include "Brick.hpp"
#include "TextureButton.hpp"
#include "NinePatch.hpp"

struct World {
    bool initialized;
    
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

struct GameScreen {
    NinePatch leftPanel;
    NinePatch rightPanel;
    TextureButton leftButton;
    TextureButton rightButton;
};

void gamePlayUpdate(GameState& gameState);

#endif
