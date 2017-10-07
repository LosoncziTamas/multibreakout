#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

void initPaddle(Paddle &paddle) {
    paddle.width = 120.0f;
    paddle.height = 40.0f;
    paddle.newPos = Vec2(SCREEN_WIDTH / 2, 20);
    paddle.speed = 50.0f;
}

void updatePaddle(GameState& gameState) {
    Paddle &paddle = gameState.paddle;
    float delta = gameState.delta;
    
    Vec2 acceleration;
    if (gameState.input.left) {
        if (paddle.velocity.x > 0){
            paddle.velocity.x = 0.0f;
        }
        acceleration.x = -1.0;
    } else if (gameState.input.right) {
        if (paddle.velocity.x < 0){
            paddle.velocity.x = 0.0f;
        }
        acceleration.x = 1.0;
    } else {
        paddle.velocity += - 0.02 * paddle.velocity;
    }
    
    acceleration *= paddle.speed;

    paddle.oldPos = paddle.newPos;
    paddle.movementDelta = (0.5f * acceleration * pow(delta, 2) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    paddle.newPos = paddle.oldPos + paddle.movementDelta;
    
    float offset = paddle.width * 0.5f;
    if (paddle.newPos.x - offset < gameState.leftBoundary) {
        Vec2 wallNorm(1, 0);
        paddle.velocity = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.movementDelta.x += 1.0f;
    } else if (paddle.newPos.x + offset > gameState.rightBoundary) {
        Vec2 wallNorm(-1, 0);
        paddle.velocity = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.movementDelta.x -= 1.0f;
    }
}

bool collide(Vec2& ballCollisionPos, Vec2& paddleCollisionPos, Paddle& paddle, float radius) {
    float verticalDist = fabsf(ballCollisionPos.y - paddleCollisionPos.y);
    float horizontalDist = fabsf(ballCollisionPos.x - paddleCollisionPos.x);
    float halfWidth = paddle.width * 0.5f;
    float halfHeight = paddle.height * 0.5f;
    
    if (horizontalDist > radius + halfWidth) {
        return false;
    }
    
    if (verticalDist > radius + halfHeight) {
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
    
    return (dx * dx + dy * dy <= radius * radius);
}

void activatePowerUp(Ball& ball, Paddle& paddle) {
    switch (ball.powerUp) {
        case speedUp:
            if (paddle.speed == 50.0f) {
                paddle.speed = 100.0f;
            } else if (paddle.speed == 25.0f) {
                paddle.speed = 50.0f;
            }
            printf("speed up\n");
            break;
        case slowDown:
            if (paddle.speed == 50.0f) {
                paddle.speed = 25.0f;
            } else if (paddle.speed == 100.0f) {
                paddle.speed = 50.0f;
            }
            printf("slow down \n");
            break;
        case strech:
            if (paddle.width == 120.0f) {
                paddle.width = 150.0f;
            } else if (paddle.width == 80.0f) {
                paddle.width = 120.0f;
            }
            printf("strech \n");
            break;
        case shrink:
            if (paddle.width == 120.0f) {
                paddle.width = 80.0f;
            } else if (paddle.width == 150.0f) {
                paddle.width = 120.0f;
            }
            printf("shrink \n");
            break;
        case neutral:
        default:
            printf("neutral \n");
            break;
    }
    ball.powerUp = neutral;
}

void resolveCollision(Ball& ball, Paddle& paddle, float delta) {
    if (collide(ball.newPos, paddle.newPos, paddle, ball.radius)) {
        for (int i = 1; i <= 4; ++i) {
            Vec2 ballCollisionLocation = (1.0f / i) * ball.oldPos + (1.0f - 1.0f / i) * ball.newPos;
            Vec2 paddleCollisionLocation = (1.0f / i) * paddle.oldPos + (1.0f - 1.0f / i) * paddle.newPos;
            if (collide(ballCollisionLocation, paddleCollisionLocation, paddle, ball.radius)) {
                Vec2 reflectionNorm = ballCollisionLocation - paddleCollisionLocation;
                reflectionNorm.normalize();
                ball.velocity = reflectionNorm;
                ball.movementDelta += reflectionNorm * paddle.movementDelta.length();
                activatePowerUp(ball, paddle);
                break;
            }
        }
    }
    
    ball.newPos = ball.oldPos + ball.movementDelta;
    paddle.newPos = paddle.oldPos + paddle.movementDelta;
}