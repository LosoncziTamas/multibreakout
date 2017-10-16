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
    Texture *paddleTexture = new Texture("paddle.png", renderer);
    renderer.textures.push_back(paddleTexture);
    gameState.paddle.textureIndex = 0;
    Texture *ballTexture = new Texture("ball.png", renderer);
    renderer.textures.push_back(ballTexture);
    for (auto& ball : gameState.balls) {
        ball.textureIndex = 1;
    };
    Texture *brickTexture = new Texture("brick_yellow.png", renderer);
    renderer.textures.push_back(brickTexture);
    for (auto& brick : gameState.bricks) {
        brick.textureIndex = 2;
    };
    Texture *enemyTexture = new Texture("enemy_paddle.png", renderer);
    renderer.textures.push_back(enemyTexture);
    gameState.enemy.paddle.textureIndex = 3;
    
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
    
    clear(renderer);
    drawLowerPaddle(renderer, gameState.paddle);
    drawPaddleDebug(renderer, gameState.paddle);
    drawUpperPaddle(renderer, gameState.enemy.paddle);
    drawPaddleDebug(renderer, gameState.enemy.paddle);
    drawBalls(renderer, gameState.balls, gameState.delta);
    drawBallsDebug(renderer, gameState.balls);
    drawBoundaries(renderer, gameState.leftBoundary, gameState.rightBoundary);
    drawBricks(renderer, gameState.bricks);
    drawBricksDebug(renderer, gameState.bricks);
    drawPoint(renderer, gameState.paddle.newPos);
    update(renderer);
}
