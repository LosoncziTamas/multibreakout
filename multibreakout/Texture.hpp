#ifndef Texture_hpp
#define Texture_hpp

#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <vector>

#include "Common.hpp"

struct World;

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
const int LEFT_BUTTON   = 12;
const int ENEMY_PADDLE  = 13;
const int RIGHT_BUTTON  = 14;

struct Atlas {
    std::vector<SDL_Rect> frames;
    SDL_Texture *texture;
};

SDL_Texture *createTexture(const char *path, SDL_Renderer* renderer);
void initTextures(SDL_Renderer* renderer, Atlas& atlas, World& world);
int getBrickTexture(PowerUp powerUp);
int getBallTexture(PowerUp powerUp);

#endif
