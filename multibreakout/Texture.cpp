#include "Texture.hpp"
#include "Renderer.hpp"

Texture::Texture(const char* path, const Renderer& renderer) {
    SDL_Surface *surface = IMG_Load(path);
    SDL_assert(surface);
    sdlTexture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, surface);
    SDL_assert(sdlTexture);
    SDL_FreeSurface(surface);
}

Texture::~Texture() {
    SDL_DestroyTexture(sdlTexture);
}
