#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"
#include "GameState.hpp"

void collideWithBrick(Ball& ball, Brick& brick);

void gameUpdate(GameState& gameState, const Renderer& renderer) {
    if (!gameState.init) {
        initPaddle(gameState.paddle);
        initEnemy(gameState.enemy);
        initBall(gameState.ball);
        gameState.leftBoundary = 10;
        gameState.rightBoundary = SCREEN_WIDTH - 10;
        gameState.init = SDL_TRUE;
        initBricks(gameState.bricks);
    }
    
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
    }

    if (gameState.paused) {
        return;
    }

    updateBall(gameState);
    updatePaddle(gameState);
    updateEnemy(gameState);
    for (auto& brick : gameState.bricks) {
        collideWithBrick(gameState.ball, brick);
    }
    resolveCollision(gameState.ball, gameState.enemy, gameState.delta);
    resolveCollision(gameState.ball, gameState.paddle, gameState.delta);
    
    renderer.clear();
    renderer.drawPaddle(gameState.paddle);
    renderer.drawPaddle(gameState.enemy);
    renderer.drawBall(gameState.ball);
    renderer.drawBoundaries(gameState.leftBoundary, gameState.rightBoundary);
    for (auto& brick : gameState.bricks) {
        renderer.drawBrick(brick);
    }
    renderer.drawPoint(gameState.paddle.newPos);
    renderer.update();
}
