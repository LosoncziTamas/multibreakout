#include <iostream>

#include "MultiBreakout.hpp"
#include "Paddle.hpp"

static SDL_bool init;
static Paddle paddle;
static Ball ball;
static SDL_bool paused;

const float paddleSpeed = 50.0f;
const int wallWidth = 10;

static SDL_Rect leftWall = {0, 0, wallWidth, SCREEN_HEIGHT};
static SDL_Rect rightWall = {SCREEN_WIDTH - wallWidth, 0, wallWidth, SCREEN_HEIGHT};

SDL_Rect createCollisionShape(Vec2 oldPos, Vec2 newPos, float radius) {
    float offset = paddle.width * 0.5f + radius;
    int x = oldPos.x < newPos.x ? oldPos.x - offset : newPos.x - offset;
    int y = oldPos.y - paddle.height - radius;
    int w = fabsf(oldPos.x - newPos.x) + paddle.width + 2 * radius;
    int h = paddle.height + 2 * radius;
    
    SDL_Rect shape = {x, y, w, h};
    
    return shape;
}

bool contains(const SDL_Rect &rect, Vec2 vec) {
    bool horizontal = vec.x >= rect.x && vec.x <= rect.x + rect.w;
    bool vertical = vec.y >= rect.y && vec.y <= rect.y + rect.h;
    return horizontal && vertical;
}

SDL_Rect createExtendedPaddleRect(Vec2 centerPos, float radius) {
    float offset = paddle.width * 0.5f + radius;
    int x = centerPos.x - offset;
    int y = centerPos.y - paddle.height - radius;
    int w = paddle.width + 2 * radius;
    int h = paddle.height + 2 * radius;
    
    SDL_Rect paddleRect = {x, y, w, h};
    return paddleRect;
}

void reflect(const Vec2 &norm, Ball &ball, const Vec2 &playerDelta = Vec2(0, 0)) {
    ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(norm) * norm;
    ball.center += norm + playerDelta;
}

void gameUpdate(const GameInput& input, const Renderer& renderer, float delta){
    
    if (!init) {
        paddle = {Vec2(SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2), Vec2(), 120, 40};
        ball = {Vec2(SCREEN_WIDTH / 2, 20), Vec2(0, -100), 16};
        init = SDL_TRUE;
    }
    
    if (input.pause) {
        paused = paused == SDL_TRUE ? SDL_FALSE : SDL_TRUE;
    }
    
    if (paused) {
        return;
    }
    
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

    if (input.mouseLeft) {
        ball.center.x = input.mouseX;
        ball.center.y = input.mouseY;
        ball.velocity.x = 0.0f;
        ball.velocity.y = 0.0f;
    }
    
    if (input.mouseRight) {
        Vec2 newVelocity(input.mouseX - ball.center.x, input.mouseY - ball.center.y);
        ball.velocity = newVelocity.normalize() * 100.0f;;
    }
    
    acceleration *= paddleSpeed;
    
    Vec2 oldPos = paddle.centerPos;
    Vec2 playerDelta = (0.5f * acceleration * pow(delta, 2) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    
    Vec2 newPos = oldPos + playerDelta;
    float offset = paddle.width * 0.5f;
    Vec2 paddleLeftCorner(newPos.x - offset, paddle.centerPos.y);
    Vec2 paddleRightCorner(newPos.x + offset, paddle.centerPos.y);
    
    if (paddleLeftCorner.x < leftWall.w) {
        Vec2 wallNorm(1, 0);
        Vec2 reflection = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.velocity = reflection;
        //add some offset to bounce
        playerDelta.x += 1.0f;
    }
    if (paddleRightCorner.x > rightWall.x) {
        Vec2 wallNorm(-1, 0);
        paddle.velocity = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        playerDelta.x -= 1.0f;
    }

    
    Vec2 ballNewPos;
    Vec2 ballDelta = ball.velocity * delta;
    ballNewPos = ball.center + ballDelta;
    
    if (ballNewPos.y + ball.radius >= SCREEN_HEIGHT) {
        Vec2 wallNorm(0.0f, 1.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;

    }
    
    if (ballNewPos.y - ball.radius <= 0) {
        Vec2 wallNorm(0.0f, -1.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
    }
    
    if (ballNewPos.x - ball.radius <= 0) {
        Vec2 wallNorm(1.0f, 0.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
    }
    
    if (ballNewPos.x + ball.radius >= SCREEN_WIDTH) {
        Vec2 wallNorm(-1.0f, 0.0f);
        ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
    }
    
    SDL_Rect collisionShape = createCollisionShape(oldPos, newPos, ball.radius);
    
    bool collision = false;
    Vec2 pointOfCollision;
    
    if (contains(collisionShape, ball.center)) {
        Vec2 ballCollisionLocation;
        Vec2 paddleCollisionLocation;
        for (int i = 0; i < 4; ++i) {
            ballCollisionLocation = (1.0f - 1.0f / i) * ball.center + (1.0f / i) * ballNewPos;
            paddleCollisionLocation = (1.0f - 1.0 / i) * paddle.centerPos + (1.0f / i) * newPos;
            SDL_Rect paddleRect = createExtendedPaddleRect(paddleCollisionLocation, ball.radius);
            if (contains(paddleRect, ballCollisionLocation)) {
                collision = true;
                pointOfCollision = ballCollisionLocation;
                
                float dx = ballNewPos.x - ball.center.x;
                float dy = ballNewPos.y - ball.center.y;
                
                float offset = 0.5f * paddle.width;
                
                float top = paddleCollisionLocation.y - paddle.height;
                float bottom = paddleCollisionLocation.y;
                float left = paddleCollisionLocation.x - offset;
                float right = paddleCollisionLocation.x + offset;
                
                if (pointOfCollision.y <= top) {
                    if (pointOfCollision.x < left) {
                        if (dx > 0 && dy > 0) {
                            reflect(Vec2(-0.707f, -0.707f), ball, playerDelta);
                            std::cout << "top|left" << ball.velocity << std::endl;
                        } else if (dx > 0 && dy < 0) {
                            reflect(Vec2(-1.0f, 0.0f), ball, playerDelta);
                            std::cout << "left" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy > 0) {
                            reflect(Vec2(0.0f, -1.0f), ball);
                            std::cout << "top" << ball.velocity << std::endl;
                        }
                    } else if (pointOfCollision.x >= left && pointOfCollision.x <= right) {
                        reflect(Vec2(0.0f, -1.0f), ball);
                        std::cout << "top" << ball.velocity << std::endl;
                    } else if (pointOfCollision.x > right) {
                        if (dx > 0 && dy > 0) {
                            reflect(Vec2(0.0f, -1.0f), ball);
                            std::cout << "top" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy < 0) {
                            reflect(Vec2(1.0f, 0.0f), ball, playerDelta);
                            std::cout << "right" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy > 0) {
                            reflect(Vec2(0.707f, -0.707f), ball, playerDelta);
                            std::cout << "top|right" << ball.velocity << std::endl;
                        }
                    } else {
                        SDL_assert(false);
                    }
                } else if (pointOfCollision.y >= bottom) {
                    if (pointOfCollision.x < left) {
                        if (dx > 0 && dy < 0) {
                            reflect(Vec2(-0.707f, 0.707f), ball, playerDelta);
                            std::cout << "bottom|left" << ball.velocity << std::endl;
                        } else if (dx > 0 && dy > 0) {
                            reflect(Vec2(-1.0f, 0.0f), ball, playerDelta);
                            std::cout << "left" << ball.velocity << std::endl;
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
                            reflect(Vec2(1.0f, 0.0f), ball, playerDelta);
                            std::cout << "right" << ball.velocity << std::endl;
                        } else if (dx < 0 && dy < 0) {
                            reflect(Vec2(0.707f, 0.707f), ball, playerDelta);
                            std::cout << "bottom|right" << ball.velocity << std::endl;
                        }
                    } else {
                        SDL_assert(false);
                    }
                } else if (pointOfCollision.y > top && pointOfCollision.y < bottom) {
                    if (pointOfCollision.x < left) {
                        reflect(Vec2(-1.0f, 0.0f), ball, playerDelta);
                        std::cout << "left" << ball.velocity << std::endl;
                    } else if (pointOfCollision.x >= left && pointOfCollision.x <= right) {
                        std::cout << "inside" << ball.velocity << std::endl;
                    } else if (pointOfCollision.x > right) {
                        reflect(Vec2(1.0f, 0.0f), ball, playerDelta);
                        std::cout << "right"<< ball.velocity << std::endl;
                    } else {
                        SDL_assert(false);
                    }
                }
                
                break;
            }
        }
    }
    
    ball.center += ball.velocity * delta;
    paddle.centerPos = oldPos + playerDelta;


    renderer.clear();
    renderer.drawBall(ball);
#if 0
    renderer.drawRectangle(collisionShape);
#endif
    renderer.drawPaddle(paddle);
    renderer.drawRectangle(leftWall);
    renderer.drawRectangle(rightWall);
#if 0
    if (collision) {
        renderer.drawPoint(pointOfCollision);
    }
#endif
    renderer.update();
}
