#include "Entity.hpp"
#include "GameState.hpp"

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
    
    //collision detection
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