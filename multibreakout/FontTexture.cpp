#include "FontTexture.hpp"

FontTexture::FontTexture(const char* text, const Renderer& renderer) {
    SDL_Surface *sdlSurface = TTF_RenderUTF8_Solid(renderer.sdlFont, text, BLACK);
    SDL_assert(sdlSurface);
    sdlTexture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, sdlSurface);
    SDL_FreeSurface(sdlSurface);
    SDL_assert(sdlTexture);
    TTF_SizeUTF8(renderer.sdlFont, text, &width, &height);
}

FontTexture::~FontTexture() {
    SDL_DestroyTexture(sdlTexture);
}