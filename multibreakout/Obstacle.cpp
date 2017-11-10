#include <SDL2/SDL_assert.h>

#include "Obstacle.hpp"
#include "MultiBreakout.hpp"
#include "Physics.hpp"
#include "Texture.hpp"


Uint32 addObstacle(Obstacles& obstacles, Uint32 width, Uint32 height, Vec2 center) {
    SDL_assert(obstacles.obstacleCount < SDL_arraysize(obstacles.content));
    
    Uint32 obstacleIndex = obstacles.obstacleCount++;
    Obstacle *obstacle = obstacles.content + obstacleIndex;
    
    obstacle->width = width;
    obstacle->height = height;
    obstacle->center = center;
    
    return obstacleIndex;
}

void initObstacles(Obstacles& obstacles) {
    Sint32 width = 30;
    Sint32 height = 30;
    
    obstacles.leftBottomIndex = addObstacle(obstacles, width, height, Vec2(160 + width * 0.5f, height * 0.5f));
    obstacles.leftTopIndex = addObstacle(obstacles, width, height, Vec2(160 + width * 0.5f, SCREEN_HEIGHT - height * 0.5f));
    obstacles.rightBottomIndex = addObstacle(obstacles, width, height, Vec2(SCREEN_WIDTH - 160 - width * 0.5f, height * 0.5f));
    obstacles.rightTopIndex = addObstacle(obstacles, width, height, Vec2(SCREEN_WIDTH - 160 - width * 0.5f, SCREEN_HEIGHT - height * 0.5f));
}

void collideWithObstacle(std::vector<Ball>& balls, Obstacles& obstacles) {
    for (auto& ball : balls) {
        for (Uint32 obstacleIndex = 0; obstacleIndex < obstacles.obstacleCount; ++obstacleIndex) {
            Obstacle& obstacle = obstacles.content[obstacleIndex];
            if (circleRectIntersect(ball.newPos, ball.radius, obstacle.center, obstacle.width, obstacle.height)) {
                Vec2 reflectionNorm = ball.newPos - obstacle.center;
                reflectionNorm.normalize();
                ball.velocity = reflectionNorm;
                float magnitude = (obstacle.width * 0.5f + ball.radius) - ball.newPos.distance(obstacle.center);
                if(magnitude > 0.0f) {
                    ball.movementDelta += reflectionNorm * magnitude;
                }
                ball.movementDelta += reflectionNorm;
            }
        }
    }
}

void drawObstacles(SDL_Renderer* renderer, Atlas& atlas, Obstacles& obstacles) {
    for (Uint32 obstacleIndex = 0; obstacleIndex < obstacles.obstacleCount; ++obstacleIndex) {
        Obstacle &obstacle = obstacles.content[obstacleIndex];
        int x = obstacle.center.x - obstacle.width * 0.5f;
        int y = SCREEN_HEIGHT - (obstacle.center.y + obstacle.height * 0.5f);
        SDL_Rect dstRec = {x, y, obstacle.width, obstacle.height};
        SDL_Rect srcRect = atlas.frames[obstacle.textureIndex];
        SDL_RenderCopy(renderer, atlas.texture, &srcRect, &dstRec);
    }
}