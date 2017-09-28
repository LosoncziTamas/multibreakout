#include "Ball.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

void initBall(Ball &ball) {
    ball.center = Vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    ball.velocity = Vec2(0, 0);
    ball.radius = 16;
}

void updateBall(Ball& ball, float delta, const GameInput& input) {
    if (input.mouseLeft) {
        ball.center.x = input.mouseX;
        ball.center.y = SCREEN_HEIGHT - input.mouseY;
        ball.velocity.x = 0.0f;
        ball.velocity.y = 0.0f;
    }
    
    if (input.mouseRight) {
        Vec2 newVelocity(input.mouseX - ball.center.x, SCREEN_HEIGHT - input.mouseY - ball.center.y);
        ball.velocity = newVelocity.normalize() * 100.0f;
    }
    
    Vec2 ballDelta = ball.velocity * delta;
    ball.oldPos = ball.center;
    ball.newPos = ball.center + ballDelta;
    
    if (ball.newPos.y + ball.radius >= SCREEN_HEIGHT) {
        Vec2 wallNorm(0.0f, -1.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
    }
    
    if (ball.newPos.y - ball.radius <= 0) {
        Vec2 wallNorm(0.0f, 1.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
    }
    
    if (ball.newPos.x - ball.radius <= 0) {
        Vec2 wallNorm(1.0f, 0.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
    }
    
    if (ball.newPos.x + ball.radius >= SCREEN_WIDTH) {
        Vec2 wallNorm(-1.0f, 0.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
    }

}