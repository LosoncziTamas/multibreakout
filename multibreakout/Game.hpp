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

enum GameType {
    oneVsOne, twoVsTwo
};

struct World {
    GameType type;
    bool initialized;
    
    int leftBoundary;
    int rightBoundary;
    
    std::vector<Brick> bricks;
    std::vector<Ball> balls;
    
    Paddle paddle;
    Enemy enemyUpper;
    
    Obstacles obstacles;
    Enemy enemyLeft;
    Enemy enemyRight;
};

struct GameUi {
    bool initialized;
    NinePatch leftPanel;
    NinePatch rightPanel;
    TextureButton leftButton;
    TextureButton rightButton;
};

void gamePlayUpdate(GameState& gameState);
void initalizeGameWorld(World& world);
void initializeUi(GameUi& gameUi);

#endif
