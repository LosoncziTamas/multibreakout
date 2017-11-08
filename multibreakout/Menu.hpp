#ifndef Menu_hpp
#define Menu_hpp

#include <SDL2/SDL_rect.h>

#include "NinePatch.hpp"
#include "FontButton.hpp"

struct GameState;
struct Atlas;

struct Menu {
    bool initialized;
    SDL_Rect gameTitleRect;
    NinePatch menuPanel;
    FontButton startGame;
    FontButton signIn;
    FontButton leaderBoards;
};

void initializeMenu(Menu& menu, FC_Font* font, Atlas& atlas);
void updateMenu(Menu& menu, GameInput& input);
void drawMenu(Menu& menu, SDL_Renderer* renderer, FC_Font* font, Atlas& atlas, std::vector<SDL_Texture*>& ninePatchTextures);


#endif