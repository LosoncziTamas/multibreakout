#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"

static SDL_bool init;
static Paddle paddle;
static Ball ball;
static SDL_bool paused;

const float paddleSpeed = 50.0f;
const int wallWidth = 10;

static SDL_Rect leftWall = {0, 0, wallWidth, SCREEN_HEIGHT};
static SDL_Rect rightWall = {SCREEN_WIDTH - wallWidth, 0, wallWidth, SCREEN_HEIGHT};

void gameUpdate(const GameInput& input, const Renderer& renderer, float delta) {
    
    if (!init) {
        initPaddle(paddle);
        initBall(ball);
        init = SDL_TRUE;
    }
    
    if (input.pause) {
        paused = paused == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
    }
    
    if (paused) {
        return;
    }
    
    update(paddle, ball, input, delta, leftWall, rightWall);


    renderer.clear();
    renderer.drawBall(ball);
#if 0
    renderer.drawRectangle(collisionShape);
#endif
    renderer.drawPaddle(paddle);
    renderer.drawRectangle(leftWall);
    renderer.drawRectangle(rightWall);
#if 0
    if (collision) {
        renderer.drawPoint(pointOfCollision);
    }
#endif
    renderer.update();
}
