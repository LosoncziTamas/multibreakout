#include "multibreakout.h"

PaddleLogic *addPaddleLogic(Entity *paddleEntity, MemoryPartition *memoryPartition, Uint32 paddleFlags)
{
    SDL_assert(paddleEntity->type == ENTITY_TYPE_PADDLE);

    PaddleLogic *paddleLogic = pushStruct(memoryPartition, PaddleLogic);
    paddleLogic->entityIndex = paddleEntity->storageIndex;
    paddleLogic->flags = paddleFlags;

    bool swapDimensions = paddleLogic->flags & (PADDLE_FLAG_ORIENTATION_LEFT | PADDLE_FLAG_ORIENTATION_RIGHT);
    if (swapDimensions)
    {
        float tmp = paddleEntity->dimensions.y;
        paddleEntity->dimensions.y = paddleEntity->dimensions.x;
        paddleEntity->dimensions.x = tmp;
    }

    /*
    if (!(paddleLogic->flags & PADDLE_FLAG_PLAYER_CONTROLLED))
    {
        SDL_assert(SDL_arraysize(gameState->enemyControls) > gameState->enemyControlCount);
        
        EnemyControl* enemyControl = gameState->enemyControls + gameState->enemyControlCount++;
        enemyControl->paddleLogicIndex = gameState->paddleCount - 1;
        enemyControl->state = ENEMY_STATE_IDLE;
        enemyControl->target = Vec2();
        
        Rectangle rect = {};
        
        if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_TOP)
        {
            rect.bottomLeft = Vec2(160, SCREEN_HEIGHT * 0.5f);
            rect.topRight = Vec2(640, SCREEN_HEIGHT);
        }
        else if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_LEFT)
        {
            rect.bottomLeft = Vec2(160, 0);
            rect.topRight = Vec2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT);
        }
        else if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_RIGHT)
        {
            rect.bottomLeft = Vec2(SCREEN_WIDTH * 0.5f, 0);
            rect.topRight = Vec2(640, SCREEN_HEIGHT);
        }
        
        enemyControl->dangerZone = rect;
    }*/

    return paddleLogic;
}

Entity *addPaddleEntity(GameState *gameState, Vec2 pos)
{
    SDL_assert(SDL_arraysize(gameState->entities) > gameState->entityCount);

    Entity *paddle = gameState->entities + gameState->entityCount++;
    paddle->storageIndex = gameState->entityCount - 1;
    paddle->p = pos;
    paddle->dimensions = vec2(1.5f, 0.5f);
    paddle->dp = Vec2();
    paddle->type = ENTITY_TYPE_PADDLE;
    paddle->paddleLogic = addPaddleLogic(paddle, 
        &gameState->entityMemory, 
        PADDLE_FLAG_ORIENTATION_BOTTOM | PADDLE_FLAG_PLAYER_CONTROLLED);

    setFlag(paddle, ENTITY_FLAG_COLLIDES);

    return paddle;
}

void updatePaddles(GameInput *input, GameState *gameState)
{
    /*for (Uint32 paddleIndex = 0; paddleIndex < gameState->paddleCount; ++paddleIndex)
    {
        PaddleLogic* paddle = gameState->paddles + paddleIndex;
        if (paddle->flags & PADDLE_FLAG_PLAYER_CONTROLLED)
        {
            if (paddle->flags & PADDLE_FLAG_ORIENTATION_BOTTOM)
            {
                paddle->moveLeft = input->left;
                paddle->moveRight = input->right;
            }
            else if (paddle->flags & PADDLE_FLAG_ORIENTATION_TOP)
            {
                paddle->moveLeft = input->right;
                paddle->moveRight = input->left;
            }
            else if (paddle->flags & PADDLE_FLAG_ORIENTATION_LEFT)
            {
                paddle->moveLeft = input->left;
                paddle->moveRight = input->right;
            }
            else if (paddle->flags & PADDLE_FLAG_ORIENTATION_RIGHT)
            {
                paddle->moveLeft = input->right;
                paddle->moveRight = input->left;
            }
            
            paddle->releaseBall = input->space;
        }*/

    /*else
        {
            EnemyControl* enemyControl = getEnemyControl(gameState, paddleIndex);
            SDL_assert(enemyControl);
            
            Entity* enemyEntity = gameState->entities + paddle->entityIndex;
            
            paddle->moveLeft = false;
            paddle->moveRight = false;
            
            bool verticallyMoving = paddle->flags & (PADDLE_FLAG_ORIENTATION_LEFT|PADDLE_FLAG_ORIENTATION_RIGHT);
            
            Entity* ballInDangerZone = getBallInDangerZone(enemyControl, paddle, gameState, enemyEntity->p);
            
            switch (enemyControl->state) {
                case ENEMY_STATE_IDLE:
                {
                    if (!ballInDangerZone)
                    {
                        //select to random target position
                        
                        if (verticallyMoving)
                        {
                            enemyControl->target.y = rand() % static_cast<int>(SCREEN_HEIGHT) + 1;
                        }
                        else
                        {
                            enemyControl->target.x = 160 + (rand() % static_cast<int>(SCREEN_HEIGHT) + 1);
                        }
                        
                        enemyControl->state = ENEMY_STATE_STEERING;
                        printf("ENEMY_STATE_STEERING \n");
                    }
                    else
                    {
                        enemyControl->state = ENEMY_STATE_DEFENDING;
                        printf("ENEMY_STATE_DEFENDING \n");
                    }
                    
                } break;
                case ENEMY_STATE_STEERING:
                {
                    float minDistance;
                    
                    if (verticallyMoving)
                    {
                        minDistance = enemyEntity->h * 0.45f;
                    }
                    else
                    {
                        minDistance = enemyEntity->w * 0.45f;
                    }
                    
                    if (ballInDangerZone)
                    {
                        enemyControl->state = ENEMY_STATE_DEFENDING;
                        printf("ENEMY_STATE_DEFENDING \n");
                    }
                    else
                    {
                        bool reachedTarget;
                        
                        if (verticallyMoving)
                        {
                            reachedTarget = SDL_fabs(enemyControl->target.y - enemyEntity->p.y) < minDistance;
                        }
                        else
                        {
                            reachedTarget = SDL_fabs(enemyControl->target.x - enemyEntity->p.x) < minDistance;
                        }
                        
                        if (reachedTarget)
                        {
                            if (paddle->ball)
                            {
                                paddle->releaseBall = true;
                                enemyControl->state = ENEMY_STATE_IDLE;
                            }
                            //release ball if has
                            //should it switch state?
                            printf("REACHED_TARGET \n");
                        }
                        else
                        {
                            if (verticallyMoving)
                            {
                                paddle->moveRight = enemyControl->target.y - enemyEntity->p.y < minDistance;
                                paddle->moveLeft = enemyControl->target.y - enemyEntity->p.y > minDistance;
                            }
                            else
                            {
                                paddle->moveLeft = enemyControl->target.x - enemyEntity->p.x < minDistance;
                                paddle->moveRight = enemyControl->target.x - enemyEntity->p.x > minDistance;
                            }
                            //move toward the target
                        }
                    }
                    
                } break;
                case ENEMY_STATE_DEFENDING:
                {
                    if (ballInDangerZone)
                    {
                        float minDistance;
                        
                        if (verticallyMoving)
                        {
                            minDistance = enemyEntity->h * 0.1f;
                            paddle->moveRight = ballInDangerZone->p.y - enemyEntity->p.y < minDistance;
                            paddle->moveLeft = ballInDangerZone->p.y - enemyEntity->p.y > minDistance;
                        }
                        else
                        {
                            minDistance = enemyEntity->w * 0.1f;
                            paddle->moveLeft = ballInDangerZone->p.x - enemyEntity->p.x < minDistance;
                            paddle->moveRight = ballInDangerZone->p.x - enemyEntity->p.x > minDistance;
                        }
                        
                        //move toward the ball
                    }
                    else
                    {
                        printf("ENEMY_STATE_IDLE \n");
                        enemyControl->state = ENEMY_STATE_IDLE;
                    }
                } break;
                default:
                    break;
            }
        }
    }*/
}

void addEntities(GameState *gameState)
{
    Entity *nullEntity = gameState->entities + gameState->entityCount++;

    nullEntity->storageIndex = gameState->entityCount - 1;
    nullEntity->type = ENTITY_TYPE_NULL;

    SDL_assert(nullEntity->storageIndex == 0);

    Vec2 paddlePos = vec2((gameState->tilesPerWidth / 2) * gameState->tileSideInMeters, gameState->tileSideInMeters * 0.5f);

    Entity *bottomPaddleEntity = addPaddleEntity(gameState, paddlePos);

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

/*
        InitializeArena(&GameState->WorldArena, Memory->PermanentStorageSize - sizeof(game_state),
                        (uint8 *)Memory->PermanentStorage + sizeof(game_state));

*/

extern "C" void gameUpdate(GameMemory *gameMemory, GameInput *gameInput, SDL_Renderer *renderer)
{
    SDL_assert(sizeof(GameState) <= gameMemory->permanentMemorySize);

    GameState *gameState = (GameState *)gameMemory->permanentMemory;

    if (!gameMemory->initialized)
    {
        //memory partitioning
        initializeMemoryPartition(&gameState->entityMemory, 
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
        gameState->worldInScreenSpace = rectFromDimAndCenter(gameState->worldDimensions * gameState->metersToPixels, vec2(screenWidth * 0.5f, screenHeight * 0.5f));
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        //add entities
        addEntities(gameState);

        gameMemory->initialized = true;
    }

    //process game input

    updatePaddles(gameInput, gameState);

    //update entities

    for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
    {
        Entity *entity = gameState->entities + entityIndex;
        SDL_assert(entity->storageIndex > 0);

        /*
        MovementSpecs* specs = movements + entityIndex;
        Vec2 ddp;
        */

        switch (entity->type)
        {
            case ENTITY_TYPE_PADDLE:
            {
                PaddleLogic* paddle = entity->paddleLogic;
                if (paddle)
                {
                    if (paddle->flags & (PADDLE_FLAG_ORIENTATION_TOP | PADDLE_FLAG_ORIENTATION_BOTTOM))
                    {
                        if (paddle->moveLeft)
                        {
                            //ddp.x = -1.0;
                        }
                        else if (paddle->moveRight)
                        {
                            //ddp.x = 1.0;
                        }
                        //entity->w = getPaddleSize(paddle->powerUps);
                    }
                }
                //SDL_assert(paddle);
            } break;
            default:
            {
                
            } break;
        }
    }

    //rendering
    beginDraw(renderer);

    Entity *paddle = &gameState->entities[1];
    Rect paddleRect = rectFromDimAndCenter(paddle->dimensions, paddle->p);
    paddleRect.bottomLeftCorner = paddleRect.bottomLeftCorner * gameState->metersToPixels;
    paddleRect.topRightCorner = paddleRect.topRightCorner * gameState->metersToPixels;

    drawRect(renderer, gameState, paddleRect, gameState->worldInScreenSpace.bottomLeftCorner);
    drawRect(renderer, gameState, gameState->worldInScreenSpace);

    Vec2 tileDims = vec2(gameState->tileSideInMeters, gameState->tileSideInMeters) * gameState->metersToPixels;

    for (Uint32 horizontalTileIndex = 0; horizontalTileIndex < gameState->tilesPerWidth; ++horizontalTileIndex)
    {
        for (Uint32 verticalTileIndex = 0; verticalTileIndex < gameState->tilesPerHeight; ++verticalTileIndex)
        {
            float offsetX = tileDims.x * 0.5f;
            float offsetY = tileDims.y * 0.5f;

            Rect tile = rectFromDimAndCenter(tileDims,
                                             vec2(offsetX + (tileDims.x * horizontalTileIndex), offsetY + (tileDims.y * verticalTileIndex)));

            drawRect(renderer, gameState, tile, gameState->worldInScreenSpace.bottomLeftCorner, 255, 255, 255, 31);
        }
    }

    endDraw(renderer);
}