#include "multibreakout.h"

Vec2 operator*(float real, Vec2 vec)
{
    Vec2 result;

    result.x = real * vec.x;
    result.y = real * vec.y;

    return result;
}

Vec2 operator*(Vec2 vec, float real)
{
    Vec2 result = real * vec;

    return result;
}

Vec2 vec2(float x, float y)
{
    Vec2 result;

    result.x = x;
    result.y = y;

    return result;
}

void setFlag(Entity *entity, Uint32 flag)
{
    entity->flags |= flag;
}

Entity* addPaddleEntity(GameState* gameState, Vec2 pos)
{
    SDL_assert(SDL_arraysize(gameState->entities) > gameState->entityCount);
    
    Entity* paddle = gameState->entities + gameState->entityCount++;
    paddle->storageIndex = gameState->entityCount - 1;
    paddle->p = pos;
    paddle->w = 90.0f;
    paddle->h = 25.0f;
    paddle->dp = Vec2();
    paddle->type = ENTITY_TYPE_PADDLE;
    
    setFlag(paddle, ENTITY_FLAG_COLLIDES);
    
    return paddle;
}

void addEntities(GameState *gameState)
{
    Entity *nullEntity = gameState->entities + gameState->entityCount++;

    nullEntity->storageIndex = gameState->entityCount - 1;
    nullEntity->type = ENTITY_TYPE_NULL;

    SDL_assert(nullEntity->storageIndex == 0);

    float paddleHeight = 25.0f;

    Entity *bottomPaddleEntity = addPaddleEntity(gameState, vec2(800 * 0.5f, paddleHeight * 0.5f + 1));
    //PaddleLogic *bottomPaddleLogic = addPaddleLogic(bottomPaddleEntity, gameState, PADDLE_FLAG_ORIENTATION_BOTTOM | PADDLE_FLAG_PLAYER_CONTROLLED);

    /*Entity *bottomBallEntity = addBallEntity(gameState, Vec2());
    BallLogic *bottomBallLogic = addBallLogic(bottomBallEntity, gameState);

    anchorBallToPaddle(bottomBallEntity, bottomPaddleEntity, bottomBallLogic, bottomPaddleLogic);*/
}

void drawEntityBounds(SDL_Renderer* renderer, Entity* entity, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_Rect rect;
    rect.w = entity->w;
    rect.h = entity->h;
    rect.x = entity->p.x - (entity->w * 0.5f);
    rect.y = 480 - (entity->p.y + entity->h * 0.5f);
    
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawRect(renderer, &rect);
}

extern "C" void gameUpdate(GameMemory *gameMemory, GameInput *GameInput, SDL_Renderer *renderer)
{
    SDL_assert(sizeof(GameState) <= gameMemory->permanentMemorySize);

    GameState *gameState = (GameState *)gameMemory->permanentMemory;

    if (!gameMemory->initialized)
    {
        //load assets

        //create world

        //add entities
        addEntities(gameState);

        gameMemory->initialized = true;
    }

    //process game input

    //update entities

    SDL_SetRenderDrawColor(renderer, 130, 189, 240, 0);
    SDL_RenderClear(renderer);

    drawEntityBounds(renderer, &gameState->entities[1], 255, 255, 255, 255);

    SDL_RenderPresent(renderer);
}