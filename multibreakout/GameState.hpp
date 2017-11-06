#ifndef GameState_hpp
#define GameState_hpp

#include <vector>

#include "Ball.hpp"
#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "Brick.hpp"
#include "Enemy.hpp"
#include "Obstacle.hpp"
#include "Renderer.hpp"
#include "NinePatch.hpp"

enum Screen {
    menu, game
};

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

struct GameState {
    float delta;
    bool paused;
    bool initialized;
    
    SDL_Renderer *renderer;
    FC_Font *font;
    Atlas atlas;
    
    Screen currScreen;
    World world;
    GameInput input;
    
    std::vector<SDL_Texture*> ninePatchTextures;
    NinePatch leftPanel;
    NinePatch rightPanel;
    NinePatch menuPanel;
};

#endif
