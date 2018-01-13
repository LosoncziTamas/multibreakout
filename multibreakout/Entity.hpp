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

struct PaddleLogic
{
    Uint32 entityIndex;
    Uint32 flags;
    bool moveLeft;
    bool moveRight;
    Entity* ball;
};

enum EnemyState
{
    ENEMY_STATE_IDLE,
    ENEMY_STATE_STEERING,
    ENEMY_STATE_DEFENDING
};

struct EnemyControl
{
    Uint32 paddleLogicIndex;
    EnemyState state;
    Rectangle dangerZone;
    Vec2 target;
};

void updateEntities(GameState *gameState);

#endif
