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
    topWall->p = Vec2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT + 10);
    topWall->dp = Vec2();
    topWall->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(topWall, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    Entity* bottomWall = gameState->entities + gameState->entityCount++;
    
    bottomWall->storageIndex = gameState->entityCount - 1;
    bottomWall->w = 480.0f;
    bottomWall->h = 20.0f;
    bottomWall->p = Vec2(SCREEN_WIDTH * 0.5f, -10);
    bottomWall->dp = Vec2();
    bottomWall->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(bottomWall, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
#if 0
    
    Entity* obstacle = gameState->entities + gameState->entityCount++;
    
    float obstacleDim = 30.0f;
    
    obstacle->storageIndex = gameState->entityCount - 1;
    obstacle->w = obstacleDim;
    obstacle->h = obstacleDim;
    obstacle->p = Vec2(160 + obstacleDim * 0.5f, obstacleDim * 0.5f);
    obstacle->dp = Vec2();
    obstacle->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(obstacle, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    obstacle = gameState->entities + gameState->entityCount++;
    
    obstacle->storageIndex = gameState->entityCount - 1;
    obstacle->w = obstacleDim;
    obstacle->h = obstacleDim;
    obstacle->p = Vec2(160 + obstacleDim * 0.5f, SCREEN_HEIGHT - obstacleDim * 0.5f);
    obstacle->dp = Vec2();
    obstacle->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(obstacle, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    obstacle = gameState->entities + gameState->entityCount++;
    
    obstacle->storageIndex = gameState->entityCount - 1;
    obstacle->w = obstacleDim;
    obstacle->h = obstacleDim;
    obstacle->p = Vec2(SCREEN_WIDTH - 160 - obstacleDim * 0.5f, obstacleDim * 0.5f);
    obstacle->dp = Vec2();
    obstacle->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(obstacle, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    obstacle = gameState->entities + gameState->entityCount++;
    
    obstacle->storageIndex = gameState->entityCount - 1;
    obstacle->w = obstacleDim;
    obstacle->h = obstacleDim;
    obstacle->p = Vec2(SCREEN_WIDTH - 160 - obstacleDim * 0.5f, SCREEN_HEIGHT - obstacleDim * 0.5f);
    obstacle->dp = Vec2();
    obstacle->type = ENTITY_TYPE_OBSTACLE;
    
    setFlag(obstacle, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
#endif
    
}

Entity* addBallEntity(GameState *gameState, Vec2 pos)
{
    Entity* ball = gameState->entities + gameState->entityCount++;
    
    ball->storageIndex = gameState->entityCount - 1;
    float radius = 10.0f;
    ball->w = radius * 2.0f;
    ball->h = radius * 2.0f;
    ball->p = pos;
    ball->dp = Vec2();
    ball->type = ENTITY_TYPE_BALL;
    
    setFlag(ball, ENTITY_FLAG_COLLIDES);
    
    return ball;
}

PaddleLogic* addPaddleLogic(Entity* paddleEntity, GameState* gameState, Uint32 paddleFlags)
{
    SDL_assert(paddleEntity->type == ENTITY_TYPE_PADDLE);
    SDL_assert(SDL_arraysize(gameState->paddles) > gameState->paddleCount);
    
    PaddleLogic* paddleLogic = gameState->paddles + gameState->paddleCount++;
    paddleLogic->entityIndex = paddleEntity->storageIndex;
    paddleLogic->flags = paddleFlags;
    
    bool swapDimensions = paddleLogic->flags & (PADDLE_FLAG_ORIENTATION_LEFT|PADDLE_FLAG_ORIENTATION_RIGHT);
    if (swapDimensions)
    {
        float tmp = paddleEntity->h;
        paddleEntity->h = paddleEntity->w;
        paddleEntity->w = tmp;
    }

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
    }
    
    return paddleLogic;
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

PaddleLogic* getPaddleLogic(GameState* gameState, Uint32 entityIndex)
{
    SDL_assert(gameState->entities[entityIndex].type == ENTITY_TYPE_PADDLE);
    
    PaddleLogic* paddle = 0;
    
    for (Uint32 paddleIndex = 0; paddleIndex < gameState->paddleCount; ++paddleIndex)
    {
        paddle = gameState->paddles + paddleIndex;
        if (paddle->entityIndex == entityIndex)
        {
            break;
        }
    }
    
    return paddle;
}

EnemyControl* getEnemyControl(GameState* gameState, Uint32 paddleLogicIndex)
{
    EnemyControl* enemyControl = 0;
    
    for (Uint32 enemyControlIndex = 0; enemyControlIndex < gameState->enemyControlCount; ++enemyControlIndex)
    {
        enemyControl = gameState->enemyControls + enemyControlIndex;
        if (enemyControl->paddleLogicIndex == paddleLogicIndex)
        {
            break;
        }
    }
    
    return enemyControl;
}

float getPitfallDistance(Vec2 enemyPos, Vec2 ballPos, PaddleLogic* enemyLogic)
{
    bool verticallyMoving = enemyLogic->flags & (PADDLE_FLAG_ORIENTATION_LEFT|PADDLE_FLAG_ORIENTATION_RIGHT);
    
    if (verticallyMoving)
    {
        return SDL_fabs(enemyPos.x - ballPos.x);
    }
    else
    {
        return SDL_fabs(enemyPos.y - ballPos.y);
    }
}

BallLogic* getBallLogic(GameState* gameState, Uint32 ballEntityIndex)
{
    SDL_assert(gameState->entities[ballEntityIndex].type == ENTITY_TYPE_BALL);
    
    BallLogic* ballLogic = 0;
    
    for (Uint32 ballLogicIndex = 0; ballLogicIndex < gameState->ballCount; ++ballLogicIndex)
    {
        ballLogic = gameState->balls + ballLogicIndex;
        if (ballLogic->entityIndex == ballEntityIndex)
        {
            break;
        }
    }
    
    return ballLogic;
}

Entity* getBallInDangerZone(EnemyControl* enemyControl, PaddleLogic* enemyLogic, GameState* gameState, Vec2 enemyPos)
{
    Entity* ball = 0;
    
    for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
    {
        Entity* entity = gameState->entities + entityIndex;
        if (entity->type == ENTITY_TYPE_BALL)
        {
            BallLogic* ballLogic = getBallLogic(gameState, entityIndex);
            
            if (!ballLogic->paddle && isInRectangle(enemyControl->dangerZone, entity->p))
            {
                if (!ball)
                {
                    ball = entity;
                }
                else
                {
                    Entity* newBall = entity;
                    
                    if (getPitfallDistance(enemyPos, newBall->p, enemyLogic) < getPitfallDistance(enemyPos, ball->p, enemyLogic))
                    {
                        ball = newBall;
                    }
                }
            }
        }
    }
    
    return ball;
    
}

void updatePaddles(GameState* gameState)
{
    for (Uint32 paddleIndex = 0; paddleIndex < gameState->paddleCount; ++paddleIndex)
    {
        PaddleLogic* paddle = gameState->paddles + paddleIndex;
        if (paddle->flags & PADDLE_FLAG_PLAYER_CONTROLLED)
        {
            if (paddle->flags & PADDLE_FLAG_ORIENTATION_BOTTOM)
            {
                paddle->moveLeft = gameState->input.left;
                paddle->moveRight = gameState->input.right;
            }
            else if (paddle->flags & PADDLE_FLAG_ORIENTATION_TOP)
            {
                paddle->moveLeft = gameState->input.right;
                paddle->moveRight = gameState->input.left;
            }
            else if (paddle->flags & PADDLE_FLAG_ORIENTATION_LEFT)
            {
                paddle->moveLeft = gameState->input.left;
                paddle->moveRight = gameState->input.right;
            }
            else if (paddle->flags & PADDLE_FLAG_ORIENTATION_RIGHT)
            {
                paddle->moveLeft = gameState->input.right;
                paddle->moveRight = gameState->input.left;
            }
            
            paddle->releaseBall = gameState->input.space;
        }
        else
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
    }
}

void updateBricks(GameState* gameState)
{
    for (Uint32 brickIndex = 0; brickIndex < gameState->brickCount; ++brickIndex)
    {
        BrickLogic* brickLogic = gameState->bricks + brickIndex;
        if (brickLogic->collidedBall)
        {
            brickLogic->collidedBall->powerUp = brickLogic->powerUp;
            printf("Ball powerup updated \n");
            
            if (--brickLogic->hitPoints == 0)
            {
                Entity* brickEntity = gameState->entities + brickLogic->entityIndex;
                SDL_assert(brickEntity->type == ENTITY_TYPE_BRICK);
                clearFlag(brickEntity, ENTITY_FLAG_COLLIDES);
                gameState->activeBrickCount--;
                printf("destroy brick \n");
            }
            brickLogic->collidedBall = 0;
        }
    }
}

void setPowerUpFlags(PaddleLogic* paddleLogic, Uint32 flags)
{
    paddleLogic->powerUps |= flags;
}

void clearPowerUpFlags(PaddleLogic* paddleLogic, Uint32 flags)
{
    paddleLogic->powerUps &= ~flags;
}

bool isSet(PaddleLogic* paddleLogic, Uint32 flags)
{
    return paddleLogic->powerUps & flags;
}

void setPaddlePowerUp(PaddleLogic* paddleLogic, EntityPowerUp newPowerUp)
{
    
    if (!paddleLogic->powerUps)
    {
        setPowerUpFlags(paddleLogic, newPowerUp);
    }
    else
    {
        if ((isSet(paddleLogic, POWER_UP_DECELERATE) && newPowerUp == POWER_UP_ACCELERATE) ||
            (isSet(paddleLogic, POWER_UP_ACCELERATE) && newPowerUp == POWER_UP_DECELERATE))
        {
            clearPowerUpFlags(paddleLogic, (POWER_UP_DECELERATE|POWER_UP_ACCELERATE));
            newPowerUp = POWER_UP_NONE;
        }
        if ((isSet(paddleLogic, POWER_UP_SHRINK) && newPowerUp == POWER_UP_ENLARGE) ||
            (isSet(paddleLogic, POWER_UP_ENLARGE) && newPowerUp == POWER_UP_SHRINK))
        {
            clearPowerUpFlags(paddleLogic, (POWER_UP_SHRINK|POWER_UP_ENLARGE));
            newPowerUp = POWER_UP_NONE;
        }
        
        setPowerUpFlags(paddleLogic, newPowerUp);
    }
    
    SDL_assert(!(isSet(paddleLogic, POWER_UP_SHRINK) && isSet(paddleLogic, POWER_UP_ENLARGE)));
    SDL_assert(!(isSet(paddleLogic, POWER_UP_ACCELERATE) && isSet(paddleLogic, POWER_UP_DECELERATE)));
    
    printf("Paddle powerup updated 0x%08x\n", paddleLogic->powerUps);
}


void updateBalls(GameState* gameState)
{
    for (Uint32 ballIndex = 0; ballIndex < gameState->ballCount; ++ballIndex)
    {
        BallLogic* ballLogic = gameState->balls + ballIndex;
        if (ballLogic->collidedPaddle)
        {
            if (ballLogic->powerUp)
            {
                setPaddlePowerUp(ballLogic->collidedPaddle, ballLogic->powerUp);
                ballLogic->powerUp = POWER_UP_NONE;
            }
            
            ballLogic->collidedPaddle = 0;
        }
    }
}



BrickLogic* getBrickLogic(GameState* gameState, Uint32 brickEntityIndex)
{
    SDL_assert(gameState->entities[brickEntityIndex].type == ENTITY_TYPE_BRICK);
    
    BrickLogic* brickLogic = 0;
    
    for (Uint32 brickLogicIndex = 0; brickLogicIndex < gameState->brickCount; ++brickLogicIndex)
    {
        brickLogic = gameState->bricks + brickLogicIndex;
        if (brickLogic->entityIndex == brickEntityIndex)
        {
            break;
        }
    }
    
    return brickLogic;
}

Entity* addBrickEntity(GameState *gameState, Vec2 pos, float brickWidth, float brickHeight)
{
    SDL_assert(SDL_arraysize(gameState->entities) > gameState->entityCount);
    Entity* brick = gameState->entities + gameState->entityCount++;
    
    brick->p = pos;
    brick->w = brickWidth;
    brick->h = brickHeight;
    brick->type = ENTITY_TYPE_BRICK;
    brick->storageIndex = gameState->entityCount - 1;
    
    setFlag(brick, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    gameState->activeBrickCount++;
    
    return brick;
}

void anchorBallToPaddle(Entity* ballEntity, Entity* paddleEntity, BallLogic* ballLogic, PaddleLogic* paddleLogic)
{
    SDL_assert(ballEntity->type == ENTITY_TYPE_BALL && paddleEntity->type == ENTITY_TYPE_PADDLE);
    
    float anchorOffset = 5.0f;
    
    if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_LEFT)
    {
        ballEntity->p = paddleEntity->p;
        ballEntity->p.x += paddleEntity->w * 0.5f + ballEntity->w * 0.5f + anchorOffset;
    }
    else if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_BOTTOM)
    {
        ballEntity->p = paddleEntity->p;
        ballEntity->p.y += paddleEntity->h * 0.5f + ballEntity->w * 0.5f + anchorOffset;
    }
    else if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_RIGHT)
    {
        ballEntity->p = paddleEntity->p;
        ballEntity->p.x -= paddleEntity->w * 0.5f + ballEntity->w * 0.5f + anchorOffset;
    }
    else if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_TOP)
    {
        ballEntity->p = paddleEntity->p;
        ballEntity->p.y -= paddleEntity->h * 0.5f + ballEntity->w * 0.5f + anchorOffset;
    }
    else
    {
        SDL_TriggerBreakpoint();
    }
    
    paddleLogic->ball = ballEntity;
    ballLogic->paddle = paddleEntity;
}

BallLogic* addBallLogic(Entity* ballEntity, GameState* gameState)
{
    SDL_assert(ballEntity->type == ENTITY_TYPE_BALL);
    SDL_assert(SDL_arraysize(gameState->balls) > gameState->ballCount);
    
    BallLogic* ballLogic = gameState->balls + gameState->ballCount++;
    ballLogic->entityIndex = ballEntity->storageIndex;
    ballLogic->powerUp = POWER_UP_NONE;
    
    return ballLogic;
}

void addEntities(GameState *gameState)
{
    Entity* nullEntity = gameState->entities + gameState->entityCount++;
    
    nullEntity->storageIndex = gameState->entityCount - 1;
    nullEntity->type = ENTITY_TYPE_NULL;
    
    SDL_assert(nullEntity->storageIndex == 0);
    
    float paddleHeight = DEFAULT_HEIGHT;
    
    {
        Entity* bottomPaddleEntity = addPaddleEntity(gameState, Vec2(SCREEN_WIDTH * 0.5f, paddleHeight * 0.5f));
        PaddleLogic* bottomPaddleLogic = addPaddleLogic(bottomPaddleEntity, gameState, PADDLE_FLAG_ORIENTATION_BOTTOM|PADDLE_FLAG_PLAYER_CONTROLLED);
        
        Entity* bottomBallEntity = addBallEntity(gameState, Vec2());
        BallLogic* bottomBallLogic = addBallLogic(bottomBallEntity, gameState);
        
        anchorBallToPaddle(bottomBallEntity, bottomPaddleEntity, bottomBallLogic, bottomPaddleLogic);
    }

#if 0
    {
        Entity* topPaddleEntity = addPaddleEntity(gameState, Vec2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT - paddleHeight * 0.5f));
        PaddleLogic* topPaddleLogic = addPaddleLogic(topPaddleEntity, gameState, PADDLE_FLAG_ORIENTATION_TOP);
        
        Entity* topBallEntity = addBallEntity(gameState, Vec2());
        BallLogic* topBallLogic = addBallLogic(topBallEntity, gameState);
        
        anchorBallToPaddle(topBallEntity, topPaddleEntity, topBallLogic, topPaddleLogic);
    }

    {
        Entity* leftPaddleEntity = addPaddleEntity(gameState, Vec2(161 + paddleHeight * 0.5f, SCREEN_HEIGHT * 0.5f));
        PaddleLogic* leftPaddleLogic = addPaddleLogic(leftPaddleEntity, gameState, PADDLE_FLAG_ORIENTATION_LEFT);
        
        Entity* leftBallEntity = addBallEntity(gameState, Vec2());
        BallLogic* leftBallLogic = addBallLogic(leftBallEntity, gameState);
        
        anchorBallToPaddle(leftBallEntity, leftPaddleEntity, leftBallLogic, leftPaddleLogic);
    }
    
    {
        Entity* rightPaddleEntity = addPaddleEntity(gameState, Vec2(639 - paddleHeight * 0.5f, SCREEN_HEIGHT * 0.5f));
        PaddleLogic* rightPaddleLogic = addPaddleLogic(rightPaddleEntity, gameState, PADDLE_FLAG_ORIENTATION_RIGHT);
        
        Entity* rightBallEntity = addBallEntity(gameState, Vec2());
        BallLogic* rightBallLogic = addBallLogic(rightBallEntity, gameState);
        
        anchorBallToPaddle(rightBallEntity, rightPaddleEntity, rightBallLogic, rightPaddleLogic);
    }
#endif
    
    Entity* projectile = gameState->entities + gameState->entityCount++;
    
    projectile->storageIndex = gameState->entityCount - 1;
    projectile->p = Vec2(400, 100);
    projectile->dp = Vec2(0.0f, 1.0f);
    projectile->w = 15.0f;
    projectile->h = 30.0f;
    projectile->type = ENTITY_TYPE_PROJECTILE;
    
    setFlag(projectile, ENTITY_FLAG_COLLIDES);
}

void addTestLevels(GameState *gameState)
{
    SDL_assert(SDL_arraysize(gameState->levels) > gameState->levelCount);
    
    Level* level = gameState->levels + gameState->levelCount++;
    
    Uint32 columns = 16;
    Uint32 rows = 4;
    
    SDL_assert(SDL_arraysize(level->components) >= columns * rows);
    
    float brickDim = 30.0f;
    Vec2 gameAreaTopLeft = Vec2(160.0f, 450.0f);
    
    //TODO: fix calc error here
    for (Uint32 rowIndex = 1; rowIndex <= rows; ++rowIndex)
    {
        for (Uint32 columnIndex = 1; columnIndex <= columns; ++columnIndex)
        {
            
            LevelComponent* levelComponent = level->components + columns * rowIndex + columnIndex;
            
            levelComponent->dim = Vec2(brickDim, brickDim);
            levelComponent->hitPoints = 1;
            levelComponent->powerUp = POWER_UP_NONE;
            levelComponent->pos = Vec2(gameAreaTopLeft.x + (brickDim * 0.5f) + (columnIndex - 1) * brickDim,
                                       gameAreaTopLeft.y - (brickDim * 0.5f) - (rowIndex - 1) * brickDim);
            
            level->componentCount++;
        }
    }
    
    SDL_assert(level->componentCount <= columns * rows);
}

void buildLevel(GameState* gameState)
{
    SDL_assert(SDL_arraysize(gameState->levels) > gameState->currentLevelIndex);

    Level* currentLevel = gameState->levels + gameState->currentLevelIndex;
    
    for (Uint32 componentIndex = 0; componentIndex < currentLevel->componentCount; ++componentIndex)
    {
        LevelComponent* levelComponent = currentLevel->components + componentIndex;
        
        Entity* brickEntity = addBrickEntity(gameState, levelComponent->pos, levelComponent->dim.x, levelComponent->dim.y);
        
        SDL_assert(SDL_arraysize(gameState->bricks) > gameState->brickCount);
        
        BrickLogic* brickLogic = gameState->bricks + gameState->brickCount++;
        brickLogic->entityIndex = brickEntity->storageIndex;
        brickLogic->hitPoints = levelComponent->hitPoints;
        brickLogic->powerUp = levelComponent->powerUp;
    }
}

struct CollisionSpecs
{
    Vec2 desiredP;
    Vec2 desiredDp;
    Vec2 oldP;
    float deltaLength;
};

void setStaticCollider(CollisionSpecs* collider, Vec2 center)
{
    SDL_assert(collider);
    
    collider->deltaLength = 0.0f;
    collider->desiredDp = Vec2();
    collider->desiredP = center;
    collider->oldP = center;
}

Vec2 getSurfaceNorm(Vec2 vector, Rectangle* surfaceRect)
{
    bool left = vector.x <= surfaceRect->bottomLeft.x;
    bool right = vector.x >= surfaceRect->topRight.x;
    bool top = vector.y >= surfaceRect->topRight.y;
    bool bottom = vector.y <= surfaceRect->bottomLeft.y;
    
    Vec2 result;
    
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

float getPaddleSize(Uint32 paddlePowerUps)
{
    if (paddlePowerUps & POWER_UP_ENLARGE)
    {
        return 120.0f;
    }
    else if (paddlePowerUps & POWER_UP_SHRINK)
    {
        return 70.0f;
    }
    else
    {
        return 90.0f;
    }
}

float getPaddleSpeed(Uint32 paddlePowerUps)
{
    if (paddlePowerUps & POWER_UP_ACCELERATE)
    {
        return 700.0f;
    }
    else if (paddlePowerUps & POWER_UP_DECELERATE)
    {
        return 250.0f;
    }
    else
    {
        return 500.0f;
    }
}

void setPowerUpColor(EntityPowerUp powerUp, Uint8* r, Uint8* g, Uint8* b)
{
    switch (powerUp) {
        case POWER_UP_ENLARGE:
            *g = 255;
            break;
        case POWER_UP_ACCELERATE:
            *b = 255;
            break;
        case POWER_UP_DECELERATE:
            *r = 255;
            break;
        case POWER_UP_SHRINK:
            *g = 255;
            *r = 255;
            break;
        case POWER_UP_NONE:
        default:
            break;
    }
}

void drawEntityBounds(SDL_Renderer* renderer, Entity* entity, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_Rect rect;
    rect.w = entity->w;
    rect.h = entity->h;
    rect.x = entity->p.x - (entity->w * 0.5f);
    rect.y = SCREEN_HEIGHT - (entity->p.y + entity->h * 0.5f);
    
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
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
        addTestLevels(gameState);
        addEntities(gameState);
        buildLevel(gameState);
        addWalls(gameState);
        initialized = true;
    }
    
    updateBricks(gameState);
    updateBalls(gameState);
    updatePaddles(gameState);
    
    if (gameState->activeBrickCount == 0)
    {
        clearArray(gameState->entities, gameState->entityCount, Entity);
        gameState->entityCount = 0;
        clearArray(gameState->paddles, gameState->paddleCount, PaddleLogic);
        gameState->paddleCount = 0;
        clearArray(gameState->enemyControls, gameState->enemyControlCount, EnemyControl);
        gameState->enemyControlCount = 0;
        clearArray(gameState->balls, gameState->ballCount, BallLogic);
        gameState->ballCount = 0;
        clearArray(gameState->bricks, gameState->brickCount, BrickLogic);
        gameState->brickCount = 0;
        
        addEntities(gameState);
        addWalls(gameState);
        
    }
    
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
                PaddleLogic* paddle = getPaddleLogic(gameState, entityIndex);
                SDL_assert(paddle);
                specs->drag = 2.0f;
                specs->speed = getPaddleSpeed(paddle->powerUps);
                if (paddle->flags & (PADDLE_FLAG_ORIENTATION_TOP|PADDLE_FLAG_ORIENTATION_BOTTOM))
                {
                    if (paddle->moveLeft)
                    {
                        ddp.x = -1.0;
                    }
                    else if (paddle->moveRight)
                    {
                        ddp.x = 1.0;
                    }
                    entity->w = getPaddleSize(paddle->powerUps);
                }
                else if (paddle->flags & (PADDLE_FLAG_ORIENTATION_LEFT|PADDLE_FLAG_ORIENTATION_RIGHT))
                {
                    if (paddle->moveLeft)
                    {
                        ddp.y = +1.0;
                    }
                    else if (paddle->moveRight)
                    {
                        ddp.y = -1.0;
                    }
                    entity->h = getPaddleSize(paddle->powerUps);
                }
                else
                {
                    SDL_TriggerBreakpoint();
                }
                
                
            } break;
            case ENTITY_TYPE_BALL:
            {
                specs->speed = 500.0f;
                specs->drag = 2.0f;
                
                BallLogic* ballLogic = getBallLogic(gameState, entityIndex);
                SDL_assert(ballLogic);
                
                if (ballLogic->paddle)
                {
                    Entity* paddleEntity = ballLogic->paddle;
                    PaddleLogic* paddleLogic = getPaddleLogic(gameState, paddleEntity->storageIndex);
                    SDL_assert(paddleLogic);
                    if (paddleLogic->releaseBall)
                    {
                        Vec2 launchForce;

                        if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_BOTTOM)
                        {
                            ddp.y = 1.0f;
                            launchForce = Vec2(0.0f, 250.0f);
                        }
                        else if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_TOP)
                        {
                            ddp.y = -1.0f;
                            launchForce = Vec2(0.0f, -250.0f);
                        }
                        else if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_LEFT)
                        {
                            ddp.x = 1.0f;
                            launchForce = Vec2(250.0f, 0.0f);
                        }
                        else if (paddleLogic->flags & PADDLE_FLAG_ORIENTATION_RIGHT)
                        {
                            ddp.x = -1.0f;
                            launchForce = Vec2(-250.0f, 0.0f);
                        }
    
                        entity->dp = paddleEntity->dp + launchForce;
                        
                        ballLogic->paddle = 0;
                        paddleLogic->ball = 0;
                    }
                    else
                    {
                        entity->dp = paddleEntity->dp;
                    }
                }
                else
                {
                    if (gameState->input.mouseRight)
                    {
                        Vec2 newVelocity(gameState->input.mouseX - entity->p.x,
                                         SCREEN_HEIGHT - gameState->input.mouseY - entity->p.y);
                        entity->dp = Vec2();
                        ddp = newVelocity.normalize();
                    }
                    else
                    {
                        Vec2 normalized(entity->dp);
                        ddp = normalized.normalize();
                    }
                }
                
                drawCircle(gameState->renderer, entity);
                
            } break;
            case ENTITY_TYPE_PROJECTILE:
            {
                specs->speed = 1000.0f;
                
                if (gameState->input.mouseLeft)
                {
                    entity->p = Vec2(gameState->input.mouseX,
                                     SCREEN_HEIGHT - gameState->input.mouseY);
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
                        Vec2 wallNorm = getSurfaceNorm(collider->desiredP, &testRect);
                        collider->oldP = collider->desiredP;
                        collider->desiredP = entityLerpP + (remainingDistance * wallNorm);
                        collider->desiredDp = reflect(entity->dp, wallNorm);
                    }
                    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_OBSTACLE)
                    {
                        Vec2 wallNorm = getSurfaceNorm(collider->desiredP, &testRect);
                        if (wallNorm.length() == 0)
                        {
                            wallNorm = (entity->p - collider->desiredP).normalize();
                        }
                        collider->oldP = collider->desiredP;
                        collider->desiredP = entityLerpP + (remainingDistance * wallNorm);
                        collider->desiredDp = reflect(entity->dp, wallNorm);
                    }
                    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_PADDLE)
                    {
                        if (circleRectIntersect(collider->desiredP, entity->w * 0.5f, testLerpP, test->w, test->h))
                        {
                            //TODO: implement better force distribution
                            float extraForceLength = test->dp.length();
                            
                            {
                                Vec2 norm = (collider->desiredP - testLerpP).normalize();
                                collider->oldP = collider->desiredP;
                                collider->desiredP = entityLerpP + (remainingDistance * norm);
                                collider->desiredDp = (norm * entity->dp.length()) + (norm * extraForceLength * 0.9f);
                            }
                            
                            {
                                Vec2 testNorm = (testLerpP - collider->desiredP).normalize();
                                PaddleLogic* paddleLogic = getPaddleLogic(gameState, test->storageIndex);
                                
                                if (paddleLogic->flags & (PADDLE_FLAG_ORIENTATION_TOP|PADDLE_FLAG_ORIENTATION_BOTTOM))
                                {
                                    testNorm.y = 0.0f;
                                }
                                else
                                {
                                    testNorm.x = 0.0f;
                                }
                                
                                testCollider->oldP = testCollider->desiredP;
                                testCollider->desiredP = testLerpP + (testRemainingDistance * testNorm);
                                testCollider->desiredDp = testNorm * extraForceLength * 0.1f;
                            }
                            
                            BallLogic* ballLogic = getBallLogic(gameState, entity->storageIndex);
                            SDL_assert(ballLogic);
                            ballLogic->collidedPaddle = getPaddleLogic(gameState, test->storageIndex);
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
                                collider->oldP = collider->desiredP;
                                collider->desiredP = entityLerpP + (remainingDistance * norm);
                                collider->desiredDp = norm * entity->dp.length();
                            }
                            
                            {
                                Vec2 testNorm = (testLerpP - entityLerpP).normalize();
                                testCollider->oldP = testCollider->desiredP;
                                testCollider->desiredP = testLerpP + (testRemainingDistance * testNorm);
                                testCollider->desiredDp = testNorm * test->dp.length();
                            }
                        }
                    }
                    else if (entity->type == ENTITY_TYPE_BALL && test->type == ENTITY_TYPE_BRICK)
                    {
                        if (circleRectIntersect(collider->desiredP, entity->w * 0.5f, testLerpP, test->w, test->h))
                        {
                            Vec2 norm = getSurfaceNorm(collider->desiredP, &testRect);
                            if (norm.length() == 0)
                            {
                                norm = (collider->desiredP - testLerpP).normalize();
                            }
                            collider->oldP = collider->desiredP;
                            collider->desiredP = entityLerpP + (remainingDistance * norm);
                            collider->desiredDp = reflect(entity->dp, norm);
                            
                            BrickLogic* brickLogic = getBrickLogic(gameState, test->storageIndex);
                            SDL_assert(brickLogic);
                            brickLogic->collidedBall = getBallLogic(gameState, entity->storageIndex);
                        }
                    }
                }
            }
        }
    }
    
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
        
        Uint8 r = 0;
        Uint8 g = 0;
        Uint8 b = 0;
        Uint8 a = 255;
        
        if (entity->type == ENTITY_TYPE_BRICK)
        {
            BrickLogic* brickLogic = getBrickLogic(gameState, entity->storageIndex);
            setPowerUpColor(brickLogic->powerUp, &r, &g, &b);
        }
        else if (entity->type == ENTITY_TYPE_BALL)
        {
            BallLogic* ballLogic = getBallLogic(gameState, entity->storageIndex);
            setPowerUpColor(ballLogic->powerUp, &r, &g, &b);
        }
        else if (entity->type == ENTITY_TYPE_PADDLE)
        {
            PaddleLogic* paddleLogic = getPaddleLogic(gameState, entity->storageIndex);
            
            if (isSet(paddleLogic, POWER_UP_ENLARGE))
            {
                g = 255;
            }
            if (isSet(paddleLogic, POWER_UP_ACCELERATE))
            {
                b = 255;
            }
            if (isSet(paddleLogic, POWER_UP_DECELERATE))
            {
                r = 255;
            }
            if (isSet(paddleLogic, POWER_UP_SHRINK))
            {
                g = 128;
            }
        }
        if (isSet(entity, ENTITY_FLAG_COLLIDES))
        {
            drawEntityBounds(gameState->renderer, entity, r, g, b, a);
        }
    }
    
    clearArray(colliders, gameState->entityCount, CollisionSpecs);
    clearArray(movements, gameState->entityCount, MovementSpecs);
    
    SDL_RenderPresent(gameState->renderer);
}