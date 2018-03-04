#include "Projectile.hpp"
#include "GameState.hpp"
#include "Physics.hpp"

Projectile* addProjectile(Vec2 location, World* world) {
    SDL_assert(world->projectileCount < SDL_arraysize(world->projectiles));
    
    Projectile* projectile = world->projectiles + world->projectileCount++;
    projectile->velocity = Vec2(0.0f, 1.0f);
    projectile->newPos = location;
    projectile->speed = 50.0;
    projectile->width = 15.0f;
    projectile->height = 30.0f;
    
    return projectile;
}

void updateProjectiles(World* world, SDL_Renderer* renderer, float delta) {
    for (Uint32 projectileIndex = 0; projectileIndex < world->projectileCount; ++projectileIndex) {
        Projectile* projectile = world->projectiles + projectileIndex;
        
        Vec2 acceleration = Vec2(0.0f, 1.0f);
        acceleration *= projectile->speed;
        
        projectile->oldPos = projectile->newPos;
        projectile->delta = (0.5f * acceleration * pow(delta, 2) + projectile->velocity * delta);
        projectile->velocity += acceleration * delta;
        projectile->newPos = projectile->oldPos + projectile->delta;
        
        Rectangle projectileRect = fromDimAndCenter(projectile->newPos,
                                                    projectile->width,
                                                    projectile->height);
        
        Rectangle enemyRect = fromDimAndCenter(world->enemyUpper.paddle.newPos,
                                               world->enemyUpper.paddle.width,
                                               world->enemyUpper.paddle.height);
        
        bool remove = !isInRectangle(world->bounds, projectile->newPos) || aabb(enemyRect, projectileRect);
        if (remove) {
            world->projectiles[projectileIndex] = world->projectiles[--world->projectileCount];
            return;
        }
        
        for (Uint32 brickIndex = 0; brickIndex < world->brickCount; ++brickIndex) {
            Brick* brick = world->bricks + brickIndex;
            Rectangle brickRect = fromDimAndCenter(brick->center, brick->width, brick->height);
            if (aabb(brickRect, projectileRect)) {
                world->bricks[brickIndex] = world->bricks[--world->brickCount];
            }
        }
        
        for (Uint32 ballIndex = 0; ballIndex < world->ballCount; ++ballIndex) {
            Ball* ball = world->balls + ballIndex;
            if (circleRectIntersect(ball->newPos, ball->radius, projectile->newPos, projectile->width, projectile->height)) {
                Vec2 reflection = ball->newPos - projectile->newPos;
                ball->velocity = reflection.normalize();
                ball->movementDelta += reflection * ball->movementDelta.length();
            }
        }
        
        SDL_Rect rect;
        rect.w = round(projectile->width);
        rect.h = round(projectile->height);
        rect.x = round(projectile->newPos.x - projectile->width * 0.5f);
        rect.y = round(SCREEN_HEIGHT - (projectile->newPos.y + projectile->height * 0.5f));
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }
}
