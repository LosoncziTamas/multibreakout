#ifndef Entity_hpp
#define Entity_hpp

#include <SDL2/SDL_stdinc.h>
#include "Vec2.hpp"
#include "Physics.hpp"

struct GameState;

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

struct Entity
{
    Uint32 storageIndex;
    Vec2 p;
    Vec2 dp;
    float w;
    float h;
    EntityType type;
    Uint32 flags;
};

struct MovementSpecs
{
    float speed;
    float drag;
};

enum PaddleFlags
{
    PADDLE_FLAG_ORIENTATION_BOTTOM = (1 << 0),
    PADDLE_FLAG_ORIENTATION_TOP = (1 << 1),
    PADDLE_FLAG_ORIENTATION_LEFT = (1 << 2),
    PADDLE_FLAG_ORIENTATION_RIGHT = (1 << 3),
    PADDLE_FLAG_PLAYER_CONTROLLED = (1 << 4)
};

enum EntityPowerUp
{
    POWER_UP_NONE = 0,
    POWER_UP_ENLARGE = (1 << 0),
    POWER_UP_SHRINK = (1 << 1),
    POWER_UP_ACCELERATE = (1 << 2),
    POWER_UP_DECELERATE = (1 << 3)
};

struct PaddleLogic
{
    Uint32 entityIndex;
    Uint32 flags;
    Uint32 powerUps;
    bool moveLeft;
    bool moveRight;
    bool releaseBall;
    Entity* ball;
};

struct BallLogic
{
    Uint32 entityIndex;
    Entity* paddle;
    EntityPowerUp powerUp;
    PaddleLogic* collidedPaddle;
};

enum EnemyState
{
    ENEMY_STATE_IDLE,
    ENEMY_STATE_STEERING,
    ENEMY_STATE_DEFENDING
};

struct BrickLogic
{
    Uint32 entityIndex;
    EntityPowerUp powerUp;
    Uint32 hitPoints;
    //For now we assume that a brick can only interact with a ball
    BallLogic* collidedBall;
};

struct EnemyControl
{
    Uint32 paddleLogicIndex;
    EnemyState state;
    Rectangle dangerZone;
    Vec2 target;
};

struct LevelComponent
{
    Vec2 pos;
    Vec2 dim;
    Uint32 hitPoints;
    EntityPowerUp powerUp;
};

struct Level
{
    LevelComponent components[128];
    Uint32 componentCount;
};

void updateEntities(GameState *gameState);

#endif
