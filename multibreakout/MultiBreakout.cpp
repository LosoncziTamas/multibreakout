#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"
#include "GameState.hpp"

const float paddleSpeed = 50.0f;
const int wallWidth = 10;

void gameUpdate(GameState& gameState, const Renderer& renderer) {
    
    if (!gameState.init) {
        initPaddle(gameState.paddle);
        initBall(gameState.ball);
        gameState.leftWall = {0, 0, wallWidth, SCREEN_HEIGHT};
        gameState.rightWall = {SCREEN_WIDTH - wallWidth, 0, wallWidth, SCREEN_HEIGHT};
        gameState.init = SDL_TRUE;
    }
    
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
    }
    
    if (gameState.input.mouseLeft) {
        gameState.ball.center.x = gameState.input.mouseX;
        gameState.ball.center.y = SCREEN_HEIGHT - gameState.input.mouseY;
        gameState.ball.velocity.x = 0.0f;
        gameState.ball.velocity.y = 0.0f;
    }
    
    if (gameState.input.mouseRight) {
        Vec2 newVelocity(gameState.input.mouseX - gameState.ball.center.x, SCREEN_HEIGHT - gameState.input.mouseY - gameState.ball.center.y);
        gameState.ball.velocity = newVelocity.normalize() * 100.0f;
    }

    
    if (gameState.paused) {
        return;
    }


    renderer.clear();
    renderer.drawPaddle(gameState.paddle);
    renderer.drawBall(gameState.ball);
    updateBall(gameState.ball, gameState.delta);
    updatePaddle(gameState.paddle, gameState.ball, gameState.input, gameState.delta, gameState.leftWall, gameState.rightWall, renderer);
#if 0
    renderer.drawRectangle(collisionShape);
#endif
    renderer.drawRectangle(gameState.leftWall);
    renderer.drawRectangle(gameState.rightWall);
#if 0
    if (collision) {
        renderer.drawPoint(pointOfCollision);
    }
#endif
    renderer.update();
}
