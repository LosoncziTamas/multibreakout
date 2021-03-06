#ifndef NinePatch_hpp
#define NinePatch_hpp

#include <SDL2/SDL_render.h>
#include <vector>

const int LEFT_PANEL = 0;
const int RIGHT_PANEL = 1;
const int MENU_PANEL = 2;

struct GameState;

struct NinePatchBase {
    SDL_Surface* surface;
    
    SDL_Rect topLeft;
    SDL_Rect bottomLeft;
    SDL_Rect topRight;
    SDL_Rect bottomRight;
    
    SDL_Rect fillableRight;
    SDL_Rect fillableLeft;
    SDL_Rect fillableTop;
    SDL_Rect fillableBottom;
    SDL_Rect fillableCenter;
};

struct NinePatch {
    int x;
    int y;
    int w;
    int h;
    int textureId;
};

void initNinePatcheBase(NinePatchBase& ninePatchBase, SDL_Renderer* renderer);
void generateTextureFromNinePatchBase(NinePatchBase& ninePatchBase, NinePatch &ninePatch, GameState &gameState);
void drawNinePatch(SDL_Texture** ninePatchTextures, NinePatch& ninePatch, SDL_Renderer* renderer);
void deleteNinePatches(SDL_Texture** ninePatchTextures, Uint32 textureCount);

#endif
