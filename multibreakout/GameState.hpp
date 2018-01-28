#ifndef GameState_hpp
#define GameState_hpp

#include <vector>

#include "Game.hpp"
#include "MultiBreakout.hpp"
#include "Renderer.hpp"
#include "NinePatch.hpp"
#include "Menu.hpp"
#include "Memory.hpp"
#include "Entity.hpp"

enum Screen {
    menu, game
};

struct GameState {
    
    float delta;
    bool paused;
    bool initialized;
    Screen currScreen;
    GameInput oldInput;
    GameInput input;
    Memory gameMemory;
    
    Uint32 textureCount;
    SDL_Texture *ninePatchTextures[3];
    SDL_Renderer *renderer;
    FC_Font *font;
    Atlas atlas;
    
    GameUi gameUi;
    World world;
    Menu menu;
    
    Entity entities[256];
    Uint32 entityCount;
    
    PaddleLogic paddles[4];
    Uint32 paddleCount;
    
    EnemyControl enemyControls[3];
    Uint32 enemyControlCount;
    
    BallLogic balls[16];
    Uint32 ballCount;
    
    BrickLogic bricks[128];
    Uint32 brickCount;
    
    Uint32 activeBrickCount;
    
    Level levels[4];
    Uint32 levelCount;
    Uint32 currentLevelIndex;
};

#endif
