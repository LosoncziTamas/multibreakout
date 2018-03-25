#include "multibreakout_entity.h"
#include "multibreakout_memory.h"
#include "multibreakout.h"

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

Entity *addObstacleEntity(GameState *gameState, Vec2 pos, Vec2 dimensions)
{
    SDL_assert(SDL_arraysize(gameState->entities) > gameState->entityCount);

    Entity *obstacle = gameState->entities + gameState->entityCount++;

    obstacle->storageIndex = gameState->entityCount - 1;
    obstacle->p = pos;
    obstacle->dimensions = dimensions;
    obstacle->dp = Vec2();
    obstacle->type = ENTITY_TYPE_OBSTACLE;

    setFlag(obstacle, ENTITY_FLAG_STATIC | ENTITY_FLAG_COLLIDES);

    return obstacle;
}

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
    paddle->paddleLogic = addPaddleLogic(
        paddle,
        &gameState->entityMemory,
        PADDLE_FLAG_ORIENTATION_BOTTOM | PADDLE_FLAG_PLAYER_CONTROLLED);

    setFlag(paddle, ENTITY_FLAG_COLLIDES);

    return paddle;
}

CollisionSpecs defaultCollisionSpecs(Vec2 pos)
{
    CollisionSpecs collisionSpecs = {};

    collisionSpecs.desiredP = pos;
    collisionSpecs.desiredDp = vec2(0.0f, 0.0f);
    collisionSpecs.oldP = pos;
    collisionSpecs.deltaLength = 0.0f;

    return collisionSpecs;
}

MovementSpecs defaultMovementSpecs()
{
    MovementSpecs specs = {};

    specs.speed = 0.0f;
    specs.drag = 0.0f;

    return specs;
}

//TODO: FIX THIS
Vec2 getSurfaceNorm(Vec2 vector, Rect surfaceRect)
{
    float left = vector.x <= surfaceRect.bottomLeftCorner.x;
    float right = vector.x >= surfaceRect.topRightCorner.x;
    float top = vector.y >= surfaceRect.topRightCorner.y;
    float bottom = vector.y <= surfaceRect.bottomLeftCorner.y;
    
    Vec2 result = vec2(0.0f, 0.0f);
    Uint32 sideCount = 0;
    
    if (right)
    {
        result = vec2(1.0f, 0.0f);
        ++sideCount;
    }
    
    if (left)
    {
        result = vec2(-1.0f, 0.0f);
        ++sideCount;
    }
    
    if (top)
    {
        result = vec2(0.0f, 1.0f);
        ++sideCount;
    }
    
    if (bottom)
    {
        result = vec2(0.0f, -1.0f);
        ++sideCount;
    }
    
    SDL_assert(!(left && right) || !(top && bottom));
    SDL_assert(sideCount < 3);
    
    /*if (top && right)
    {
        result = (vector - surfaceRect->topRight).normalize();
    }
    
    if (bottom && right)
    {
        Vec2 bottomRight(surfaceRect->topRight.x, surfaceRect->bottomLeft.y);
        result = (vector - bottomRight).normalize();
    }
    
    if (top && left)
    {
        Vec2 topLeft(surfaceRect->bottomLeft.x, surfaceRect->topRight.y);
        result = (vector - topLeft).normalize();
    }
    
    if (bottom && left)
    {
        result = (vector - surfaceRect->bottomLeft).normalize();
    }*/
    
    return result;
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