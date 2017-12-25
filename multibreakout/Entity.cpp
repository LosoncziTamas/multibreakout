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

void addEntities(GameState *gameState)
{
    Entity* paddle = gameState->entities + gameState->entityCount++;
    
    paddle->storageIndex = gameState->entityCount - 1;
    paddle->w = 90.0f;
    paddle->h = 25.0f;
    paddle->p = Vec2(SCREEN_WIDTH * 0.5f, DEFAULT_HEIGHT * 0.5f + 21.0f);
    paddle->dp = Vec2();
    paddle->type = ENTITY_TYPE_PADDLE;
    
    setFlag(paddle, ENTITY_FLAG_COLLIDES);
    
    Entity* ball = gameState->entities + gameState->entityCount++;
    
    ball->storageIndex = gameState->entityCount - 1;
    float radius = 10.0f;
    ball->w = radius * 2.0f;
    ball->h = radius * 2.0f;
    ball->p = Vec2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
    ball->dp = Vec2();
    ball->type = ENTITY_TYPE_BALL;
    
    setFlag(ball, ENTITY_FLAG_COLLIDES);
}

Vec2 getWallNorm(Entity* entity, Entity* wall)
{
    Rectangle rect = fromDimAndCenter(wall->p, wall->w, wall->h);
    
    bool left = entity->p.x <= rect.bottomLeft.x;
    bool right = entity->p.x >= rect.topRight.x;
    bool top = entity->p.y >= rect.topRight.y;
    bool bottom = entity->p.y <= rect.bottomLeft.y;
    
    if (right)
    {
        return Vec2(1.0f, 0.0f);
    }
    else if (left)
    {
        return Vec2(-1.0f, 0.0f);
    }
    else if (top)
    {
        return Vec2(0.0f, 1.0f);
    }
    else if (bottom)
    {
        return Vec2(0.0f, -1.0f);
    }
    else
    {
        SDL_TriggerBreakpoint();
        return Vec2();
    }
}

bool resolveCollision(Entity* entity, Entity* test, float remainingDistance, Vec2* desiredP, Vec2 testP)
{
    bool result = false;
    
    if (entity->type == ENTITY_TYPE_PADDLE && test->type == ENTITY_TYPE_OBSTACLE)
    {
        Vec2 wallNorm = getWallNorm(entity, test);
        entity->dp = reflect(entity->dp, wallNorm);
        *desiredP = testP + (remainingDistance * wallNorm);
        result = true;
    }
    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_OBSTACLE)
    {
        Vec2 wallNorm = getWallNorm(entity, test);
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
    
    return result;
}

void printEntity(Entity *entity)
{
    printf("dp.x: %f dp.y: %f \n", entity->dp.x, entity->dp.y);

}

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
        for (Uint32 entityIndex = 0; entityIndex < gameState->entityCount; ++entityIndex)
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
                        Rectangle verificationRect = fromDimAndCenter(desiredP, entity->w, entity->h);
                        if (aabb(verificationRect, testRect))
                        {
                            
                        }
                        
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

void updateEntities(GameState *gameState)
{
    if (!initialized)
    {
        addWalls(gameState);
        addEntities(gameState);
        initialized = true;
    }
    
    GameInput *input = &gameState->input;
        
    SDL_SetRenderDrawColor(gameState->renderer, 130, 189, 240, 0);
    SDL_RenderClear(gameState->renderer);
    
    for (Uint32 entityIndex = 0; entityIndex < gameState->entityCount; ++entityIndex)
    {
        Entity *entity = gameState->entities + entityIndex;
        Vec2 ddp;
        MovementSpecs specs;
        
        switch (entity->type)
        {
            case ENTITY_TYPE_PADDLE:
            {
                specs.speed = 500.0f;
                specs.drag = 2.0f;
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
                specs.speed = 200.0f;
                if (input->mouseRight) {
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
            case ENTITY_TYPE_OBSTACLE:
            {
                //do nothing
            } break;
            default:
            {
                SDL_TriggerBreakpoint();
            } break;
        }
        
        if (!isSet(entity, ENTITY_FLAG_STATIC))
        {
            moveEntity(gameState, entity, ddp, specs);
        }
        drawEntityBounds(gameState->renderer, entity);
    }
    
    SDL_RenderPresent(gameState->renderer);
}