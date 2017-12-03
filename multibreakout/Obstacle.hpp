#ifndef Obstacle_hpp
#define Obstacle_hpp

#include <vector>
#include <SDL2/SDL_render.h>

#include "Vec2.hpp"
#include "Ball.hpp"

struct Atlas;

const int OBSTACLES_SIZE_MAX = 32;

struct Obstacle {
    Vec2 center;
    Sint32 width;
    Sint32 height;
    Uint32 textureIndex;
};

struct Obstacles {
    Obstacle content[OBSTACLES_SIZE_MAX];
    Uint32 obstacleCount;
    
    Uint32 leftBottomIndex;
    Uint32 leftTopIndex;
    Uint32 rightBottomIndex;
    Uint32 rightTopIndex;
};

void initObstacles(Obstacles& obstacles);
void collideWithObstacle(World& world, Obstacles& obstacles);
void drawObstacles(SDL_Renderer* renderer, Atlas* atlas, Obstacles* obstacles);

#endif