#ifndef Projectile_hpp
#define Projectile_hpp

#include "Vec2.hpp"
#include <SDL2/SDL_render.h>

struct World;

struct Projectile {
    Vec2 velocity;
    Vec2 oldPos;
    Vec2 newPos;
    Vec2 delta;
    float width;
    float height;
    float speed;
};

void updateProjectiles(World* world, SDL_Renderer* renderer, float delta);

Projectile* addProjectile(Vec2 location, World* world);


#endif
