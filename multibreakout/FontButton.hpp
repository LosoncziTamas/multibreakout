#ifndef FontButton_hpp
#define FontButton_hpp

#include <string>
#include <SDL2/SDL_render.h>

#include "SDL_FontCache.h"

struct GameState;

struct FontButton {
    const char* text;
    int x;
    int y;
    int w;
    int h;
    void (*onclick)(GameState& gameState);
};

void initFontButton(FontButton& button, FC_Font* font, int x, int y, const char* text, void (*onclick)(void));
void drawFontButton(FontButton& button, FC_Font* font, SDL_Renderer* renderer);
void updateFontButton(FontButton& button, GameState& gameState);

#endif
