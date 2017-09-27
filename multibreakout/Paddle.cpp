#include "Paddle.hpp"
#include "MultiBreakout.hpp"

const float paddleSpeed = 50.0f;

void initPaddle(Paddle &paddle);
void update(Paddle& paddle, Ball& ball, const GameInput& input, float delta, const SDL_Rect& leftWall, const SDL_Rect& rightWall);
void collide(float delta, Paddle& paddle, Ball& ball, const Renderer& renderer);

std::ostream& operator<< (std::ostream& stream, const Paddle& paddle) {
    stream << "vel: " << paddle.velocity << " pos: " << paddle.centerPos;
    return stream;
}

void initPaddle(Paddle &paddle) {
    paddle.width = 120;
    paddle.height = 40;
    paddle.centerPos = Vec2(SCREEN_WIDTH / 2, 50);
}

void updatePaddle(Paddle& paddle, Ball& ball, const GameInput& input, float delta, const SDL_Rect& leftWall, const SDL_Rect& rightWall, const Renderer& renderer) {
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
    
    acceleration *= paddleSpeed;

    paddle.oldPos = paddle.centerPos;
    paddle.movementDelta = (0.5f * acceleration * pow(delta, 2) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    paddle.newPos = paddle.oldPos + paddle.movementDelta;
    
    float offset = paddle.width * 0.5f;
    Vec2 paddleLeftCorner(paddle.newPos.x - offset, paddle.centerPos.y);
    Vec2 paddleRightCorner(paddle.newPos.x + offset, paddle.centerPos.y);
    
    if (paddleLeftCorner.x < leftWall.w) {
        Vec2 wallNorm(1, 0);
        Vec2 reflection = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.velocity = reflection;
        paddle.movementDelta.x += 1.0f;
    } else if (paddleRightCorner.x > rightWall.x) {
        Vec2 wallNorm(-1, 0);
        paddle.velocity = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.movementDelta.x -= 1.0f;
    }
    
    collide(delta, paddle, ball, renderer);
}

SDL_Rect createCollisionShape(Paddle& paddle, float radius) {
    float offset = paddle.width * 0.5f + radius;
    int x = paddle.oldPos.x < paddle.newPos.x ? paddle.oldPos.x - offset : paddle.newPos.x - offset;
    int y = paddle.oldPos.y - radius; // old: paddle.oldPos.y - paddle.height - radius
    int w = fabsf(paddle.oldPos.x - paddle.newPos.x) + paddle.width + 2 * radius;
    int h = paddle.height + 2 * radius;
    SDL_Rect shape = {x, y, w, h};
    
    return shape;
}

bool contains(const SDL_Rect& rect, Vec2& vec) {
    bool horizontal = vec.x >= rect.x && vec.x <= rect.x + rect.w;
    bool vertical = vec.y >= rect.y && vec.y <= rect.y + rect.h;
    return horizontal && vertical;
}

SDL_Rect createExtendedPaddleRect(Paddle& paddle, Vec2 paddleCollisionLocation, float radius) {
    float offset = paddle.width * 0.5f + radius;
    int x = paddleCollisionLocation.x - offset;
    int y = paddleCollisionLocation.y - radius; //paddleCollisionLocation.y - paddle.height - radius
    int w = paddle.width + 2 * radius;
    int h = paddle.height + 2 * radius;
    
    SDL_Rect paddleRect = {x, y, w, h};
    return paddleRect;
}

void reflect(const Vec2& norm, Ball& ball, const Vec2& playerDelta = Vec2(0, 0)) {
    ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(norm) * norm;
    ball.center += norm + playerDelta;
}

void collide(float delta, Paddle& paddle, Ball& ball, const Renderer& renderer) {
    SDL_Rect collisionShape = createCollisionShape(paddle, ball.radius);

    bool collision = false;
    Vec2 pointOfCollision;

    if (contains(collisionShape, ball.center)) {
        Vec2 ballCollisionLocation;
        Vec2 paddleCollisionLocation;
        for (int i = 1; i <= 4; ++i) {
            float scale1 = (1.0f - 1.0f / i);
            float scale2 = (1.0f / i);
            ballCollisionLocation = scale1 * ball.center + scale2 * ball.newPos;
            paddleCollisionLocation = scale1 * paddle.centerPos + scale2 * paddle.newPos;
            SDL_Rect paddleRect = createExtendedPaddleRect(paddle, paddleCollisionLocation, ball.radius);
            if (contains(paddleRect, ballCollisionLocation)) {
                collision = true;
                pointOfCollision = ballCollisionLocation;
                
                renderer.drawPoint(pointOfCollision);
                
                float dx = ball.newPos.x - ball.center.x;
                float dy = ball.newPos.y - ball.center.y;
                
                float offset = 0.5f * paddle.width;
                
                float top = paddleCollisionLocation.y + paddle.height;
                float bottom = paddleCollisionLocation.y;
                float left = paddleCollisionLocation.x - offset;
                float right = paddleCollisionLocation.x + offset;
                
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
    
    ball.center += ball.velocity * delta;
    paddle.centerPos = paddle.oldPos + paddle.movementDelta;
}