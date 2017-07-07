#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"

static SDL_bool init;
static Paddle paddle;

const float paddleSpeed = 50.0f;
const int wallWidth = 10;

static SDL_Rect leftWall = {0, 0, wallWidth, SCREEN_HEIGHT};
static SDL_Rect rightWall = {SCREEN_WIDTH - wallWidth, 0, wallWidth, SCREEN_HEIGHT};

void gameUpdate(const GameInput& input, const Renderer& renderer, float delta){
    
    if (!init) {
        paddle = {Vec2(SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2), Vec2(), 120, 40};
        init = SDL_TRUE;
    }
    
    Vec2 acceleration;
    if (input.left) {
        if (paddle.velocity.x > 0){
            paddle.velocity.x = 0;
        }
        acceleration.x = -1.0;
    }
    if (input.right) {
        if (paddle.velocity.x < 0){
            paddle.velocity.x = 0;
        }
        acceleration.x = 1.0;
    }
    
    acceleration *= paddleSpeed;
        
    Vec2 oldPos = paddle.centerPos;
    Vec2 playerDelta = (0.5f * acceleration * sqrtf(delta) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    Vec2 newPos = oldPos + playerDelta;
    
    float offset = paddle.width * 0.5f;
    Vec2 paddleLeftCorner(newPos.x - offset, paddle.centerPos.y);
    Vec2 paddleRightCorner(newPos.x + offset, paddle.centerPos.y);
    
    if (paddleLeftCorner.x < leftWall.w) {
        Vec2 wallNorm(1, 0);
        playerDelta = playerDelta - 2 * playerDelta * wallNorm.dotProduct(wallNorm);
        paddle.velocity.x *= -1.0;

    }
    if (paddleRightCorner.x > rightWall.x) {
        Vec2 wallNorm(-1, 0);
        playerDelta = playerDelta - 2 * playerDelta * wallNorm.dotProduct(wallNorm);
        paddle.velocity.x *= -1.0;
    }

    paddle.centerPos = oldPos + playerDelta;
    
    std::cout << paddle << std::endl;

    renderer.clear();
    renderer.drawPaddle(paddle);
    renderer.drawRectangle(leftWall);
    renderer.drawRectangle(rightWall);
    renderer.update();
}
