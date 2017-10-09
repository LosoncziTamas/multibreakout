#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"
#include "GameState.hpp"

void collideWithBrick(Ball& ball, Brick& brick);

void gameUpdate(GameState& gameState, const Renderer& renderer) {
    if (!gameState.init) {
        initPaddle(gameState.paddle);
        initEnemy(gameState.enemy);
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

    updateBalls(gameState);
    updatePaddle(gameState);
    
    updateEnemy(gameState.enemy, gameState.balls, gameState.delta, gameState.leftBoundary, gameState.rightBoundary);
    collideWithBrick(gameState.balls, gameState.bricks);
    resolveCollision(gameState.balls, gameState.enemy, gameState.delta);
    resolveCollision(gameState.balls, gameState.paddle, gameState.delta);
    collideBalls(gameState.balls);
    
    renderer.clear();
    renderer.drawPaddle(gameState.paddle);
    renderer.drawPaddle(gameState.enemy);
    for (auto& ball: gameState.balls) {
        renderer.drawBall(ball);
    }
    renderer.drawBoundaries(gameState.leftBoundary, gameState.rightBoundary);
    for (auto& brick : gameState.bricks) {
        renderer.drawBrick(brick);
    }
    renderer.drawPoint(gameState.paddle.newPos);
    renderer.update();
}
