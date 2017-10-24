#ifndef Atlas_hpp
#define Atlas_hpp

#include <vector>
#include <SDL2/SDL_rect.h>

struct Renderer;
struct SDL_Texture;

struct Atlas {
    std::vector<SDL_Rect> frames;
    SDL_Texture *texture;
};

void initAtlas(std::string& json, Renderer& renderer);

#endif 
