#ifndef Texture_hpp
#define Texture_hpp

#include <SDL2_image/SDL_image.h>

#include "GameState.hpp"

struct Renderer;

const int GREY_BALL     = 0;
const int BLUE_BALL     = 1;
const int BROWN_BALL    = 2;
const int GREEN_BALL    = 3;
const int PURPLE_BALL   = 4;
const int BLUE_BRICK    = 5;
const int BROWN_BRICK   = 6;
const int GREY_BRICK    = 7;
const int GREEN_BRICK   = 8;
const int PURPLE_BRICK  = 9;
const int YELLOW_BRICK  = 10;
const int PLAYER_PADDLE = 11;
const int ENEMY_PADDLE  = 12;

SDL_Texture *createTexture(const char *path, const Renderer& renderer);
void initTextures(Renderer& renderer, GameState& gameState);
int getBallTexture(PowerUp powerUp);

#endif
