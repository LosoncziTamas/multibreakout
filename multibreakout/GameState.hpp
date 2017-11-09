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

struct GameState {
    float delta;
    bool paused;
    bool initialized;
    Screen currScreen;
    GameInput input;
    
    std::vector<SDL_Texture*> ninePatchTextures;
    SDL_Renderer *renderer;
    FC_Font *font;
    Atlas atlas;
    
    GameUi gameUi;
    World world;
    Menu menu;
};

#endif
