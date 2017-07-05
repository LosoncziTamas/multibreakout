#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"

static SDL_bool init;
static Paddle paddle;

const float paddleSpeed = 50.0f;

void gameUpdate(const GameInput& input, const Renderer& renderer, float delta){
    
    if (!init) {
        paddle = {Vec2(SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2), Vec2(), 120, 40};
        init = SDL_TRUE;
    }
    
    Vec2 acceleration;
    if (input.left) {
        acceleration.x = -1.0;
    }
    if (input.right) {
        acceleration.x = 1.0;
    }
    
    acceleration *= paddleSpeed;
        
    Vec2 oldPos = paddle.centerPos;
    Vec2 playerDelta = (0.5f * acceleration * sqrtf(delta) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    Vec2 newPos = oldPos + playerDelta;

    paddle.centerPos = newPos;
    
    std::cout << paddle << std::endl;

    renderer.clear();
    renderer.drawPaddle(paddle);
    renderer.update();
}
