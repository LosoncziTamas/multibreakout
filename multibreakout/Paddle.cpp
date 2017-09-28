#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "Rect.hpp"
#include "GameState.hpp"

const float paddleSpeed = 50.0f;

void initPaddle(Paddle &paddle);
void update(Paddle& paddle, Ball& ball, const GameInput& input, float delta, const SDL_Rect& leftWall, const SDL_Rect& rightWall);
void collide(float delta, Paddle& paddle, Ball& ball, const Renderer& renderer);

void initPaddle(Paddle &paddle) {
    paddle.width = 120;
    paddle.height = 40;
    paddle.centerPos = Vec2(SCREEN_WIDTH / 2, 50);
}

void updatePaddle(GameState& gameState, const Renderer& renderer) {
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
    acceleration *= paddleSpeed;

    paddle.oldPos = paddle.centerPos;
    paddle.movementDelta = (0.5f * acceleration * pow(delta, 2) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    paddle.newPos = paddle.oldPos + paddle.movementDelta;
    
    float offset = paddle.width * 0.5f;
    if (paddle.newPos.x - offset < gameState.leftWall.w) {
        Vec2 wallNorm(1, 0);
        Vec2 reflection = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.velocity = reflection;
        paddle.movementDelta.x += 1.0f;
    } else if (paddle.newPos.x + offset > gameState.rightWall.x) {
        Vec2 wallNorm(-1, 0);
        paddle.velocity = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.movementDelta.x -= 1.0f;
    }
}

Rect createCollisionShape(Paddle& paddle, float radius) {
    float offset = paddle.width * 0.5f + radius;
    int x = paddle.oldPos.x < paddle.newPos.x ? paddle.oldPos.x - offset : paddle.newPos.x - offset;
    int y = paddle.oldPos.y - radius;
    int w = fabsf(paddle.oldPos.x - paddle.newPos.x) + paddle.width + 2 * radius;
    int h = paddle.height + 2 * radius;
    Rect shape = {x, y, w, h};
    
    return shape;
}

Rect createExtendedPaddleRect(Paddle& paddle, Vec2 paddleCollisionLocation, float radius) {
    float offset = paddle.width * 0.5f + radius;
    int x = paddleCollisionLocation.x - offset;
    int y = paddleCollisionLocation.y - radius;
    int w = paddle.width + 2 * radius;
    int h = paddle.height + 2 * radius;
    Rect paddleRect = {x, y, w, h};
    
    return paddleRect;
}

void reflect(const Vec2& norm, Ball& ball, const Vec2& playerDelta = Vec2(0, 0)) {
    ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(norm) * norm;
    ball.center += norm + playerDelta;
}

void collide(GameState& gameState, const Renderer& renderer) {
    Paddle &paddle = gameState.paddle;
    Ball &ball = gameState.ball;
    Rect collisionShape = createCollisionShape(paddle, ball.radius);
    bool collision = false;
    Vec2 pointOfCollision;

    if (contains(collisionShape, ball.center)) {
        Vec2 ballCollisionLocation;
        Vec2 paddleCollisionLocation;
        
        for (int i = 1; i <= 4; ++i) {
            ballCollisionLocation = (1.0f - 1.0f / i) * ball.center + (1.0f / i) * ball.newPos;
            paddleCollisionLocation = (1.0f - 1.0f / i) * paddle.centerPos + (1.0f / i) * paddle.newPos;
            Rect paddleRect = createExtendedPaddleRect(paddle, paddleCollisionLocation, ball.radius);
            if (contains(paddleRect, ballCollisionLocation)) {
                collision = true;
                pointOfCollision = ballCollisionLocation;
                
                renderer.drawPoint(pointOfCollision);
                
                float dx = ball.newPos.x - ball.center.x;
                float dy = ball.newPos.y - ball.center.y;
                
                float top = paddleCollisionLocation.y + paddle.height;
                float bottom = paddleCollisionLocation.y;
                float left = paddleCollisionLocation.x - 0.5f * paddle.width;
                float right = paddleCollisionLocation.x + 0.5f * paddle.width;
                
                if (pointOfCollision.y >= top) {
                    if (pointOfCollision.x < left) {
                        if (dx > 0 && dy > 0) {
                            reflect(Vec2(-0.707f, 0.707f), ball, paddle.movementDelta);
                            std::cout << "top|left" << ball.velocity << std::endl;
                        } else if (dx > 0 && dy < 0) {
                            reflect(Vec2(-1.0f, 0.0f), ball, paddle.movementDelta);
                            std::cout << "left" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy > 0) {
                            reflect(Vec2(0.0f, 1.0f), ball);
                            std::cout << "top" << ball.velocity << std::endl;
                        }
                    } else if (pointOfCollision.x >= left && pointOfCollision.x <= right) {
                        reflect(Vec2(0.0f, 1.0f), ball);
                        std::cout << "top" << ball.velocity << std::endl;
                    } else if (pointOfCollision.x > right) {
                        if (dx > 0 && dy > 0) {
                            reflect(Vec2(0.0f, 1.0f), ball);
                            std::cout << "top" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy < 0) {
                            reflect(Vec2(1.0f, 0.0f), ball, paddle.movementDelta);
                            std::cout << "right" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy > 0) {
                            reflect(Vec2(0.707f, 0.707f), ball, paddle.movementDelta);
                            std::cout << "top|right" << ball.velocity << std::endl;
                        }
                    } else {
                        SDL_assert(false);
                    }
                } else if (pointOfCollision.y <= bottom) {
                    if (pointOfCollision.x < left) {
                        if (dx > 0 && dy < 0) {
                            reflect(Vec2(-0.707f, -0.707f), ball, paddle.movementDelta);
                            std::cout << "bottom|left" << ball.velocity << std::endl;
                        } else if (dx > 0 && dy > 0) {
                            reflect(Vec2(-1.0f, 0.0f), ball, paddle.movementDelta);
                            if (ball.velocity.x > 0) {
                                ball.velocity.x *= -1;
                                std::cout << "left" << ball.velocity << std::endl;
                            }
                        } else if (dx < 0 && dy < 0) {
                            reflect(Vec2(0.0f, -1.0f), ball);
                            std::cout << "bottom" << ball.velocity << std::endl;
                        }
                    } else if (pointOfCollision.x >= left && pointOfCollision.x <= right) {
                        reflect(Vec2(0.0f, -1.0f), ball);
                        std::cout << "bottom" << ball.velocity << std::endl;
                    } else if (pointOfCollision.x > right) {
                        if (dx > 0 && dy < 0) {
                            reflect(Vec2(0.0f, -1.0f), ball);
                            std::cout << "bottom" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy > 0) {
                            reflect(Vec2(1.0f, 0.0f), ball, paddle.movementDelta);
                            std::cout << "right" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy < 0) {
                            reflect(Vec2(0.707f, -0.707f), ball, paddle.movementDelta);
                            std::cout << "bottom|right" << ball.velocity << std::endl;
                        }
                    } else {
                        SDL_assert(false);
                    }
                } else if (pointOfCollision.y < top && pointOfCollision.y > bottom) {
                    if (pointOfCollision.x < left) {
                        reflect(Vec2(-1.0f, 0.0f), ball, paddle.movementDelta);
                        if (ball.velocity.x > 0) {
                            ball.velocity.x *= -1;
                        }
                        std::cout << "left" << ball.velocity << std::endl;
                    } else if (pointOfCollision.x >= left && pointOfCollision.x <= right) {
                        std::cout << "inside" << ball.velocity << std::endl;
                    } else if (pointOfCollision.x > right) {
                        reflect(Vec2(1.0f, 0.0f), ball, paddle.movementDelta);
                        std::cout << "right"<< ball.velocity << std::endl;
                    } else {
                        SDL_assert(false);
                    }
                }
                break;
            }
        }
    }
    
    ball.center += ball.velocity * gameState.delta;
    paddle.centerPos = paddle.oldPos + paddle.movementDelta;
}