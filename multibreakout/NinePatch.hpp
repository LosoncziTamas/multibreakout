#ifndef NinePatch_hpp
#define NinePatch_hpp

#include <SDL2/SDL_render.h>

struct Renderer;

struct NinePatch {
    int x;
    int y;
    int w;
    int h;
    SDL_Texture* texture;
};

void initNinePatch(NinePatch& ninePatch, Renderer& renderer);
void drawNinePatch(NinePatch& ninePatch, Renderer& renderer);
void deleteNinePatch(NinePatch& ninePatch);

#endif
