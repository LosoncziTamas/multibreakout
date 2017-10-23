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

void initAtlas(Atlas& atlas, std::string& json, const Renderer& renderer);

#endif 
