#ifndef Obstacle_hpp
#define Obstacle_hpp

#include <vector>
#include <SDL2/SDL_render.h>

#include "Vec2.hpp"
#include "Ball.hpp"

struct Atlas;

const int OBSTACLES_SIZE = 4;

struct Obstacle {
    Vec2 center;
    int width;
    int height;
    int textureIndex;
};

struct Obstacles {
    union {
        Obstacle content[OBSTACLES_SIZE];
        struct {
            Obstacle leftBottom;
            Obstacle leftTop;
            Obstacle rightBottom;
            Obstacle rightTop;
        };
    };
};

void initObstacles(Obstacles& obstacles);
void collideWithObstacle(std::vector<Ball>& balls, Obstacles& obstacles);
void drawObstacles(SDL_Renderer* renderer, Atlas& atlas, Obstacles& obstacles);

#endif