#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"
#include "GameState.hpp"

void collideWithBrick(Ball& ball, Brick& brick);

void gameUpdate(GameState& gameState, const Renderer& renderer) {
    if (!gameState.init) {
        initPaddle(gameState.paddle);
        initBall(gameState.ball);
        gameState.leftBoundary = 10;
        gameState.rightBoundary = SCREEN_WIDTH - 10;
        gameState.init = SDL_TRUE;
        gameState.brick = {Vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), 25, 25};
    }
    
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
    }

    if (gameState.paused) {
        return;
    }

    updateBall(gameState.ball, gameState.delta, gameState.input);
    updatePaddle(gameState);
    collideWithBrick(gameState.ball, gameState.brick);
    resolveCollision(gameState.ball, gameState.paddle, renderer, gameState.delta);
    
    renderer.clear();
    renderer.drawPaddle(gameState.paddle);
    renderer.drawBall(gameState.ball);
    renderer.drawBoundaries(gameState.leftBoundary, gameState.rightBoundary);
    renderer.drawBrick(gameState.brick);
    renderer.drawPoint(gameState.paddle.newPos);
    renderer.update();
}

bool collide(Ball& ball, Brick& brick) {
    float verticalDist = fabsf(ball.newPos.y - brick.center.y);
    float horizontalDist = fabsf(ball.newPos.x - brick.center.x);
    float halfWidth = brick.width * 0.5f;
    float halfHeight = brick.height * 0.5f;
    
    if (horizontalDist > ball.radius + halfWidth) {
        return false;
    }
    
    if (verticalDist > ball.radius + halfHeight) {
        return false;
    }
    
    if (horizontalDist <= halfWidth) {
        return true;
    }
    
    if (verticalDist <= halfHeight) {
        return true;
    }
    
    float dx = horizontalDist - halfWidth;
    float dy = verticalDist - halfHeight;
    
    return (dx * dx + dy * dy <= ball.radius * ball.radius);
}

void collideWithBrick(Ball& ball, Brick& brick) {
    if (collide(ball, brick)) {
        Vec2 reflectionNorm = ball.newPos - brick.center;
        reflectionNorm.normalize();
        ball.velocity = reflectionNorm;
        ball.movementDelta += reflectionNorm;
    }
}
