#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"

static SDL_bool init;
static Paddle paddle;

const float PADDLE_SPEED = 4.0f;

void gameUpdate(const GameInput& input, const Renderer& renderer, float delta){
    
    std::cout << delta << std::endl;
    
    if (!init) {
        paddle = {Vec2(SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2), 120, 40};
        init = SDL_TRUE;
    }
    
    Vec2 paddleDelta;
    if (input.left) {
        paddleDelta.x = -1.0;
    }
    if (input.right) {
        paddleDelta.x = 1.0;
    }
    
    paddleDelta *= PADDLE_SPEED;
    
    paddle.centerPos += paddleDelta;
    
    renderer.clear();
    renderer.drawPaddle(paddle);
    renderer.update();
}
