#include <iostream>

#include "MultiBreakout.hpp"
#include "Renderer.hpp"

void initGameState(GameState& gameState) {
    gameState.leftBoundary = 160;
    gameState.rightBoundary = SCREEN_WIDTH - 160;
    gameState.init = true;
    initPaddle(gameState.paddle);
    initUpperEnemy(gameState.enemyUpper);
    initLeftEnemy(gameState.enemyLeft, gameState.leftBoundary);
    initRightEnemy(gameState.enemyRight, gameState.rightBoundary);
    initBricks(gameState.bricks);
    Ball ball;
    initBall(ball, gameState.balls, gameState.paddle);
    Ball ball2;
    initBall(ball2, gameState.balls, gameState.enemyUpper.paddle);
}

void gameUpdate(GameState& gameState, Renderer& renderer) {
    if (!gameState.init) {
        srand(time(NULL));
        initGameState(gameState);
        initTextures(renderer, gameState);
    }
    
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
    }

    if (gameState.paused) {
        return;
    }

    updateBalls(gameState);
    updatePaddle(gameState);
    
    updateEnemy(gameState.enemyUpper, gameState.balls, gameState.delta, gameState.leftBoundary, gameState.rightBoundary);
    collideWithBrick(gameState.balls, gameState.bricks);
    resolveCollision(gameState.balls, gameState.enemyUpper.paddle, gameState.delta);
    resolveCollision(gameState.balls, gameState.paddle, gameState.delta);
    collideBalls(gameState.balls);

    clear(renderer);
    drawLeftPaddle(renderer, gameState.enemyLeft.paddle);
    drawRightPaddle(renderer, gameState.enemyRight.paddle);
    drawLowerPaddle(renderer, gameState.paddle);
    drawPaddleDebug(renderer, gameState.paddle);
    drawUpperPaddle(renderer, gameState.enemyUpper.paddle);
    drawPaddleDebug(renderer, gameState.enemyUpper.paddle);
    drawBalls(renderer, gameState.balls, gameState.delta);
    drawBallsDebug(renderer, gameState.balls);
    drawBoundaries(renderer, gameState.leftBoundary, gameState.rightBoundary);
    drawBricks(renderer, gameState.bricks);
    drawBricksDebug(renderer, gameState.bricks);
    drawPoint(renderer, gameState.paddle.newPos);
    drawDebugInfo(renderer, gameState);
    update(renderer);
}
