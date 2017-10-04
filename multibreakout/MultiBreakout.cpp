#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"
#include "GameState.hpp"

void gameUpdate(GameState& gameState, const Renderer& renderer) {
    if (!gameState.init) {
        initPaddle(gameState.paddle);
        initBall(gameState.ball);
        gameState.leftBoundary = 10;
        gameState.rightBoundary = SCREEN_WIDTH - 10;
        gameState.init = SDL_TRUE;
        gameState.brick = {SCREEN_WIDTH / 2 - 12, SCREEN_HEIGHT / 2 , 25, 25};
    }
    
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
    }

    if (gameState.paused) {
        return;
    }

    updateBall(gameState.ball, gameState.delta, gameState.input);
    updatePaddle(gameState);
    resolveCollision(gameState.ball, gameState.paddle, renderer, gameState.delta);
    
    renderer.clear();
    renderer.drawPaddle(gameState.paddle);
    renderer.drawBall(gameState.ball);
    renderer.drawBoundaries(gameState.leftBoundary, gameState.rightBoundary);
    renderer.drawBrick(gameState.brick);
    renderer.drawPoint(gameState.paddle.newPos);
    renderer.update();
}
