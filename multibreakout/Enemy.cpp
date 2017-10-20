#include "Enemy.hpp"
#include <SDL2/SDL_assert.h>

static void initCommon(Enemy& enemy) {
    enemy.paddle.speed = DEFAULT_SPEED;
    enemy.state = none;
    enemy.paddle.textureIndex = INVALID_INDEX;
    enemy.paddle.ballIndex = INVALID_INDEX;
}

void initLeftEnemy(Enemy& enemy, float leftBoundary) {
    initCommon(enemy);
    enemy.paddle.orientation = left;
    enemy.paddle.height = DEFAULT_WIDTH;
    enemy.paddle.width = DEFAULT_HEIGHT;
    enemy.paddle.newPos = Vec2(leftBoundary + enemy.paddle.width * 0.5f, SCREEN_HEIGHT * 0.5f);
    enemy.paddle.oldPos = enemy.paddle.newPos;
    enemy.steeringPos = enemy.paddle.newPos;
}

void initRightEnemy(Enemy& enemy, float rightBoundary) {
    initCommon(enemy);
    enemy.paddle.orientation = right;
    enemy.paddle.height = DEFAULT_WIDTH;
    enemy.paddle.width = DEFAULT_HEIGHT;
    enemy.paddle.newPos = Vec2(rightBoundary - enemy.paddle.width * 0.5f, SCREEN_HEIGHT * 0.5f);
    enemy.paddle.oldPos = enemy.paddle.newPos;
    enemy.steeringPos = enemy.paddle.newPos;
}

void initUpperEnemy(Enemy& enemy) {
    initCommon(enemy);
    enemy.paddle.orientation = upper;
    enemy.paddle.width = DEFAULT_WIDTH;
    enemy.paddle.height = DEFAULT_HEIGHT;
    enemy.paddle.newPos = Vec2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT - enemy.paddle.height * 0.5f);
    enemy.paddle.oldPos = enemy.paddle.newPos;
    enemy.steeringPos = enemy.paddle.newPos;
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
        if (ball.newPos.y > SCREEN_HEIGHT * 0.5f && betterTarget && ball.assignedPaddle == nullptr) {
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

void moveTowardHorizontal(Paddle& enemy, const Vec2& target, float delta, float leftBoundary, float rightBoundary) {
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

Vec2 getLeftTargetPosition(Paddle& enemy, std::vector<Ball>& balls) {
    Vec2 targetPos(enemy.newPos);
    Ball *target = nullptr;
    
    for (auto& ball : balls) {
        bool betterTarget = target == nullptr || (target->newPos.x > ball.newPos.x && ball.velocity.x < 0);
        if (ball.newPos.x < SCREEN_WIDTH * 0.5f && betterTarget && ball.assignedPaddle == nullptr) {
            targetPos = ball.newPos;
            target = &ball;
            if (target->newPos.y - target->radius < 0) {
                targetPos.y += enemy.height;
            } else if (target->newPos.y + target->radius > SCREEN_HEIGHT) {
                targetPos.y -= enemy.height;
            }
            SDL_assert(ball.assignedPaddle != &enemy);
        }
    }
    return targetPos;
}

void moveTowardVertical(Paddle& enemy, const Vec2& target, float delta, float topBoundary, float bottomBoundary) {
    Vec2 acceleration;
    Vec2 diff = target - enemy.newPos;
    if (diff.y > 0) {
        if (enemy.velocity.y < 0){
            enemy.velocity.y = 0.0f;
        }
        acceleration.y = 1.0;
    } else if (diff.y < 0) {
        if (enemy.velocity.y > 0){
            enemy.velocity.y = 0.0f;
        }
        acceleration.y = -1.0;
    } else {
        enemy.velocity += -0.02 * enemy.velocity;
    }
    
    acceleration *= enemy.speed;
    
    enemy.oldPos = enemy.newPos;
    enemy.movementDelta = (0.5f * acceleration * pow(delta, 2) + enemy.velocity * delta);
    enemy.velocity += acceleration * delta;
    enemy.newPos = enemy.oldPos + enemy.movementDelta;
    
    float offset = enemy.height * 0.5f;
    if (enemy.newPos.y - offset < bottomBoundary) {
        Vec2 wallNorm(0.0f, 1.0f);
        enemy.velocity = enemy.velocity - 2 * enemy.velocity.dotProduct(wallNorm) * wallNorm;
        enemy.movementDelta.y += 1.0f;
    } else if (enemy.newPos.y + offset > topBoundary) {
        Vec2 wallNorm(0.0f, -1.0f);
        enemy.velocity = enemy.velocity - 2 * enemy.velocity.dotProduct(wallNorm) * wallNorm;
        enemy.movementDelta.y -= 1.0f;
    }
}

bool leftDanger(std::vector<Ball>& balls) {
    for (auto& ball : balls) {
        if (ball.newPos.x < SCREEN_WIDTH * 0.5f && ball.assignedPaddle == nullptr) {
            return true;
        }
    }
    return false;
}

void updateLeftEnemy(Enemy& enemy, std::vector<Ball>& balls, float delta, Obstacles& obstacles) {
    float topBoundary = obstacles.leftTop.center.y - obstacles.leftTop.height * 0.5;
    float bottomBoundary = obstacles.leftBottom.center.y + obstacles.leftBottom.height * 0.5;
    bool isDanger = leftDanger(balls);
    
    switch (enemy.state) {
        case none: {
            if (enemy.paddle.ballIndex != INVALID_INDEX && !isDanger) {
                enemy.steeringPos = enemy.paddle.newPos;
                int randomY = SCREEN_HEIGHT * 0.5f;
                enemy.steeringPos.y = randomY;
                enemy.state = steering;
            } else if (isDanger) {
                enemy.state = defending;
            }
        } break;
        case steering: {
            bool reached = enemy.steeringPos.distance(enemy.paddle.newPos) <= enemy.paddle.height;
            if (!isDanger && !reached) {
                moveTowardVertical(enemy.paddle, enemy.steeringPos, delta, topBoundary, bottomBoundary);
            } else {
                if (reached) {
                    Ball& assignedBall = balls.at(enemy.paddle.ballIndex);
                    assignedBall.velocity = Vec2(1.0f, 0.0f);
                    assignedBall.assignedPaddle = nullptr;
                    enemy.paddle.ballIndex = INVALID_INDEX;
                }
                enemy.state = isDanger ? defending : none;
            }
        }break;
        case defending: {
            if (!isDanger) {
                enemy.state = none;
            } else {
                Vec2 target = getLeftTargetPosition(enemy.paddle, balls);
                moveTowardVertical(enemy.paddle, target, delta, topBoundary, bottomBoundary);
            }
        } break;
        default:
            break;
    }
}

Vec2 getRightTargetPosition(Paddle& enemy, std::vector<Ball>& balls) {
    Vec2 targetPos(enemy.newPos);
    Ball *target = nullptr;
    
    for (auto& ball : balls) {
        bool betterTarget = target == nullptr || (target->newPos.x < ball.newPos.x && ball.velocity.x > 0);
        if (ball.newPos.x > SCREEN_WIDTH * 0.5f && betterTarget && ball.assignedPaddle == nullptr) {
            targetPos = ball.newPos;
            target = &ball;
            if (target->newPos.y - target->radius < 0) {
                targetPos.y += enemy.height;
            } else if (target->newPos.y + target->radius > SCREEN_HEIGHT) {
                targetPos.y -= enemy.height;
            }
            SDL_assert(ball.assignedPaddle != &enemy);
        }
    }
    return targetPos;
}

bool rightDanger(std::vector<Ball>& balls) {
    for (auto& ball : balls) {
        if (ball.newPos.x > SCREEN_WIDTH * 0.5f && ball.assignedPaddle == nullptr) {
            return true;
        }
    }
    return false;
}

void updateRightEnemy(Enemy& enemy, std::vector<Ball>& balls, float delta, Obstacles& obstacles) {
    float topBoundary = obstacles.rightTop.center.y - obstacles.rightTop.height * 0.5;
    float bottomBoundary = obstacles.rightBottom.center.y + obstacles.rightBottom.height * 0.5;
    bool isDanger = rightDanger(balls);
    
    switch (enemy.state) {
        case none: {
            if (enemy.paddle.ballIndex != INVALID_INDEX && !isDanger) {
                enemy.steeringPos = enemy.paddle.newPos;
                int randomY = rand() % SCREEN_HEIGHT + 1;
                enemy.steeringPos.y = randomY;
                enemy.state = steering;
            } else if (isDanger) {
                enemy.state = defending;
            }
        } break;
        case steering: {
            bool reached = enemy.steeringPos.distance(enemy.paddle.newPos) <= enemy.paddle.height;
            if (!isDanger && !reached) {
                moveTowardVertical(enemy.paddle, enemy.steeringPos, delta, topBoundary, bottomBoundary);
            } else {
                if (reached) {
                    Ball& assignedBall = balls.at(enemy.paddle.ballIndex);
                    assignedBall.velocity = Vec2(-1.0f, 0.0f);
                    assignedBall.assignedPaddle = nullptr;
                    enemy.paddle.ballIndex = INVALID_INDEX;
                }
                enemy.state = isDanger ? defending : none;
            }
        }break;
        case defending: {
            if (!isDanger) {
                enemy.state = none;
            } else {
                Vec2 target = getRightTargetPosition(enemy.paddle, balls);
                moveTowardVertical(enemy.paddle, target, delta, topBoundary, bottomBoundary);
            }
        } break;
        default:
            break;
    }
}

void updateEnemy(Enemy& enemy, std::vector<Ball>& balls, float delta, Obstacles& obstacles) {
    float leftBoundary = obstacles.leftTop.center.x + obstacles.leftTop.width * 0.5;
    float rightBoundary = obstacles.rightTop.center.x - obstacles.rightTop.width * 0.5;
    bool isDanger = danger(balls);
    
    switch (enemy.state) {
        case none: {
            if (enemy.paddle.ballIndex != INVALID_INDEX && !isDanger) {
                enemy.steeringPos = enemy.paddle.newPos;
                int randomX = leftBoundary + (rand() % static_cast<int>(rightBoundary - leftBoundary) + 1);
                enemy.steeringPos.x = randomX;
                enemy.state = steering;
            } else if (isDanger) {
                enemy.state = defending;
            }
        } break;
        case steering: {
            bool reached = enemy.steeringPos.distance(enemy.paddle.newPos) < enemy.paddle.width;
            if (!isDanger && !reached) {
                moveTowardHorizontal(enemy.paddle, enemy.steeringPos, delta, leftBoundary, rightBoundary);
            } else {
                if (reached) {
                    Ball& assignedBall = balls.at(enemy.paddle.ballIndex);
                    assignedBall.velocity = Vec2(0.0f, -1.0f);
                    assignedBall.assignedPaddle = nullptr;
                    enemy.paddle.ballIndex = INVALID_INDEX;
                }
                enemy.state = isDanger ? defending : none;
            }
        }break;
        case defending: {
            if (!isDanger) {
                enemy.state = none;
            } else {
                Vec2 target = getTargetPosition(enemy.paddle, balls, leftBoundary, rightBoundary);
                moveTowardHorizontal(enemy.paddle, target, delta, leftBoundary, rightBoundary);
            }
        } break;
        default:
            break;
    }
}