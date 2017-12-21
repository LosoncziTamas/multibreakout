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

void addEntities(GameState *gameState)
{
    Entity* paddle = gameState->entities + gameState->entityCount++;
    
    paddle->storageIndex = gameState->entityCount - 1;
    paddle->w = 90.0f;
    paddle->h = 25.0f;
    paddle->p = Vec2(SCREEN_WIDTH * 0.5f, DEFAULT_HEIGHT * 0.5f);
    paddle->dp = Vec2();
    paddle->type = ENTITY_TYPE_PADDLE;
    
    setFlag(paddle, ENTITY_FLAG_COLLIDES);
    
    Entity* wall = gameState->entities + gameState->entityCount++;
    
    wall->storageIndex = gameState->entityCount - 1;
    wall->w = 20.0f;
    wall->h = SCREEN_HEIGHT;
    wall->p = Vec2(150.f, SCREEN_HEIGHT * 0.5f);
    wall->dp = Vec2();
    wall->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(wall, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
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

void resolveCollision(Entity* a, Entity* b)
{
    //NOTE: less dominant should be first
    if (a->type > b->type)
    {
        Entity* tmp = a;
        a = b;
        b = tmp;
    }
    
    if (a->type == ENTITY_TYPE_PADDLE && b->type == ENTITY_TYPE_OBSTACLE)
    {
        Vec2 wallNorm(1, 0);
        a->dp = reflect(a->dp, wallNorm);
    }
}

void moveEntity(GameState *gameState, Entity *entity, Vec2 ddp, MovementSpecs specs)
{
    float delta = gameState->delta;
    
    ddp *= specs.speed;
    ddp += - specs.drag * entity->dp;
    
    Vec2 oldP = entity->p;
    Vec2 movementDelta = (0.5f * ddp * pow(delta, 2) + entity->dp * delta);
    entity->dp = ddp * delta + entity->dp;
    Vec2 newP = oldP + movementDelta;
    entity->p = newP;
    
    for (Uint32 entityIndex = entity->storageIndex + 1; entityIndex < gameState->entityCount; ++entityIndex)
    {
        Entity *test = gameState->entities + entityIndex;
        SDL_assert(test->storageIndex > entity->storageIndex);
        Rectangle entityRect = fromDimAndCenter(entity->p, entity->w, entity->h);
        Rectangle testRect = fromDimAndCenter(test->p, test->w, test->h);
        
        bool collides = isSet(entity, ENTITY_FLAG_COLLIDES) && isSet(test, ENTITY_FLAG_COLLIDES);
        if (collides && aabb(entityRect, testRect))
        {
            resolveCollision(entity, test);
        }
    }
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


void updateEntities(GameState *gameState)
{
    if (!initialized)
    {
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
                //ddp.x = 1.0f;
                //specs.speed = 200.0f;
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