#ifndef Texture_hpp
#define Texture_hpp

#include <SDL2_image/SDL_image.h>

#include "GameState.hpp"

struct Renderer;

SDL_Texture *createTexture(const char *path, const Renderer& renderer);
void initTextures(Renderer& renderer, GameState& gameState);

#endif
