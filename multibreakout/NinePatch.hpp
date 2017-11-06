#ifndef NinePatch_hpp
#define NinePatch_hpp

#include <SDL2/SDL_render.h>

const int LEFT_PANEL = 0;
const int RIGHT_PANEL = 1;
const int MENU_PANEL = 2;

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

void initNinePatcheBase(NinePatchBase& ninePatchBase, SDL_Renderer* renderer, std::vector<SDL_Texture*>& ninePatchTextures);
void generateTextureFromNinePatchBase(NinePatchBase& ninePatchBase, NinePatch &ninePatch, SDL_Renderer* renderer, std::vector<SDL_Texture*>& ninePatchTextures);
void generateNinePatches(SDL_Renderer* renderer, std::vector<SDL_Texture*>& ninePatchTextures);
void drawNinePatch(std::vector<SDL_Texture*>& ninePatchTextures, NinePatch& ninePatch, SDL_Renderer* renderer);
void deleteNinePatches(std::vector<SDL_Texture*>& ninePatchTextures);

#endif
