#include "multibreakout.h"

void setFlag(Entity *entity, Uint32 flag)
{
    entity->flags |= flag;
}

Entity *addPaddleEntity(GameState *gameState, Vec2 pos)
{
    SDL_assert(SDL_arraysize(gameState->entities) > gameState->entityCount);

    Entity *paddle = gameState->entities + gameState->entityCount++;
    paddle->storageIndex = gameState->entityCount - 1;
    paddle->p = pos;
    paddle->dimensions = vec2(3.0f, 1.0f);
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

    Vec2 paddlePos = vec2((gameState->tilesPerWidth / 2) * gameState->tileSideInMeters, gameState->tileSideInMeters * 0.5f);

    Entity *bottomPaddleEntity = addPaddleEntity(gameState, paddlePos);
    //PaddleLogic *bottomPaddleLogic = addPaddleLogic(bottomPaddleEntity, gameState, PADDLE_FLAG_ORIENTATION_BOTTOM | PADDLE_FLAG_PLAYER_CONTROLLED);

    /*Entity *bottomBallEntity = addBallEntity(gameState, Vec2());
    BallLogic *bottomBallLogic = addBallLogic(bottomBallEntity, gameState);

    anchorBallToPaddle(bottomBallEntity, bottomPaddleEntity, bottomBallLogic, bottomPaddleLogic);*/
}

void beginDraw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 130, 189, 240, 0);
    SDL_RenderClear(renderer);
}

void endDraw(SDL_Renderer *renderer)
{
    SDL_RenderPresent(renderer);
}

void drawRect(SDL_Renderer *renderer, GameState *gameState, Rect rectangle)
{
    SDL_Rect sdlRect;

    Vec2 rectDimInPixels = getRectangleDim(rectangle) * gameState->metersToPixels;

    sdlRect.w = rectDimInPixels.x;
    sdlRect.h = rectDimInPixels.y;
    sdlRect.x = rectangle.bottomLeftCorner.x * gameState->metersToPixels;
    sdlRect.y = gameState->screenDimensions.y - (rectangle.topRightCorner.y * gameState->metersToPixels);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &sdlRect);
}

extern "C" void gameUpdate(GameMemory *gameMemory, GameInput *GameInput, SDL_Renderer *renderer)
{
    SDL_assert(sizeof(GameState) <= gameMemory->permanentMemorySize);

    GameState *gameState = (GameState *)gameMemory->permanentMemory;

    if (!gameMemory->initialized)
    {
        //load assets

        //create world
        gameState->tileSideInMeters = 0.5f;
        gameState->worldDimensions = vec2(15.0f, 15.0f);
        gameState->tilesPerWidth = gameState->worldDimensions.x / gameState->tileSideInMeters;
        gameState->tilesPerHeight = gameState->worldDimensions.y / gameState->tileSideInMeters;

        gameState->tileSideInPixels = 16;
        gameState->metersToPixels = SDL_static_cast(float, gameState->tileSideInPixels) / gameState->tileSideInMeters;
        
        int screenWidth, screenHeight;
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

        gameState->screenDimensions = vec2(screenWidth, screenHeight);
        gameState->worldInScreenSpace = rectFromDimAndCenter(gameState->worldDimensions * gameState->metersToPixels, vec2(screenWidth * 0.5f, screenHeight * 0.5f));
        
        //add entities
        addEntities(gameState);

        gameMemory->initialized = true;
    }

    //process game input

    //update entities

    beginDraw(renderer);

    Entity* paddle = &gameState->entities[1];
    Rect paddleRect = rectFromDimAndCenter(paddle->dimensions, paddle->p);
    drawRect(renderer, gameState, paddleRect);

    endDraw(renderer);
}