#ifndef Paddle_hpp
#define Paddle_hpp

#include <SDL2/SDL_render.h>

#include "Vec2.hpp"
#include "Ball.hpp"
#include "MultiBreakout.hpp"


struct World;
struct Atlas;

const float DEFAULT_WIDTH = 90.0f;
const float DEFAULT_HEIGHT = 25.0f;
const float DEFAULT_SPEED = 50.0f;

const float SMALL_WIDTH = 80.0f;
const float LARGE_WIDTH = 150.0f;

const float LOW_SPEED = 25.0f;
const float HIGH_SPEED = 100.0f;

enum Orientation {upper, lower, left, right};

struct Paddle {
    float width;
    float height;
    float speed;
    int ballIndex;
    int textureIndex;
    Vec2 velocity;
    Vec2 movementDelta;
    Vec2 oldPos;
    Vec2 newPos;
    Orientation orientation;
};

void initPaddle(Paddle* paddle);

void updatePaddle(World* world, GameInput* input, GameInput* oldInput, float delta);

void resolveCollision(World& world, Paddle& paddle, float delta);

Vec2 acceleratePaddle(Vec2 acceleration, Paddle* paddle, float delta);

void drawPaddleDebug(SDL_Renderer* renderer, Paddle* paddle);

void drawUpperPaddle(SDL_Renderer* renderer, Atlas* atlas, Paddle* paddle);

void drawLeftPaddle(SDL_Renderer* renderer, Atlas* atlas, Paddle* paddle);

void drawRightPaddle(SDL_Renderer* renderer, Atlas* atlas, Paddle* paddle);

void drawLowerPaddle(SDL_Renderer* renderer, Atlas* atlas, Paddle* paddle);

#endif