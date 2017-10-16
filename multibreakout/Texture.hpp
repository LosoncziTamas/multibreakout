#ifndef Texture_hpp
#define Texture_hpp

#include <SDL2_image/SDL_image.h>

struct Renderer;

struct Texture {
    SDL_Texture *sdlTexture;
    
    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(const char *path, const Renderer& renderer);
    ~Texture();

};
#endif
