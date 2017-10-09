#include "Ball.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

void initBall(Ball &ball) {
    ball.newPos = Vec2(SCREEN_WIDTH / 2, 60);
    ball.radius = 16.0f;
    ball.speed = 100.0f;
    ball.powerUp = neutral;
}

void updateBall(GameState& gameState) {
    Ball& ball = gameState.ball;
    GameInput& input = gameState.input;
    
    if (input.mouseLeft) {
        ball.newPos.x = input.mouseX;
        ball.newPos.y = SCREEN_HEIGHT - input.mouseY;
        ball.velocity.x = 0.0f;
        ball.velocity.y = 0.0f;
    }
    
    if (input.mouseRight) {
        Vec2 newVelocity(input.mouseX - ball.newPos.x, SCREEN_HEIGHT - input.mouseY - ball.newPos.y);
        ball.velocity = newVelocity.normalize();
    }
    
    ball.movementDelta = ball.velocity * ball.speed * gameState.delta;
    ball.oldPos = ball.newPos;
    ball.newPos = ball.oldPos + ball.movementDelta;
    
    float ballTop = ball.newPos.y + ball.radius;
    if (ballTop >= SCREEN_HEIGHT) {
        Vec2 wallNorm(0.0f, -1.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
        ball.movementDelta.y += SCREEN_HEIGHT - ballTop;
    }
    
    float ballBottom = ball.newPos.y - ball.radius;
    if (ball.newPos.y - ball.radius <= 0) {
        Vec2 wallNorm(0.0f, 1.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
        ball.movementDelta.y -= ballBottom;
    }
    
    float ballLeft = ball.newPos.x - ball.radius;
    if (ballLeft <= gameState.leftBoundary) {
        Vec2 wallNorm(1.0f, 0.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
        ball.movementDelta.x += gameState.leftBoundary - ballLeft;
    }
    
    float ballRight = ball.newPos.x + ball.radius;
    if (ballRight >= gameState.rightBoundary) {
        Vec2 wallNorm(-1.0f, 0.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
        ball.movementDelta.x += gameState.rightBoundary - ballRight;
    }
    
}