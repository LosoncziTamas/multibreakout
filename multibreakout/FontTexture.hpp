#ifndef FontTexture_hpp
#define FontTexture_hpp

#include "Renderer.hpp"

struct FontTexture {
    SDL_Texture *sdlTexture;
    int width;
    int height;
    
    FontTexture(const char* text, const Renderer& renderer);
    FontTexture(const FontTexture& other) = delete;
    FontTexture& operator=(const FontTexture&) = delete;
    ~FontTexture();
};

#endif
