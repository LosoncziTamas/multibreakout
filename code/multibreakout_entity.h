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

struct Entity
{
    Uint32 storageIndex;
    Vec2 p;
    Vec2 dp;
    Vec2 dimensions;
    EntityType type;
    Uint32 flags;
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

enum PaddleFlags
{
    PADDLE_FLAG_ORIENTATION_BOTTOM = (1 << 0),
    PADDLE_FLAG_ORIENTATION_TOP = (1 << 1),
    PADDLE_FLAG_ORIENTATION_LEFT = (1 << 2),
    PADDLE_FLAG_ORIENTATION_RIGHT = (1 << 3),
    PADDLE_FLAG_PLAYER_CONTROLLED = (1 << 4)
};

void setFlag(Entity *entity, Uint32 flag);

#endif