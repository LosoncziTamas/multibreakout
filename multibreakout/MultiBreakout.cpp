#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"
#include "GameState.hpp"

void gameUpdate(GameState& gameState, const Renderer& renderer) {
    if (!gameState.init) {
        initPaddle(gameState.paddle);
        initBall(gameState.ball);
        gameState.leftWall = {0, 0, 10, SCREEN_HEIGHT};
        gameState.rightWall = {SCREEN_WIDTH - 10, 0, 10, SCREEN_HEIGHT};
        gameState.init = SDL_TRUE;
    }
    
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
    }

    if (gameState.paused) {
        return;
    }

    updateBall(gameState.ball, gameState.delta, gameState.input);
    updatePaddle(gameState, renderer);
    collide(gameState, renderer);

    renderer.clear();
    renderer.drawPaddle(gameState.paddle);
    renderer.drawBall(gameState.ball);
    renderer.drawRectangle(gameState.leftWall);
    renderer.drawRectangle(gameState.rightWall);
    renderer.update();
}
