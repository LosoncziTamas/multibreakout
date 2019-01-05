#include "multibreakout.h"

void setPaddleInput(GameInput *input, Entity *paddleEntity)
{
    SDL_assert(paddleEntity->type == ENTITY_TYPE_PADDLE);
    SDL_assert(paddleEntity->paddleState);

    PaddleState *paddleState = paddleEntity->paddleState;

    if (paddleState->flags & PADDLE_FLAG_PLAYER_CONTROLLED)
    {
        if (paddleState->flags & PADDLE_FLAG_ORIENTATION_BOTTOM)
        {
            paddleState->moveLeft = input->left;
            paddleState->moveRight = input->right;
        }
        else if (paddleState->flags & PADDLE_FLAG_ORIENTATION_TOP)
        {
            paddleState->moveLeft = input->right;
            paddleState->moveRight = input->left;
        }
        else if (paddleState->flags & PADDLE_FLAG_ORIENTATION_LEFT)
        {
            paddleState->moveLeft = input->left;
            paddleState->moveRight = input->right;
        }
        else if (paddleState->flags & PADDLE_FLAG_ORIENTATION_RIGHT)
        {
            paddleState->moveLeft = input->right;
            paddleState->moveRight = input->left;
        }

        paddleState->releaseBall = input->space;
    }
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

void drawRect(SDL_Renderer *renderer, GameState *gameState, Rect rectangle, Vec2 offset = vec2(0.0f, 0.0f), Uint8 r = 0, Uint8 g = 0, Uint8 b = 0, Uint8 a = 255)
{
    SDL_Rect sdlRect;

    Vec2 rectDimInPixels = getRectangleDim(rectangle);

    sdlRect.w = rectDimInPixels.x;
    sdlRect.h = rectDimInPixels.y;

    sdlRect.x = offset.x + rectangle.bottomLeftCorner.x;
    sdlRect.y = gameState->screenDimensions.y - (rectangle.topRightCorner.y + offset.y);

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawRect(renderer, &sdlRect);
}

void addEntities(GameState *gameState)
{
    Entity *nullEntity = gameState->entities + gameState->entityCount++;

    nullEntity->storageIndex = gameState->entityCount - 1;
    nullEntity->type = ENTITY_TYPE_NULL;

    SDL_assert(nullEntity->storageIndex == 0);

    float epsilon = 0.01f;
    Vec2 paddlePos = vec2(
        (gameState->tilesPerWidth / 2) * gameState->tileSideInMeters,
        (gameState->tileSideInMeters * 1.5f) + epsilon);

    Entity *bottomPaddleEntity = addPaddleEntity(gameState, paddlePos);
    Entity *ball = addBallEntity(gameState, vec2(0.0f, 0.0f), 0.25f);
    Entity *brick = addBrickEntity(gameState, vec2(3.25f, 3.25f), vec2(0.5f, 0.5f));
    anchorBallToPaddle(ball, bottomPaddleEntity);

    addObstacleEntity(gameState, vec2(0.25f, 3.75f), vec2(0.5f, 7.5f));
    addObstacleEntity(gameState, vec2(7.25f, 3.75f), vec2(0.5f, 7.5f));
    addObstacleEntity(gameState, vec2(3.75f, 7.25f), vec2(7.5f, 0.5f));
    addObstacleEntity(gameState, vec2(3.75f, 0.25f), vec2(7.5f, 0.5f));
}

Entity *getPaddleRefFromBallEntity(Entity *entity)
{
    SDL_assert(entity->type == ENTITY_TYPE_BALL);
    Entity *result = 0;
    if (entity->ballState)
    {
        if (entity->ballState->paddle)
        {
            result = entity->ballState->paddle;
            SDL_assert(result->type == ENTITY_TYPE_PADDLE);
            if (result->paddleState)
            {
                SDL_assert(result->paddleState->ball == entity);
            }
            else
            {
                SDL_TriggerBreakpoint();
            }
        }
    }
    return result;
}

extern "C" void gameUpdate(GameMemory *gameMemory, GameInput *gameInput, SDL_Renderer *renderer)
{
    SDL_assert(sizeof(GameState) <= gameMemory->permanentMemorySize);

    GameState *gameState = (GameState *)gameMemory->permanentMemory;

    if (!gameMemory->initialized)
    {
        //memory partitioning
        initializeMemoryPartition(
            &gameState->entityMemory,
            gameMemory->permanentMemorySize - sizeof(GameState),
            (Uint8 *)gameMemory->permanentMemory + sizeof(GameState));

        //load assets

        //create world
        gameState->tileSideInMeters = 0.5f;
        gameState->worldDimensions = vec2(7.5f, 7.5f);
        gameState->tilesPerWidth = gameState->worldDimensions.x / gameState->tileSideInMeters;
        gameState->tilesPerHeight = gameState->worldDimensions.y / gameState->tileSideInMeters;

        gameState->tileSideInPixels = 32;
        gameState->metersToPixels = SDL_static_cast(float, gameState->tileSideInPixels) / gameState->tileSideInMeters;

        //create renderer
        int screenWidth, screenHeight;
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

        gameState->screenDimensions = vec2(screenWidth, screenHeight);
        gameState->worldInScreenSpace = rectFromDimAndCenter(
            gameState->worldDimensions * gameState->metersToPixels,
            vec2(screenWidth * 0.5f, screenHeight * 0.5f));
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        //add entities
        addEntities(gameState);

        gameMemory->initialized = true;
    }

    //update entities

    for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
    {
        Entity *entity = gameState->entities + entityIndex;
        SDL_assert(entity->storageIndex > 0);

        MovementSpecs specs = defaultMovementSpecs();
        CollisionSpecs collisionSpecs = defaultCollisionSpecs(entity->p);

        Vec2 ddp = vec2(0.0f, 0.0f);

        switch (entity->type)
        {
        case ENTITY_TYPE_PADDLE:
        {
            setPaddleInput(gameInput, entity);

            SDL_assert(entity->paddleState);
            if (entity->paddleState->flags & (PADDLE_FLAG_ORIENTATION_TOP | PADDLE_FLAG_ORIENTATION_BOTTOM))
            {
                if (entity->paddleState->moveLeft)
                {
                    ddp.x = -1.0;
                }
                else if (entity->paddleState->moveRight)
                {
                    ddp.x = 1.0;
                }
                //entity->w = getPaddleSize(paddle->powerUps);
            }

            //TODO: use proper physics constants
            specs.drag = 2.0f;
            specs.speed = 20.0f;
        }
        break;
        case ENTITY_TYPE_BALL:
        {
            specs.drag = 2.0f;
            specs.speed = 10.0f;

            SDL_assert(entity->ballState);

            Entity *anchoredPaddle = getPaddleRefFromBallEntity(entity);
            if (anchoredPaddle)
            {
                PaddleState *paddleState = anchoredPaddle->paddleState;
                if (paddleState->releaseBall)
                {
                    Vec2 launchForce;

                    if (paddleState->flags & PADDLE_FLAG_ORIENTATION_BOTTOM)
                    {
                        ddp.y = 1.0f;
                        launchForce = vec2(0.0f, 5.0f);
                    }
                    else if (paddleState->flags & PADDLE_FLAG_ORIENTATION_TOP)
                    {
                        ddp.y = -1.0f;
                        launchForce = vec2(0.0f, -5.0f);
                    }
                    else if (paddleState->flags & PADDLE_FLAG_ORIENTATION_LEFT)
                    {
                        ddp.x = 1.0f;
                        launchForce = vec2(5.0f, 0.0f);
                    }
                    else if (paddleState->flags & PADDLE_FLAG_ORIENTATION_RIGHT)
                    {
                        ddp.x = -1.0f;
                        launchForce = vec2(-5.0f, 0.0f);
                    }

                    entity->dp = anchoredPaddle->dp + launchForce;

                    entity->ballState->paddle = 0;
                    paddleState->ball = 0;
                }
                else
                {
                    entity->dp = anchoredPaddle->dp;
                }
            }
            else
            {
                ddp = normalize(entity->dp);
            }
        }
        break;
        case ENTITY_TYPE_BRICK:
        {
            SDL_assert(entity->brickState);
            BrickState *brickState = entity->brickState;
            if (brickState->collidedBall)
            {
                if (--brickState->hitPoints == 0)
                {
                    clearFlag(entity, ENTITY_FLAG_COLLIDES);
                }
                brickState->collidedBall = 0;
            }
        }
        break;
        case ENTITY_TYPE_OBSTACLE:
        default:
        {
        }
        break;
        }

        if (!isSet(entity, ENTITY_FLAG_STATIC))
        {
            ddp *= specs.speed;
            ddp += -specs.drag * entity->dp;

            Vec2 movementDelta = (0.5f * ddp * square(gameInput->deltaTime) + entity->dp * gameInput->deltaTime);

            collisionSpecs.deltaLength = length(movementDelta);
            collisionSpecs.desiredDp = ddp * gameInput->deltaTime + entity->dp;
            collisionSpecs.desiredP = entity->p + movementDelta;
        }

        Uint32 iterationCount = 4;
        for (Uint32 iteration = 0; iteration <= iterationCount; ++iteration)
        {
            bool hit = false;

            float t = iteration * (1.0f / iterationCount);
            Vec2 entityLerpP = ((1.0f - t) * collisionSpecs.oldP) + (t * collisionSpecs.desiredP);

            for (Uint32 testEntityIndex = 1; testEntityIndex < gameState->entityCount; ++testEntityIndex)
            {
                Entity *testEntity = gameState->entities + testEntityIndex;

                if (testEntity->storageIndex == entity->storageIndex ||
                    !isSet(testEntity, ENTITY_FLAG_COLLIDES) ||
                    !isSet(entity, ENTITY_FLAG_COLLIDES))
                {
                    continue;
                }

                Rect entityBounds = rectFromDimAndCenter(entity->dimensions, entityLerpP);
                Rect testEntityBounds = rectFromDimAndCenter(testEntity->dimensions, testEntity->p);

                if (rectsOverlap(entityBounds, testEntityBounds))
                {
                    float remainingDistance = (1.0f - t) * collisionSpecs.deltaLength;
                    if (remainingDistance == 0.0f && iteration == iterationCount)
                    {
                        remainingDistance = collisionSpecs.deltaLength * (1.0f / iterationCount);
                    }

                    if ((entity->type == ENTITY_TYPE_PADDLE || entity->type == ENTITY_TYPE_BALL) && testEntity->type == ENTITY_TYPE_OBSTACLE)
                    {
                        Vec2 wallNorm = getSurfaceNorm(collisionSpecs.desiredP, testEntityBounds);

                        collisionSpecs.oldP = collisionSpecs.desiredP;
                        collisionSpecs.desiredP = entityLerpP + (remainingDistance * wallNorm);
                        collisionSpecs.desiredDp = reflect(entity->dp, wallNorm);
                        collisionSpecs.deltaLength = remainingDistance;
                    }
                    else if (entity->type == ENTITY_TYPE_BALL && testEntity->type == ENTITY_TYPE_PADDLE)
                    {
                        float extraForceLength = length(testEntity->dp);

                        Vec2 norm = normalize(collisionSpecs.desiredP - testEntity->p);

                        collisionSpecs.oldP = collisionSpecs.desiredP;
                        collisionSpecs.desiredP = entityLerpP + (remainingDistance * norm);
                        collisionSpecs.desiredDp = (norm * length(entity->dp)) + (norm * extraForceLength * 0.9f);
                    }
                    else if (entity->type == ENTITY_TYPE_PADDLE && testEntity->type == ENTITY_TYPE_BALL)
                    {
                        float extraForceLength = length(entity->dp);

                        Vec2 norm = normalize(entity->p - collisionSpecs.desiredP);

                        PaddleState *paddleState = entity->paddleState;
                        SDL_assert(paddleState);
                        if (paddleState->flags & (PADDLE_FLAG_ORIENTATION_TOP | PADDLE_FLAG_ORIENTATION_BOTTOM))
                        {
                            norm.y = 0.0f;
                        }
                        else
                        {
                            norm.x = 0.0f;
                        }

                        collisionSpecs.oldP = collisionSpecs.desiredP;
                        collisionSpecs.desiredP = entity->p + (remainingDistance * norm);
                        collisionSpecs.desiredDp = norm * extraForceLength * 0.1f;
                    }
                    else if (entity->type == ENTITY_TYPE_BALL && testEntity->type == ENTITY_TYPE_BRICK)
                    {
                        Vec2 norm = getSurfaceNorm(collisionSpecs.desiredP, testEntityBounds);

                        collisionSpecs.oldP = collisionSpecs.desiredP;
                        collisionSpecs.desiredP = entityLerpP + (remainingDistance * norm);
                        collisionSpecs.desiredDp = reflect(entity->dp, norm);

                        BrickState *brickState = testEntity->brickState;
                        SDL_assert(brickState);
                        brickState->collidedBall = entity;
                    }
                }
            }
        }

        entity->p = collisionSpecs.desiredP;
        entity->dp = collisionSpecs.desiredDp;
    }

    //rendering
    beginDraw(renderer);

    drawRect(renderer, gameState, gameState->worldInScreenSpace);

    Vec2 tileDims = vec2(gameState->tileSideInMeters, gameState->tileSideInMeters) * gameState->metersToPixels;

    for (Uint32 horizontalTileIndex = 0; horizontalTileIndex < gameState->tilesPerWidth; ++horizontalTileIndex)
    {
        for (Uint32 verticalTileIndex = 0; verticalTileIndex < gameState->tilesPerHeight; ++verticalTileIndex)
        {
            float offsetX = tileDims.x * 0.5f;
            float offsetY = tileDims.y * 0.5f;

            Rect tile = rectFromDimAndCenter(
                tileDims,
                vec2(offsetX + (tileDims.x * horizontalTileIndex), offsetY + (tileDims.y * verticalTileIndex)));

            drawRect(renderer, gameState, tile, gameState->worldInScreenSpace.bottomLeftCorner, 255, 255, 255, 31);
        }
    }

    for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
    {
        Entity *entity = gameState->entities + entityIndex;
        Rect rect = rectFromDimAndCenter(entity->dimensions, entity->p);
        rect.bottomLeftCorner *= gameState->metersToPixels;
        rect.topRightCorner *= gameState->metersToPixels;

        drawRect(renderer, gameState, rect, gameState->worldInScreenSpace.bottomLeftCorner);
    }

    endDraw(renderer);
}