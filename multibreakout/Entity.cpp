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

Entity* addBall(GameState *gameState)
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

    return ball;
}

Entity* addPaddle(GameState* gameState, Vec2 pos, Uint32 paddleFlags)
{
    SDL_assert(SDL_arraysize(gameState->entities) > gameState->entityCount);
    Entity* paddle = gameState->entities + gameState->entityCount++;
    
    paddle->storageIndex = gameState->entityCount - 1;
    paddle->w = 90.0f;
    paddle->h = 25.0f;
    paddle->p = pos;
    paddle->dp = Vec2();
    paddle->type = ENTITY_TYPE_PADDLE;
    
    setFlag(paddle, ENTITY_FLAG_COLLIDES);
    
    SDL_assert(SDL_arraysize(gameState->paddles) > gameState->paddleCount);
    PaddleLogic* logic = gameState->paddles + gameState->paddleCount++;
    
    logic->entityIndex = paddle->storageIndex;
    logic->flags = paddleFlags;
    
    if (!(logic->flags & PADDLE_FLAG_PLAYER_CONTROLLED))
    {
        SDL_assert(SDL_arraysize(gameState->enemyControls) > gameState->enemyControlCount);
        EnemyControl* enemyControl = gameState->enemyControls + gameState->enemyControlCount++;
        
        enemyControl->paddleLogicIndex = gameState->paddleCount - 1;
        enemyControl->state = ENEMY_STATE_IDLE;
        enemyControl->target = Vec2();
        
        Rectangle rect = {};
        rect.bottomLeft = Vec2(160, SCREEN_HEIGHT * 0.5f);
        rect.topRight = Vec2(640, SCREEN_HEIGHT);
        
        enemyControl->dangerZone = rect;
    }
    
    return paddle;
}

PaddleLogic* getLogicForPaddle(GameState* gameState, Uint32 entityIndex)
{
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
            
            paddle->releaseBall = gameState->input.space;
        }
        else
        {
            EnemyControl* enemyControl = getEnemyControl(gameState, paddleIndex);
            SDL_assert(enemyControl);

            Entity* enemyEntity = gameState->entities + paddle->entityIndex;
            
            paddle->moveLeft = false;
            paddle->moveRight = false;
            
            for (Uint32 entityIndex = 1; entityIndex < gameState->entityCount; ++entityIndex)
            {
                Entity* entity = gameState->entities + entityIndex;
                //NOTE: this implementation implies there is only one ball
                if (entity->type == ENTITY_TYPE_BALL && entity != paddle->ball)
                {
                    bool danger = isInRectangle(enemyControl->dangerZone, entity->p);
                    switch (enemyControl->state) {
                        case ENEMY_STATE_IDLE:
                        {
                            if (!danger)
                            {
                                //select to random target position
                                enemyControl->target.x = SCREEN_WIDTH * 0.5f;
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
                            float minDistance = enemyEntity->w * 0.45f;

                            if (danger)
                            {
                                enemyControl->state = ENEMY_STATE_DEFENDING;
                                printf("ENEMY_STATE_DEFENDING \n");
                            }
                            else
                            {
                                bool reachedTarget = SDL_fabs(enemyControl->target.x - enemyEntity->p.x) < minDistance;
                                if (reachedTarget)
                                {
                                    //release ball if has
                                    //should it switch state?
                                    printf("REACHED_TARGET \n");
                                }
                                else
                                {
                                    paddle->moveLeft = enemyControl->target.x - enemyEntity->p.x < minDistance;
                                    paddle->moveRight = enemyControl->target.x - enemyEntity->p.x > minDistance;
                                    //move toward the target
                                }
                            }

                        } break;
                        case ENEMY_STATE_DEFENDING:
                        {
                            if (danger)
                            {
                                float minDistance = enemyEntity->w * 0.45f;
                                paddle->moveLeft = entity->p.x - enemyEntity->p.x < minDistance;
                                paddle->moveRight = entity->p.x - enemyEntity->p.x > minDistance;
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

#if 0
            if (--brickLogic->hitPoints == 0)
            {
                printf("destroy brick \n");
            }
#endif
            brickLogic->collidedBall = 0;
        }
    }

}

void updateBalls(GameState* gameState)
{
    for (Uint32 ballIndex = 0; ballIndex < gameState->ballCount; ++ballIndex)
    {
        BallLogic* ballLogic = gameState->balls + ballIndex;
        if (ballLogic->collidedPaddle)
        {
            ballLogic->collidedPaddle->powerUp = ballLogic->powerUp;
            printf("Paddle powerup updated \n");

            ballLogic->powerUp = POWER_UP_NONE;
            ballLogic->collidedPaddle = 0;
        }
    }
}

BallLogic* getBallLogic(GameState* gameState, Uint32 ballEntityIndex)
{
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

BrickLogic* getBrickLogic(GameState* gameState, Uint32 brickEntityIndex)
{
    BrickLogic* brickLogic = 0;
    
    for (Uint32 brickLogicIndex = 0; brickLogicIndex < gameState->ballCount; ++brickLogicIndex)
    {
        brickLogic = gameState->bricks + brickLogicIndex;
        if (brickLogic->entityIndex == brickLogicIndex)
        {
            break;
        }
    }
    
    return brickLogic;
}

Entity* addBrick(GameState *gameState, Vec2 pos, float brickWidth, float brickHeight)
{
    SDL_assert(SDL_arraysize(gameState->entities) > gameState->entityCount);
    Entity* brick = gameState->entities + gameState->entityCount++;
    
    brick->p = pos;
    brick->w = brickWidth;
    brick->h = brickHeight;
    brick->type = ENTITY_TYPE_BRICK;
    brick->storageIndex = gameState->entityCount - 1;
    
    setFlag(brick, ENTITY_FLAG_STATIC|ENTITY_FLAG_COLLIDES);
    
    return brick;
}

void addEntities(GameState *gameState)
{
    Entity* nullEntity = gameState->entities + gameState->entityCount++;
    
    nullEntity->storageIndex = gameState->entityCount - 1;
    nullEntity->type = ENTITY_TYPE_NULL;
    
    SDL_assert(nullEntity->storageIndex == 0);
    
    float paddleHeight = DEFAULT_HEIGHT;
    
    addPaddle(gameState,
              Vec2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT - (21.0f + paddleHeight * 0.5f)),
              PADDLE_FLAG_ORIENTATION_TOP);
    
    Entity* player = addPaddle(gameState,
              Vec2(SCREEN_WIDTH * 0.5f, paddleHeight * 0.5f + 21.0f),
              PADDLE_FLAG_ORIENTATION_BOTTOM|PADDLE_FLAG_PLAYER_CONTROLLED);
    
    PaddleLogic* playerLogic = getLogicForPaddle(gameState, player->storageIndex);
    
    Entity* ball = addBall(gameState);

    SDL_assert(SDL_arraysize(gameState->balls) > gameState->ballCount);
    BallLogic* ballLogic = gameState->balls + gameState->ballCount++;
    
    ballLogic->entityIndex = ball->storageIndex;
    ballLogic->powerUp = POWER_UP_NONE;
    
    //TODO: extract to function
    playerLogic->ball = ball;
    ballLogic->paddle = player;
    
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
            Vec2 pos(pivotX + (columnIndex * brickWidth) + halfWidth,
                     pivotY - (rowIndex * brickHeight) + halfHeight);
            
            Entity* brick = addBrick(gameState, pos, brickWidth, brickHeight);
            
            SDL_assert(SDL_arraysize(gameState->bricks) > gameState->brickCount);
            BrickLogic* brickLogic = gameState->bricks + gameState->brickCount++;
            
            brickLogic->entityIndex = brick->storageIndex;
            brickLogic->hitPoints = 1;
            brickLogic->powerUp = POWER_UP_NONE;
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

float getPaddleSize(EntityPowerUp powerUp)
{
    switch (powerUp) {
        case POWER_UP_ENLARGE:
            return 120.0f;
        case POWER_UP_SHRINK:
            return 70.0f;
        default:
            return 90.0f;
    }
}

float getPaddleSpeed(EntityPowerUp powerUp)
{
    switch (powerUp) {
        case POWER_UP_ACCELERATE:
            return 700.0f;
        case POWER_UP_DECELERATE:
            return 250.0f;
        default:
            return 500.0f;
    }
}

void updateEntities(GameState *gameState)
{
    if (!initialized)
    {
        addEntities(gameState);
        addWalls(gameState);
        initialized = true;
    }
    
    updateBricks(gameState);
    updateBalls(gameState);
    updatePaddles(gameState);
    
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
                PaddleLogic* paddle = getLogicForPaddle(gameState, entityIndex);
                SDL_assert(paddle);
                specs->drag = 2.0f;
                specs->speed = getPaddleSpeed(paddle->powerUp);
                entity->w = getPaddleSize(paddle->powerUp);
                
                if (paddle->moveLeft)
                {
                    ddp.x = -1.0;
                }
                else if (paddle->moveRight)
                {
                    ddp.x = 1.0;
                }
            } break;
            case ENTITY_TYPE_BALL:
            {
                specs->speed = 200.0f;
                specs->drag = 2.0f;
                
                BallLogic* ballLogic = getBallLogic(gameState, entityIndex);
                SDL_assert(ballLogic);
                
                if (ballLogic->paddle)
                {
                    Entity* paddleEntity = ballLogic->paddle;
                    PaddleLogic* paddleLogic = getLogicForPaddle(gameState, paddleEntity->storageIndex);
                    SDL_assert(paddleLogic);
                    if (paddleLogic->releaseBall)
                    {
                        ddp.y = 1.0f;
                        Vec2 launchForce(0.0f, 250.0f);
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
                                testNorm.y = 0.0f;
                                testCollider->oldP = testCollider->desiredP;
                                testCollider->desiredP = testLerpP + (testRemainingDistance * testNorm);
                                testCollider->desiredDp = testNorm * extraForceLength * 0.1f;
                            }
                            
                            BallLogic* ballLogic = getBallLogic(gameState, entity->storageIndex);
                            SDL_assert(ballLogic);
                            ballLogic->collidedPaddle = getLogicForPaddle(gameState, test->storageIndex);
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
        
        drawEntityBounds(gameState->renderer, entity);
    }
    
    clearArray(colliders, gameState->entityCount, CollisionSpecs);
    clearArray(movements, gameState->entityCount, MovementSpecs);
    
    SDL_RenderPresent(gameState->renderer);
}