#ifndef GameState_hpp
#define GameState_hpp

#include <vector>

#include "Game.hpp"
#include "MultiBreakout.hpp"
#include "Renderer.hpp"
#include "NinePatch.hpp"
#include "Menu.hpp"

enum Screen {
    menu, game
};

struct Memory {
    Uint8 *base;
    size_t size;
    size_t used;
};

struct GameState {
    
    float delta;
    bool paused;
    bool initialized;
    Screen currScreen;
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
};

#endif
