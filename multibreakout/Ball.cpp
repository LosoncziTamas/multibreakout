#include "Ball.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

static bool guard = true;

void initBall(Vec2& position, std::vector<Ball>& balls) {
    guard = false;
    Ball ball;
    ball.radius = 16.0f;
    ball.speed = 100.0f;
    ball.newPos = position;
    ball.powerUp = neutral;
    balls.push_back(ball);
}

void collideBalls(std::vector<Ball>& balls) {
    size_t n = balls.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            Ball& ball1 = balls.at(i);
            Ball& ball2 = balls.at(j);
            float distanceSqr = ball1.newPos.distanceSqr(ball2.newPos);
            //TODO: use square for better results?
            if (distanceSqr <= ball1.radius * ball1.radius + ball2.radius * ball2.radius) {
                Vec2 reflection1 = ball1.newPos - ball2.newPos;
                Vec2 reflection2 = ball2.newPos - ball1.newPos;
                ball1.velocity = reflection1.normalize();
                ball2.velocity = reflection2.normalize();
                ball1.movementDelta += reflection1 * ball1.movementDelta.length();
                ball2.movementDelta += reflection2 * ball2.movementDelta.length();
            }
        }
    }
}

void updateBalls(GameState& gameState) {
    GameInput& input = gameState.input;
    
    if (input.mouseLeft && guard) {
        Vec2 ballPosition(input.mouseX, SCREEN_HEIGHT - input.mouseY);
        initBall(ballPosition, gameState.balls);
    }
    
    for (auto& ball : gameState.balls) {
        
        if (input.mouseRight) {
            guard = true;
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
}