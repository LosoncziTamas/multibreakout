#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

const float DEFAULT_WIDTH = 120.0f;
const float DEFAULT_HEIGHT = 40.0f;
const float DEFAULT_SPEED = 50.0f;

const float SMALL_WIDTH = 80.0f;
const float LARGE_WIDTH = 150.0f;

const float LOW_SPEED = 25.0f;
const float HIGH_SPEED = 100.0f;

void initEnemy(Paddle& enemy) {
    enemy.width = DEFAULT_WIDTH;
    enemy.height = DEFAULT_HEIGHT;
    enemy.newPos = Vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 20);
    enemy.oldPos = enemy.newPos;
    enemy.speed = DEFAULT_SPEED;
    enemy.orientation = upper;
    enemy.brain.state = none;
    enemy.brain.steeringPos = enemy.newPos;
}

bool danger(std::vector<Ball>& balls) {
    for (auto& ball : balls) {
        if (ball.newPos.y > SCREEN_HEIGHT * 0.5f && ball.assignedPaddle == nullptr) {
            return true;
        }
    }
    return false;
}

Vec2 getTargetPosition(Paddle& enemy, std::vector<Ball>& balls, float leftBoundary, float rightBoundary) {
    Vec2 targetPos(enemy.newPos);
    Ball *target = nullptr;
    
    for (auto& ball : balls) {
        bool betterTarget = target == nullptr || (target->newPos.y < ball.newPos.y && ball.velocity.y > 0);
        if (ball.newPos.y > SCREEN_HEIGHT * 0.5f && betterTarget) {
            targetPos = ball.newPos;
            target = &ball;
            if (target->newPos.x - target->radius < leftBoundary) {
                targetPos.x += enemy.width;
            } else if (target->newPos.x + target->radius > rightBoundary) {
                targetPos.x -= enemy.width;
            }
            SDL_assert(ball.assignedPaddle != &enemy);
        }
    }
    return targetPos;
}

void moveToward(Paddle& enemy, const Vec2& target, float delta, float leftBoundary, float rightBoundary) {
    Vec2 acceleration;
    Vec2 diff = target - enemy.newPos;
    if (diff.x > 0) {
        if (enemy.velocity.x < 0){
            enemy.velocity.x = 0.0f;
        }
        acceleration.x = 1.0;
    } else if (diff.x < 0) {
        if (enemy.velocity.x > 0){
            enemy.velocity.x = 0.0f;
        }
        acceleration.x = -1.0;
    } else {
        enemy.velocity += -0.02 * enemy.velocity;
    }
    
    acceleration *= enemy.speed;
    
    enemy.oldPos = enemy.newPos;
    enemy.movementDelta = (0.5f * acceleration * pow(delta, 2) + enemy.velocity * delta);
    enemy.velocity += acceleration * delta;
    enemy.newPos = enemy.oldPos + enemy.movementDelta;
    
    float offset = enemy.width * 0.5f;
    if (enemy.newPos.x - offset < leftBoundary) {
        Vec2 wallNorm(1, 0);
        enemy.velocity = enemy.velocity - 2 * enemy.velocity.dotProduct(wallNorm) * wallNorm;
        enemy.movementDelta.x += 1.0f;
    } else if (enemy.newPos.x + offset > rightBoundary) {
        Vec2 wallNorm(-1, 0);
        enemy.velocity = enemy.velocity - 2 * enemy.velocity.dotProduct(wallNorm) * wallNorm;
        enemy.movementDelta.x -= 1.0f;
    }
    
}

void updateEnemy(Paddle& enemy, std::vector<Ball>& balls, float delta, float leftBoundary, float rightBoundary) {
    bool isDanger = danger(balls);
    switch (enemy.brain.state) {
        case none: {
            if (enemy.ballIndex != INVALID_INDEX && !isDanger) {
                enemy.brain.steeringPos = enemy.newPos;
                enemy.brain.steeringPos.x = 100.0f; //TODO: select random pos
                enemy.brain.state = steering;
            } else if (isDanger) {
                enemy.brain.state = defending;
            }
            std::cout << "none" << std::endl;
        } break;
        case steering: {
            bool reached = enemy.brain.steeringPos.distance(enemy.newPos) < enemy.width * 0.5f;
            if (!isDanger && !reached) {
                moveToward(enemy, enemy.brain.steeringPos, delta, leftBoundary, rightBoundary);
            } else {
                if (reached) {
                    Ball& assignedBall = balls.at(enemy.ballIndex);
                    assignedBall.velocity = Vec2(0.0f, -1.0f);
                    assignedBall.assignedPaddle = nullptr;
                    enemy.ballIndex = INVALID_INDEX;
                }
                enemy.brain.state = isDanger ? defending : none;
            }
            std::cout << "steering" << std::endl;
        }break;
        case defending: {
            if (!isDanger) {
                enemy.brain.state = none;
            } else {
                Vec2 target = getTargetPosition(enemy, balls, leftBoundary, rightBoundary);
                moveToward(enemy, target, delta, leftBoundary, rightBoundary);
            }
            std::cout << "defending" << std::endl;
        } break;
        default:
            break;
    }
}

void initPaddle(Paddle &paddle) {
    paddle.width = DEFAULT_WIDTH;
    paddle.height = DEFAULT_HEIGHT;
    paddle.newPos = Vec2(SCREEN_WIDTH / 2, 20);
    paddle.speed = DEFAULT_SPEED;
    paddle.orientation = lower;
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

bool collide(Vec2& ballCollisionPos, Vec2& paddleCollisionPos, const Paddle& paddle, float radius) {
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
            if (paddle.speed == DEFAULT_SPEED) {
                paddle.speed = HIGH_SPEED;
            } else if (paddle.speed == LOW_SPEED) {
                paddle.speed = DEFAULT_SPEED;
            }
            printf("speed up\n");
            break;
        case slowDown:
            if (paddle.speed == DEFAULT_SPEED) {
                paddle.speed = LOW_SPEED;
            } else if (paddle.speed == HIGH_SPEED) {
                paddle.speed = DEFAULT_SPEED;
            }
            printf("slow down \n");
            break;
        case strech:
            if (paddle.width == DEFAULT_WIDTH) {
                paddle.width = LARGE_WIDTH;
            } else if (paddle.width == SMALL_WIDTH) {
                paddle.width = DEFAULT_WIDTH;
            }
            printf("strech \n");
            break;
        case shrink:
            if (paddle.width == DEFAULT_WIDTH) {
                paddle.width = SMALL_WIDTH;
            } else if (paddle.width == LARGE_WIDTH) {
                paddle.width = DEFAULT_WIDTH;
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

void resolveCollision(std::vector<Ball>& balls, Paddle& paddle, float delta) {
    for (auto& ball : balls) {
        if (collide(ball.newPos, paddle.newPos, paddle, ball.radius)) {
            for (int i = 1; i <= 4; ++i) {
                Vec2 ballCollisionLocation = (1.0f / i) * ball.oldPos + (1.0f - 1.0f / i) * ball.newPos;
                Vec2 paddleCollisionLocation = (1.0f / i) * paddle.oldPos + (1.0f - 1.0f / i) * paddle.newPos;
                if (collide(ballCollisionLocation, paddleCollisionLocation, paddle, ball.radius)) {
                    Vec2 reflection = ballCollisionLocation - paddleCollisionLocation;
                    Vec2 reflectionInverse = paddleCollisionLocation - ballCollisionLocation;
                    reflection.normalize();
                    //ignore vertical reflection
                    reflectionInverse.y = 0.0f;
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