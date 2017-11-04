#include <SDL2/SDL_assert.h>

#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"
#include "Physics.hpp"
#include "Texture.hpp"

void initPaddle(Paddle &paddle) {
    paddle.width = DEFAULT_WIDTH;
    paddle.height = DEFAULT_HEIGHT;
    paddle.newPos = Vec2(SCREEN_WIDTH * 0.5f, DEFAULT_HEIGHT * 0.5f);
    paddle.speed = DEFAULT_SPEED;
    paddle.orientation = lower;
    paddle.textureIndex = INVALID_INDEX;
}

void acceleratePaddle(Vec2& acceleration, Paddle& paddle, float delta) {
    acceleration *= paddle.speed;
    paddle.oldPos = paddle.newPos;
    paddle.movementDelta = (0.5f * acceleration * pow(delta, 2) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    paddle.newPos = paddle.oldPos + paddle.movementDelta;
}

void updatePaddle(World& world, GameInput& input, float delta) {
    Paddle &paddle = world.paddle;
    
    Vec2 acceleration;
    if (input.left) {
        if (paddle.velocity.x > 0){
            paddle.velocity.x = 0.0f;
        }
        acceleration.x = -1.0;
    } else if (input.right) {
        if (paddle.velocity.x < 0){
            paddle.velocity.x = 0.0f;
        }
        acceleration.x = 1.0;
    } else {
        paddle.velocity += - 0.02 * paddle.velocity;
    }
    
    acceleratePaddle(acceleration, paddle, delta);
    
    float offset = paddle.width * 0.5f;
    if (paddle.newPos.x - offset < (world.obstacles.leftBottom.center.x + world.obstacles.leftBottom.width * 0.5f)) {
        Vec2 wallNorm(1, 0);
        paddle.velocity = reflect(paddle.velocity, wallNorm);
        paddle.movementDelta.x += 1.0f;
    } else if (paddle.newPos.x + offset > (world.obstacles.rightBottom.center.x - world.obstacles.leftBottom.width * 0.5f)) {
        Vec2 wallNorm(-1, 0);
        paddle.velocity = reflect(paddle.velocity, wallNorm);
        paddle.movementDelta.x -= 1.0f;
    }
}

void activatePowerUp(Ball& ball, Paddle& paddle) {
    switch (ball.powerUp) {
        case speedUp:
            if (paddle.speed == DEFAULT_SPEED) {
                paddle.speed = HIGH_SPEED;
            } else if (paddle.speed == LOW_SPEED) {
                paddle.speed = DEFAULT_SPEED;
            }
            break;
        case slowDown:
            if (paddle.speed == DEFAULT_SPEED) {
                paddle.speed = LOW_SPEED;
            } else if (paddle.speed == HIGH_SPEED) {
                paddle.speed = DEFAULT_SPEED;
            }
            break;
        case strech:
            if (paddle.width == DEFAULT_WIDTH) {
                paddle.width = LARGE_WIDTH;
            } else if (paddle.width == SMALL_WIDTH) {
                paddle.width = DEFAULT_WIDTH;
            } else if (paddle.height == DEFAULT_WIDTH) {
                paddle.height = LARGE_WIDTH;
            } else if (paddle.height == SMALL_WIDTH) {
                paddle.height = DEFAULT_WIDTH;
            }
            break;
        case shrink:
            if (paddle.width == DEFAULT_WIDTH) {
                paddle.width = SMALL_WIDTH;
            } else if (paddle.width == LARGE_WIDTH) {
                paddle.width = DEFAULT_WIDTH;
            } else if (paddle.height == DEFAULT_WIDTH) {
                paddle.height = SMALL_WIDTH;
            } else if (paddle.height == LARGE_WIDTH) {
                paddle.height = DEFAULT_WIDTH;
            }
            break;
        case neutral:
            break;
        default:
            SDL_assert(false);
            break;
    }
    ball.textureIndex = GREY_BALL;
    ball.powerUp = neutral;
}

void resolveCollision(std::vector<Ball>& balls, Paddle& paddle, float delta) {
    for (auto& ball : balls) {
        if (circleRectIntersect(ball.newPos, ball.radius, paddle.newPos, paddle.width, paddle.height)) {
            for (int i = 1; i <= 4; ++i) {
                Vec2 ballCollisionLocation = (1.0f / i) * ball.oldPos + (1.0f - 1.0f / i) * ball.newPos;
                Vec2 paddleCollisionLocation = (1.0f / i) * paddle.oldPos + (1.0f - 1.0f / i) * paddle.newPos;
                if (circleRectIntersect(ballCollisionLocation, ball.radius, paddleCollisionLocation, paddle.width, paddle.height)) {
                    Vec2 reflection = ballCollisionLocation - paddleCollisionLocation;
                    Vec2 reflectionInverse = paddleCollisionLocation - ballCollisionLocation;
                    reflection.normalize();
                    if (paddle.orientation == left || paddle.orientation == right) {
                        reflectionInverse.x = 0.0f;
                    } else {
                        reflectionInverse.y = 0.0f;
                    }
                    reflectionInverse.normalize();
                    ball.velocity = reflection;
                    ball.movementDelta += reflection * paddle.movementDelta.length();
                    paddle.movementDelta += reflectionInverse * paddle.movementDelta.length();
                    activatePowerUp(ball, paddle);
                    break;
                }
            }
        }
        ball.newPos = ball.oldPos + ball.movementDelta;
        paddle.newPos = paddle.oldPos + paddle.movementDelta;
    }
}