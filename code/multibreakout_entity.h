#ifndef MULTIBREAKOUT_ENTITY_H
#define MULTIBREAKOUT_ENTITY_H

#include <SDL2/SDL_stdinc.h>

#include "multibreakout_math.h"

enum EntityType
{
    ENTITY_TYPE_NULL,
    ENTITY_TYPE_BALL,
    ENTITY_TYPE_PADDLE,
    ENTITY_TYPE_BRICK,
    ENTITY_TYPE_OBSTACLE,
    ENTITY_TYPE_PROJECTILE
};

enum EntityFlags
{
    ENTITY_FLAG_COLLIDES = (1 << 0),
    ENTITY_FLAG_STATIC = (1 << 1)
};

enum EntityPowerUp
{
    POWER_UP_NONE = 0,
    POWER_UP_ENLARGE = (1 << 0),
    POWER_UP_SHRINK = (1 << 1),
    POWER_UP_ACCELERATE = (1 << 2),
    POWER_UP_DECELERATE = (1 << 3)
};

enum PaddleFlags
{
    PADDLE_FLAG_ORIENTATION_BOTTOM = (1 << 0),
    PADDLE_FLAG_ORIENTATION_TOP = (1 << 1),
    PADDLE_FLAG_ORIENTATION_LEFT = (1 << 2),
    PADDLE_FLAG_ORIENTATION_RIGHT = (1 << 3),
    PADDLE_FLAG_PLAYER_CONTROLLED = (1 << 4)
};

struct Entity;

struct PaddleAi
{
    enum State
    {
        IDLE,
        STEERING,
        DEFENDING
    } state;

    Rect dangerZone;
    Vec2 target;
};

struct PaddleState
{
    Uint32 entityIndex;
    Uint32 flags;
    Uint32 powerUps;
    bool moveLeft;
    bool moveRight;
    bool releaseBall;
    Entity *ball;
};

struct BallState
{
    Uint32 entityIndex;
    EntityPowerUp powerUp;
    Entity *paddle;
    //For now we assume that a ball can only interact with a paddle
    Entity *collidedPaddle;
};

struct BrickState
{
    Uint32 entityIndex;
    EntityPowerUp powerUp;
    Uint32 hitPoints;
    //For now we assume that a brick can only interact with a ball
    Entity *collidedBall;
};

struct Entity
{
    Uint32 storageIndex;
    Vec2 p;
    Vec2 dp;
    Vec2 dimensions;
    EntityType type;
    Uint32 flags;

    //Use union here?
    PaddleState *paddleState;
    BallState *ballState;
    BrickState *brickState;
};

struct MovementSpecs
{
    float speed;
    float drag;
};

struct CollisionSpecs
{
    Vec2 desiredP;
    Vec2 desiredDp;
    Vec2 oldP;
    float deltaLength;
};

void setFlag(Entity *entity, Uint32 flag);
bool isSet(Entity *entity, Uint32 flag);
void clearFlag(Entity *entity, Uint32 flag);
void anchorBallToPaddle(Entity *ballEntity, Entity *paddleEntity);

struct GameState;
Entity *addBallEntity(GameState *gameState, Vec2 pos, float radius);
Entity *addObstacleEntity(GameState *gameState, Vec2 pos, Vec2 dimensions);
Entity *addPaddleEntity(GameState *gameState, Vec2 pos);
Entity *addBrickEntity(GameState *gameState, Vec2 pos, Vec2 dimensions);

MovementSpecs defaultMovementSpecs();
CollisionSpecs defaultCollisionSpecs(Vec2 pos);
Vec2 getSurfaceNorm(Vec2 vector, Rect surfaceRect);

#endif