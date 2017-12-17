#ifndef Entity_hpp
#define Entity_hpp

#include <SDL2/SDL_stdinc.h>
#include "Vec2.hpp"

enum EntityType
{
    ENTITY_TYPE_BALL,
    ENTITY_TYPE_PADDLE,
    ENTITY_TYPE_BRICK,
    ENTITY_TYPE_OBSTACLE,
    ENTITY_TYPE_PROJECTILE
};

enum EntityFlags
{
    ENTITY_FLAG_COLLIDES = (1 << 0)
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

#endif
