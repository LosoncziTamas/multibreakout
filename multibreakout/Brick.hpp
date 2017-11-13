#ifndef Brick_hpp
#define Brick_hpp

#include <vector>
#include <SDL2/SDL_pixels.h>

#include "Vec2.hpp"
#include "Ball.hpp"
#include "Common.hpp"

struct Brick {
    Vec2 center;
    Sint32 width;
    Sint32 height;
    Uint32 textureIndex;
    bool active;
    void (*callback)(Brick* brick, Ball* ball);
    PowerUp powerUp;
};

void initBricks(World& world);
void collideWithBrick(World& world);
void drawBricksDebug(SDL_Renderer* renderer, World& world);
void drawBricks(SDL_Renderer* renderer, Atlas& atlas, World& world);

#endif
