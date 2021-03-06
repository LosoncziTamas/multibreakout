#include <SDL2/SDL_assert.h>

#include "Brick.hpp"
#include "MultiBreakout.hpp"
#include "Physics.hpp"
#include "Renderer.hpp"
#include "Game.hpp"

void callback(Brick* brick, Ball* ball) {
    SDL_assert(brick && ball);
    switch (brick->powerUp) {
        case speedUp:
        case slowDown:
        case strech:
        case shrink:
        case neutral:
            break;
        default:
            SDL_assert(false);
            break;
    }
    ball->powerUp = brick->powerUp;
    ball->textureIndex = getBallTexture(ball->powerUp);
    brick->active = false;
}

void initBricks(World* world) {
    int brickWidth = 30;
    int brickHeight = 30;
    int columns = 4;
    int rows = 5;
    float halfWidth = brickWidth * 0.5f;
    float halfHeight = brickHeight * 0.5f;
    float pivotX = SCREEN_WIDTH * 0.5f - columns * halfHeight;
    float pivotY = SCREEN_HEIGHT * 0.5f + rows * halfWidth;
    
    Brick *brick = 0;
    
    for (int i = 0; i < columns; ++i) {
        for (int j = 0; j < rows; ++j) {
            SDL_assert(world->brickCount < SDL_arraysize(world->bricks));
            
            brick = world->bricks + world->brickCount++;
            brick->center = Vec2(pivotX + (i * brickWidth) + halfWidth, pivotY - (j * brickHeight) + halfHeight);
            brick->width = brickWidth;
            brick->height = brickHeight;
            brick->active = true;
            brick->callback = callback;
            brick->powerUp = SDL_static_cast(PowerUp, rand() % (shrink + 1));
            brick->textureIndex = getBrickTexture(brick->powerUp);
        }
    }
}

void collideWithBrick(World& world) {
    for (Uint32 ballIndex = 0; ballIndex < world.ballCount; ++ballIndex) {
        Ball *ball = world.balls + ballIndex;
        for (Uint32 brickIndex = 0; brickIndex < world.brickCount; ++brickIndex) {
            Brick *brick = world.bricks + brickIndex;
            if (!brick->active)
                continue;
            if (circleRectIntersect(ball->newPos, ball->radius, brick->center, brick->width, brick->height)) {
                Vec2 reflectionNorm = ball->newPos - brick->center;
                reflectionNorm.normalize();
                ball->velocity = reflectionNorm;
                ball->movementDelta += reflectionNorm;
                brick->callback(brick, ball);
            }
        }
    }
}

void drawBricksDebug(SDL_Renderer* renderer, World& world) {
    for (Uint32 brickIndex = 0; brickIndex < world.brickCount; ++brickIndex) {
        Brick *brick = world.bricks + brickIndex;
        if (!brick->active) {
            continue;
        }
        SDL_Color color = getDrawColor(brick->powerUp);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        int x = brick->center.x - brick->width * 0.5f;
        int y = SCREEN_HEIGHT - (brick->center.y + brick->height * 0.5f);
        SDL_Rect sdlRect = {x, y, brick->width, brick->height};
        SDL_RenderDrawRect(renderer, &sdlRect);
    }
}

void drawBricks(SDL_Renderer* renderer, Atlas* atlas, World* world) {
    for (Uint32 brickIndex = 0; brickIndex < world->brickCount; ++brickIndex) {
        Brick *brick = world->bricks + brickIndex;
        if (!brick->active) {
            continue;
        }
        int x = brick->center.x - brick->width * 0.5f;
        int y = SCREEN_HEIGHT - (brick->center.y + brick->height * 0.5f);
        SDL_Rect dstRec = {x, y, brick->width, brick->height};
        SDL_Rect srcRect = atlas->frames[brick->textureIndex];
        SDL_RenderCopy(renderer, atlas->texture, &srcRect, &dstRec);
    }
}