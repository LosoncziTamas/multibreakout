#ifndef Ball_hpp
#define Ball_hpp

#include <vector>
#include <SDL2/SDL_render.h>

#include "Vec2.hpp"
#include "Common.hpp"

struct Atlas;
struct World;
struct GameInput;
struct Paddle;

struct Ball {
    Vec2 velocity;
    Vec2 oldPos;
    Vec2 newPos;
    Vec2 movementDelta;
    PowerUp powerUp;
    float radius;
    float speed;
    Paddle *assignedPaddle;
    int textureIndex;
};

Ball *addBall(World& world, Paddle& paddle);
void updateBalls(World& world, GameInput& input, float delta);
void collideBalls(World& world);
void drawBalls(SDL_Renderer* renderer, Atlas& atlas, World& world);
void drawBallsDebug(SDL_Renderer* renderer, World& world);

#endif