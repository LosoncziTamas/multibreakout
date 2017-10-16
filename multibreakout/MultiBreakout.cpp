#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"
#include "GameState.hpp"

void initGameState(GameState& gameState) {
    initPaddle(gameState.paddle);
    initEnemy(gameState.enemy);
    gameState.leftBoundary = 10;
    gameState.rightBoundary = SCREEN_WIDTH - 10;
    gameState.init = SDL_TRUE;
    initBricks(gameState.bricks);
    Ball ball;
    initBall(ball, gameState.balls, gameState.paddle);
    Ball ball2;
    initBall(ball2, gameState.balls, gameState.enemy.paddle);
}

void initTextures(Renderer& renderer, GameState& gameState) {
    Texture *paddle = new Texture("paddle.png", renderer);
    renderer.textures.push_back(paddle);
    gameState.paddle.textureIndex = 0;
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
    
    updateEnemy(gameState.enemy, gameState.balls, gameState.delta, gameState.leftBoundary, gameState.rightBoundary);
    collideWithBrick(gameState.balls, gameState.bricks);
    resolveCollision(gameState.balls, gameState.enemy.paddle, gameState.delta);
    resolveCollision(gameState.balls, gameState.paddle, gameState.delta);
    collideBalls(gameState.balls);
    
    renderer.clear();
    renderer.drawLowerPaddle(gameState.paddle);
    renderer.drawPaddleDebug(gameState.enemy.paddle);
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
