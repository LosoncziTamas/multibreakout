#include "Entity.hpp"
#include "GameState.hpp"
#include "Physics.hpp"

static bool initialized;

bool isSet(Entity *entity, Uint32 flag)
{
    return entity->flags & flag;
}

void setFlag(Entity *entity, Uint32 flag)
{
    entity->flags |= flag;
}

void clearFlag(Entity *entity, Uint32 flag)
{
    entity->flags &= ~flag;
}

void addWalls(GameState *gameState)
{
    Entity* leftWall = gameState->entities + gameState->entityCount++;
    
    leftWall->storageIndex = gameState->entityCount - 1;
    leftWall->w = 20.0f;
    leftWall->h = SCREEN_HEIGHT;
    leftWall->p = Vec2(150.f, SCREEN_HEIGHT * 0.5f);
    leftWall->dp = Vec2();
    leftWall->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(leftWall, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    Entity* rightWall = gameState->entities + gameState->entityCount++;
    
    rightWall->storageIndex = gameState->entityCount - 1;
    rightWall->w = 20.0f;
    rightWall->h = SCREEN_HEIGHT;
    rightWall->p = Vec2(SCREEN_WIDTH - 150.f, SCREEN_HEIGHT * 0.5f);
    rightWall->dp = Vec2();
    rightWall->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(rightWall, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    Entity* topWall = gameState->entities + gameState->entityCount++;
    
    topWall->storageIndex = gameState->entityCount - 1;
    topWall->w = 480.0f;
    topWall->h = 20.0f;
    topWall->p = Vec2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT - 10);
    topWall->dp = Vec2();
    topWall->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(topWall, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    Entity* bottomWall = gameState->entities + gameState->entityCount++;
    
    bottomWall->storageIndex = gameState->entityCount - 1;
    bottomWall->w = 480.0f;
    bottomWall->h = 20.0f;
    bottomWall->p = Vec2(SCREEN_WIDTH * 0.5f, 10);
    bottomWall->dp = Vec2();
    bottomWall->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(bottomWall, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
}

void addBalls(GameState *gameState)
{
    Entity* ball = gameState->entities + gameState->entityCount++;
    
    ball->storageIndex = gameState->entityCount - 1;
    float radius = 10.0f;
    ball->w = radius * 2.0f;
    ball->h = radius * 2.0f;
    ball->p = Vec2(SCREEN_WIDTH * 0.5f, 60);
    ball->dp = Vec2();
    ball->type = ENTITY_TYPE_BALL;
    
    setFlag(ball, ENTITY_FLAG_COLLIDES);
    
    ball = gameState->entities + gameState->entityCount++;
    
    ball->storageIndex = gameState->entityCount - 1;
    ball->w = radius * 2.0f;
    ball->h = radius * 2.0f;
    ball->p = Vec2(SCREEN_WIDTH * 0.5f, 90);
    ball->dp = Vec2();
    ball->type = ENTITY_TYPE_BALL;
    
    setFlag(ball, ENTITY_FLAG_COLLIDES);

    ball = gameState->entities + gameState->entityCount++;
    
    ball->storageIndex = gameState->entityCount - 1;
    ball->w = radius * 2.0f;
    ball->h = radius * 2.0f;
    ball->p = Vec2(SCREEN_WIDTH * 0.5f - ball->w, 80);
    ball->dp = Vec2();
    ball->type = ENTITY_TYPE_BALL;
    
    setFlag(ball, ENTITY_FLAG_COLLIDES);
    
    ball = gameState->entities + gameState->entityCount++;
    
    ball->storageIndex = gameState->entityCount - 1;
    ball->w = radius * 2.0f;
    ball->h = radius * 2.0f;
    ball->p = Vec2(SCREEN_WIDTH * 0.5f + ball->w, 80);
    ball->dp = Vec2();
    ball->type = ENTITY_TYPE_BALL;
    
    setFlag(ball, ENTITY_FLAG_COLLIDES);
    
}

void addEntities(GameState *gameState)
{
    /* NOTE: currently dominance during collision resolution
    is determined by the order of the entities */
    
    Entity* nullEntity = gameState->entities + gameState->entityCount++;
    
    nullEntity->storageIndex = gameState->entityCount - 1;
    nullEntity->type = ENTITY_TYPE_NULL;
    
    SDL_assert(nullEntity->storageIndex == 0);
    
    Entity* paddle = gameState->entities + gameState->entityCount++;
    
    paddle->storageIndex = gameState->entityCount - 1;
    paddle->w = 90.0f;
    paddle->h = 25.0f;
    paddle->p = Vec2(SCREEN_WIDTH * 0.5f, DEFAULT_HEIGHT * 0.5f + 21.0f);
    paddle->dp = Vec2();
    paddle->type = ENTITY_TYPE_PADDLE;
    
    setFlag(paddle, ENTITY_FLAG_COLLIDES);
    
    addBalls(gameState);

    float brickWidth = 30;
    float brickHeight = 30;
    Uint32 columns = 4;
    Uint32 rows = 5;
    float halfWidth = brickWidth * 0.5f;
    float halfHeight = brickHeight * 0.5f;
    float pivotX = SCREEN_WIDTH * 0.5f - columns * halfHeight;
    float pivotY = SCREEN_HEIGHT * 0.5f + rows * halfWidth;
    
    for (Uint32 columnIndex = 0; columnIndex < columns; ++columnIndex)
    {
        for (Uint32 rowIndex = 0; rowIndex < rows; ++rowIndex)
        {
            Entity* brick = gameState->entities + gameState->entityCount++;
            brick->p = Vec2(pivotX + (columnIndex * brickWidth) + halfWidth,
                            pivotY - (rowIndex * brickHeight) + halfHeight);
            brick->w = brickWidth;
            brick->h = brickHeight;
            brick->type = ENTITY_TYPE_BRICK;
            brick->storageIndex = gameState->entityCount - 1;
            setFlag(brick, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
        }
    }
    
    Entity* projectile = gameState->entities + gameState->entityCount++;
    
    projectile->storageIndex = gameState->entityCount - 1;
    projectile->p = Vec2(400, 100);
    projectile->dp = Vec2(0.0f, 1.0f);
    projectile->w = 15.0f;
    projectile->h = 30.0f;
    projectile->type = ENTITY_TYPE_PROJECTILE;

    setFlag(projectile, ENTITY_FLAG_COLLIDES);
}

Vec2 getSurfaceNorm(Vec2 desiredP, Entity* rectEntity)
{
    Rectangle rect = fromDimAndCenter(rectEntity->p, rectEntity->w, rectEntity->h);
    
    bool left = desiredP.x <= rect.bottomLeft.x;
    bool right = desiredP.x >= rect.topRight.x;
    bool top = desiredP.y >= rect.topRight.y;
    bool bottom = desiredP.y <= rect.bottomLeft.y;
    
    Vec2 result;
    
#if 0
    if ((right && top) || (right && bottom) ||
        (left && top) || (left && bottom))
    {
        result = (desiredP - rectEntity->p).normalize();
    } else
#endif
    
    if (right)
    {
        result = Vec2(1.0f, 0.0f);
    }
    else if (left)
    {
        result = Vec2(-1.0f, 0.0f);
    }
    else if (top)
    {
        result = Vec2(0.0f, 1.0f);
    }
    else if (bottom)
    {
        result = Vec2(0.0f, -1.0f);
    }
    
    return result;
}

bool resolveCollision(Entity* entity, Entity* test, float remainingDistance, Vec2* desiredP, Vec2 testP)
{
    bool result = false;
    
    if (entity->type == ENTITY_TYPE_PADDLE && test->type == ENTITY_TYPE_OBSTACLE)
    {
        Vec2 wallNorm = getSurfaceNorm(*desiredP, test);
        entity->dp = reflect(entity->dp, wallNorm);
        *desiredP = testP + (remainingDistance * wallNorm);
        result = true;
    }
    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_OBSTACLE)
    {
        Vec2 wallNorm = getSurfaceNorm(*desiredP, test);
        if (wallNorm.length() == 0)
        {
            wallNorm = (entity->p - *desiredP).normalize();
        }
        entity->dp = reflect(entity->dp, wallNorm);
        *desiredP = testP + (remainingDistance * wallNorm);
        result = true;
    }
    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_PADDLE)
    {
        if (circleRectIntersect(*desiredP, entity->w * 0.5f, test->p, test->w, test->h))
        {
            Vec2 norm = (*desiredP - test->p).normalize();
            entity->dp = (norm * entity->dp.length()) + (norm * test->dp.length());
            *desiredP = testP + (remainingDistance * norm);
            result = true;
        }
    }
    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_BRICK)
    {
        if (circleRectIntersect(*desiredP, entity->w * 0.5f, test->p, test->w, test->h))
        {
            Vec2 norm = getSurfaceNorm(*desiredP, test);
            if (norm.length() == 0)
            {
                norm = (*desiredP - test->p).normalize();
            }
            entity->dp = reflect(entity->dp, norm);
            *desiredP = testP + (remainingDistance * norm);
            result = true;
        }
    }
    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_BALL)
    {
        float distance = entity->p.distance(test->p);
        bool collide = distance <= entity->w * 0.5 + test->w * 0.5;
        if (collide)
        {
            Vec2 norm = (entity->p - test->p).normalize();
            entity->dp = norm * entity->dp.length();
            *desiredP = testP + (remainingDistance * norm);
            return true;
        }
    }
    
    return result;
}

void printEntity(Entity *entity)
{
    printf("dp.x: %f dp.y: %f \n", entity->dp.x, entity->dp.y);
}

struct CollisionSpecs
{
    Vec2 desiredP;
    Vec2 desiredDp;
    Vec2 oldP;
    float deltaLength;
};

void moveEntity(GameState *gameState, Entity *entity, Vec2 ddp, MovementSpecs specs)
{
    float delta = gameState->delta;
    
    ddp *= specs.speed;
    ddp += - specs.drag * entity->dp;
    
    Vec2 oldP = entity->p;
    Vec2 movementDelta = (0.5f * ddp * pow(delta, 2) + entity->dp * delta);
    entity->dp = ddp * delta + entity->dp;
    Vec2 desiredP = oldP + movementDelta;
    float deltaLength = movementDelta.length();
    
    if (isSet(entity, ENTITY_FLAG_COLLIDES))
    {
        for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
        {
            Entity *test = gameState->entities + entityIndex;
            if (test == entity || !isSet(test, ENTITY_FLAG_COLLIDES))
            {
                continue;
            }
            
            Rectangle testRect = fromDimAndCenter(test->p, test->w, test->h);
            
            Uint32 iterationCount = 4;
            for (Uint32 iteration = 0; iteration <= iterationCount; ++iteration)
            {
                float t = iteration * (1.0 / iterationCount);
                Vec2 testP = ((1.0 - t) * oldP) + (t * desiredP);
                Rectangle entityRect = fromDimAndCenter(testP, entity->w, entity->h);
                
                if (aabb(entityRect, testRect))
                {
                    float remainingDistance = (1.0f - t) * deltaLength;
                    if (remainingDistance == 0.0f && iteration == iterationCount)
                    {
                        remainingDistance = deltaLength * (1.0f / iterationCount);
                    }
                    
                    bool collides = resolveCollision(entity, test, remainingDistance, &desiredP, testP);
                    if (collides)
                    {
                        break;
                    }
                }
            }
        }
    }
    
    entity->p = desiredP;
}



void drawEntityBounds(SDL_Renderer* renderer, Entity* entity) {
    SDL_Rect rect;
    rect.w = entity->w;
    rect.h = entity->h;
    rect.x = entity->p.x - (entity->w * 0.5f);
    rect.y = SCREEN_HEIGHT - (entity->p.y + entity->h * 0.5f);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void drawCircle(SDL_Renderer* renderer, Entity* entity)
{
    int x0 = round(entity->p.x);
    int y0 = round(SCREEN_HEIGHT - entity->p.y);
    int radius = entity->w * 0.5f;
        
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
        SDL_RenderDrawPoint(renderer, x0 + y, y0 + x);
        SDL_RenderDrawPoint(renderer, x0 - y, y0 + x);
        SDL_RenderDrawPoint(renderer, x0 - x, y0 + y);
        SDL_RenderDrawPoint(renderer, x0 - x, y0 - y);
        SDL_RenderDrawPoint(renderer, x0 - y, y0 - x);
        SDL_RenderDrawPoint(renderer, x0 + y, y0 - x);
        SDL_RenderDrawPoint(renderer, x0 + x, y0 - y);
        
        if (err <= 0)
        {
            y++;
            err += dy;
            dy +=2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-radius << 1) + dx;
        }
    }
}

void setStaticCollider(CollisionSpecs* collider, Vec2 center)
{
    SDL_assert(collider);
    
    collider->deltaLength = 0.0f;
    collider->desiredDp = Vec2();
    collider->desiredP = center;
    collider->oldP = center;
}

void updateEntities(GameState *gameState)
{
    if (!initialized)
    {
        addEntities(gameState);
        addWalls(gameState);
        initialized = true;
    }
    
    GameInput *input = &gameState->input;
        
    SDL_SetRenderDrawColor(gameState->renderer, 130, 189, 240, 0);
    SDL_RenderClear(gameState->renderer);
    
    CollisionSpecs* colliders = pushArray(&gameState->gameMemory, gameState->entityCount, CollisionSpecs);
    MovementSpecs* movements = pushArray(&gameState->gameMemory, gameState->entityCount, MovementSpecs);
    
    for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
    {
        Entity *entity = gameState->entities + entityIndex;
        SDL_assert(entity->storageIndex > 0);
        
        MovementSpecs* specs = movements + entityIndex;
        Vec2 ddp;
        
        switch (entity->type)
        {
            case ENTITY_TYPE_PADDLE:
            {
                specs->speed = 500.0f;
                specs->drag = 2.0f;
                
                if (input->left)
                {
                    ddp.x = -1.0;
                }
                else if (input->right)
                {
                    ddp.x = 1.0;
                }
            } break;
            case ENTITY_TYPE_BALL:
            {
                specs->speed = 200.0f;
                specs->drag = 2.0f;
                
                if (input->mouseRight)
                {
                    Vec2 newVelocity(input->mouseX - entity->p.x, SCREEN_HEIGHT - input->mouseY - entity->p.y);
                    entity->dp = Vec2();
                    ddp = newVelocity.normalize();
                }
                else
                {
                    Vec2 normalized(entity->dp);
                    ddp = normalized.normalize();
                }
                drawCircle(gameState->renderer, entity);
                
            } break;
            case ENTITY_TYPE_PROJECTILE:
            {
                specs->speed = 1000.0f;
                
                if (input->mouseLeft)
                {
                    entity->p = Vec2(input->mouseX, SCREEN_HEIGHT - input->mouseY);
                    entity->dp = Vec2(0.0f, 1.0f);
                    ddp = entity->dp;
                }
                else
                {
                    Vec2 normalized(entity->dp);
                    ddp = normalized.normalize();
                }
            } break;
            case ENTITY_TYPE_OBSTACLE:
            case ENTITY_TYPE_BRICK:
            break;
            default:
            {
                SDL_TriggerBreakpoint();
            } break;
        }
        
        CollisionSpecs* collider = colliders + entityIndex;
        
        if (isSet(entity, ENTITY_FLAG_STATIC))
        {
            setStaticCollider(collider, entity->p);
        }
        else
        {
            float delta = gameState->delta;
            ddp *= specs->speed;
            ddp += - specs->drag * entity->dp;
            
            Vec2 movementDelta = (0.5f * ddp * pow(delta, 2) + entity->dp * delta);
            
            collider->oldP = entity->p;
            collider->desiredDp = ddp * delta + entity->dp;
            collider->desiredP = entity->p + movementDelta;
            collider->deltaLength = movementDelta.length();
        }
    }
    
    Uint32 iterationCount = 4;
    for (Uint32 iteration = 0; iteration <= iterationCount; ++iteration)
    {
        float t = iteration * (1.0 / iterationCount);
        
        for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
        {
            Entity* entity = gameState->entities + entityIndex;
            
            CollisionSpecs* collider = colliders + entity->storageIndex;
            Vec2 entityLerpP = ((1.0 - t) * collider->oldP) + (t * collider->desiredP);
            
            Rectangle entityRect = fromDimAndCenter(entityLerpP, entity->w, entity->h);
            
            for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
            {
                Entity* test = gameState->entities + entityIndex;
                if (test == entity || !isSet(test, ENTITY_FLAG_COLLIDES))
                {
                    continue;
                }
                
                CollisionSpecs* testCollider = colliders + test->storageIndex;
                Vec2 testLerpP = ((1.0 - t) * testCollider->oldP) + (t * testCollider->desiredP);

                Rectangle testRect = fromDimAndCenter(testLerpP, test->w, test->h);
                
                if (!(isSet(entity, ENTITY_FLAG_STATIC) && isSet(test, ENTITY_FLAG_STATIC)) && aabb(entityRect, testRect))
                {
                    float remainingDistance = (1.0f - t) * collider->deltaLength;
                    if (remainingDistance == 0.0f && iteration == iterationCount)
                    {
                        remainingDistance = collider->deltaLength * (1.0f / iterationCount);
                    }
                    
                    float testRemainingDistance = (1.0f - t) * testCollider->deltaLength;
                    if (testRemainingDistance == 0.0f && iteration == iterationCount)
                    {
                        testRemainingDistance = testCollider->deltaLength * (1.0f / iterationCount);
                    }
                    
                    if (entity->type == ENTITY_TYPE_PADDLE && test->type == ENTITY_TYPE_OBSTACLE)
                    {
                        Vec2 wallNorm = getSurfaceNorm(collider->desiredP, test);
                        collider->desiredDp = reflect(entity->dp, wallNorm);
                        collider->desiredP = entityLerpP + (remainingDistance * wallNorm);
                        
                        testCollider->desiredP = testCollider->oldP;

                    }
                    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_OBSTACLE)
                    {
                        Vec2 wallNorm = getSurfaceNorm(collider->desiredP, test);
                        if (wallNorm.length() == 0)
                        {
                            wallNorm = (entity->p - collider->desiredP).normalize();
                        }
                        collider->desiredDp = reflect(entity->dp, wallNorm);
                        collider->desiredP = entityLerpP + (remainingDistance * wallNorm);
                        
                        testCollider->desiredP = testCollider->oldP;
                    }
                    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_PADDLE)
                    {
                        if (circleRectIntersect(collider->desiredP, entity->w * 0.5f, testLerpP, test->w, test->h))
                        {
                            //TODO: implement better force distribution
                            
                            float extraForceLength = test->dp.length() * 0.5f;
                            
                            {
                                Vec2 norm = (collider->desiredP - testLerpP).normalize();
                                collider->desiredDp = (norm * entity->dp.length()) + (norm * extraForceLength);
                                collider->desiredP = entityLerpP + (remainingDistance * norm);
                            }
                            
                            {
                                Vec2 testNorm = (testLerpP - collider->desiredP).normalize();
                                testNorm.y = 0.0f;
                                testCollider->desiredP = testLerpP + (testRemainingDistance * testNorm);
                                testCollider->desiredDp = testNorm * extraForceLength;
                            }

                        }
                    }
                    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_BALL)
                    {
                        float distance = entityLerpP.distance(testLerpP);
                        bool collide = distance <= entity->w * 0.5 + test->w * 0.5;
                        if (collide)
                        {
                            {
                                Vec2 norm = (entityLerpP - testLerpP).normalize();
                                collider->desiredDp = norm * entity->dp.length();
                                collider->desiredP = entityLerpP + (remainingDistance * norm);
                            }
                            
                            {
                                Vec2 testNorm = (testLerpP - entityLerpP).normalize();
                                testCollider->desiredDp = testNorm * test->dp.length();
                                testCollider->desiredP = testLerpP + (testRemainingDistance * testNorm);
                            }
                        }
                    }
                    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_BRICK)
                    {
                        if (circleRectIntersect(collider->desiredP, entity->w * 0.5f, testLerpP, test->w, test->h))
                        {
                            Vec2 norm = getSurfaceNorm(collider->desiredP, test);
                            if (norm.length() == 0)
                            {
                                norm = (collider->desiredP - testLerpP).normalize();
                            }
                            collider->desiredDp = reflect(entity->dp, norm);
                            collider->desiredP = entityLerpP + (remainingDistance * norm);
                            
                            testCollider->desiredP = testCollider->oldP;
                        }
                    }
                }
            }
        }
    }
    
    //TODO: Collision result finalization
    for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
    {
        Entity* entity = gameState->entities + entityIndex;
        SDL_assert(entity->storageIndex > 0);

        CollisionSpecs* collider = colliders + entity->storageIndex;
        
        if (!isSet(entity, ENTITY_FLAG_STATIC))
        {
            entity->p = collider->desiredP;
            entity->dp = collider->desiredDp;
        }
        
        drawEntityBounds(gameState->renderer, entity);
    }
    
    clearArray(colliders, gameState->entityCount, CollisionSpecs);
    clearArray(movements, gameState->entityCount, MovementSpecs);
    
    SDL_RenderPresent(gameState->renderer);
}