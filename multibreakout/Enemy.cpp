#include <SDL2/SDL_assert.h>

#include "Physics.hpp"
#include "Enemy.hpp"
#include "Renderer.hpp"
#include "GameState.hpp"

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
    
    acceleratePaddle(acceleration, enemy, delta);
    
    float offset = enemy.width * 0.5f;
    if (enemy.newPos.x - offset < leftBoundary) {
        Vec2 wallNorm(1, 0);
        enemy.velocity = reflect(enemy.velocity, wallNorm);
        enemy.movementDelta.x += 1.0f;
    } else if (enemy.newPos.x + offset > rightBoundary) {
        Vec2 wallNorm(-1, 0);
        enemy.velocity = reflect(enemy.velocity, wallNorm);
        enemy.movementDelta.x -= 1.0f;
    }
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
    
    acceleratePaddle(acceleration, enemy, delta);
    
    float offset = enemy.height * 0.5f;
    if (enemy.newPos.y - offset < bottomBoundary) {
        Vec2 wallNorm(0.0f, 1.0f);
        enemy.velocity = reflect(enemy.velocity, wallNorm);
        enemy.movementDelta.y += 1.0f;
    } else if (enemy.newPos.y + offset > topBoundary) {
        Vec2 wallNorm(0.0f, -1.0f);
        enemy.velocity = reflect(enemy.velocity, wallNorm);
        enemy.movementDelta.y -= 1.0f;
    }
}

void moveToward(Paddle& enemy, const Vec2& target, float delta, float leftBoundary, float rightBoundary, float topBoundary, float bottomBoundary) {
    if (enemy.orientation == upper) {
        moveTowardHorizontal(enemy, target, delta, leftBoundary, rightBoundary);
    } else {
        moveTowardVertical(enemy, target, delta, topBoundary, bottomBoundary);
    }
}

bool betterTarget(Orientation orientation, Ball* ball, Ball *currentTarget) {
    if (orientation == upper) {
        return (!currentTarget || (currentTarget->newPos.y < ball->newPos.y && ball->velocity.y > 0)) && ball->newPos.y > SCREEN_HEIGHT * 0.5f;
    } else if (orientation == left) {
        return (!currentTarget || (currentTarget->newPos.x > ball->newPos.x && ball->velocity.x < 0)) && ball->newPos.x < SCREEN_WIDTH * 0.5f;
    } else if (orientation == right) {
        return (!currentTarget || (currentTarget->newPos.x < ball->newPos.x && ball->velocity.x > 0)) && ball->newPos.x > SCREEN_WIDTH * 0.5f;
    }
    return false;
}

void setTarget(Ball *target, Orientation orientation, float leftBoundary, float rightBoundary, Paddle& enemy) {
    if (orientation == upper) {
        if (target->newPos.x - target->radius < leftBoundary) {
            target->newPos.x += enemy.width;
        } else if (target->newPos.x + target->radius > rightBoundary) {
            target->newPos.x -= enemy.width;
        }
    } else if (orientation == left || orientation == right) {
        if (target->newPos.y - target->radius < 0) {
            target->newPos.y += enemy.height;
        } else if (target->newPos.y + target->radius > SCREEN_HEIGHT) {
            target->newPos.y -= enemy.height;
        }
    }
    
}

Vec2 getTarget(Paddle& enemy, World& world, float leftBoundary, float rightBoundary) {
    Vec2 targetPos(enemy.newPos);
    Ball *target = nullptr;
    
    for (Uint32 ballIndex = 0; ballIndex < world.ballCount; ++ballIndex) {
        Ball* ball = world.balls + ballIndex;
        if (betterTarget(enemy.orientation, ball, target) && !ball->assignedPaddle) {
            targetPos = ball->newPos;
            target = ball;
            setTarget(target, enemy.orientation, leftBoundary, rightBoundary, enemy);
            SDL_assert(ball->assignedPaddle != &enemy);
        }
    }
    return targetPos;
}

bool danger(World& world, Orientation orientation) {
    for (Uint32 ballIndex = 0; ballIndex < world.ballCount; ++ballIndex) {
        Ball* ball = world.balls + ballIndex;
        if (orientation == upper) {
            if (ball->newPos.y > SCREEN_HEIGHT * 0.5f && ball->assignedPaddle == nullptr) {
                return true;
            }
        } else if (orientation == left) {
            if (ball->newPos.x < SCREEN_WIDTH * 0.5f && ball->assignedPaddle == nullptr) {
                return true;
            }
        } else if (orientation == right) {
            if (ball->newPos.x > SCREEN_WIDTH * 0.5f && ball->assignedPaddle == nullptr) {
                return true;
            }
        }
    }
    return false;
}

Vec2 generateSteeringPos(const Vec2& paddlePos, Orientation orientation, float leftBoundary, float rightBoundary, float bottomBoundary, float topBoundary) {
    Vec2 steeringPos(paddlePos);
    if (orientation == upper) {
        steeringPos.x = leftBoundary + (rand() % static_cast<int>(rightBoundary - leftBoundary) + 1);
    } else if (orientation == left || orientation == right) {
        steeringPos.y = bottomBoundary + (rand() % static_cast<int>(topBoundary - bottomBoundary) + 1);
    }
    return steeringPos;
}

bool reachedTarget(Enemy& enemy) {
    if (enemy.paddle.orientation == upper) {
        return enemy.steeringPos.distance(enemy.paddle.newPos) <= enemy.paddle.width;
    } else {
        return enemy.steeringPos.distance(enemy.paddle.newPos) <= enemy.paddle.height;
    }
}

void launchBall(World& world, Enemy& enemy) {
    SDL_assert(enemy.paddle.ballIndex < world.ballCount);
    Ball* assignedBall = world.balls + enemy.paddle.ballIndex;
    if (enemy.paddle.orientation == left) {
        assignedBall->velocity = Vec2(1.0f, 0.0f);
    } else if (enemy.paddle.orientation == right) {
        assignedBall->velocity = Vec2(-1.0f, 0.0f);
    } else if (enemy.paddle.orientation == upper) {
        assignedBall->velocity = Vec2(0.0f, -1.0f);
    } else {
        SDL_assert(false);
    }
    assignedBall->assignedPaddle = nullptr;
    enemy.paddle.ballIndex = INVALID_INDEX;
}

void updateEnemy(World& world, Enemy& enemy, float delta) {
    Obstacle* leftBottom = world.obstacles.content + world.obstacles.leftBottomIndex;
    Obstacle* leftTop = world.obstacles.content + world.obstacles.leftTopIndex;
    Obstacle* rightTop = world.obstacles.content + world.obstacles.rightTopIndex;
    
    float topBoundary = leftTop->center.y - leftTop->height * 0.5;
    float bottomBoundary = leftBottom->center.y + leftBottom->height * 0.5;
    
    float leftBoundary = leftTop->center.x + leftTop->width * 0.5;
    float rightBoundary = rightTop->center.x - rightTop->width * 0.5;
    
    bool isDanger = danger(world, enemy.paddle.orientation);
    switch (enemy.state) {
        case none: {
            if (enemy.paddle.ballIndex != INVALID_INDEX && !isDanger) {
                enemy.steeringPos = generateSteeringPos(enemy.paddle.newPos, enemy.paddle.orientation, leftBoundary, rightBoundary, bottomBoundary, topBoundary);
                enemy.state = steering;
            } else if (isDanger) {
                enemy.state = defending;
            }
        } break;
        case steering: {
            bool reached = reachedTarget(enemy);
            if (isDanger && !reached) {
                moveToward(enemy.paddle, enemy.steeringPos, delta, topBoundary, bottomBoundary, leftBoundary, rightBoundary);
            } else {
                if (reached) {
                    launchBall(world, enemy);
                }
                enemy.state = isDanger ? defending : none;
            }
        }break;
        case defending: {
            if (!isDanger) {
                enemy.state = none;
            } else {
                Vec2 target = getTarget(enemy.paddle, world, leftBoundary, rightBoundary);
                moveToward(enemy.paddle, target, delta, leftBoundary, rightBoundary, topBoundary, bottomBoundary);
            }
        } break;
        default:
            break;
    }
}

const char *aiStates[] =
{
    "none",
    "steering",
    "defending"
};

void drawDebugInfo(SDL_Renderer* renderer, FC_Font* font, World& world, float delta) {
    FC_Draw(font, renderer, 0, 0, "delta: %f\nleft ai: %s\nright ai: %s\ntop ai: %s\n ",
            delta,
            aiStates[world.enemyLeft.state],
            aiStates[world.enemyRight.state],
            aiStates[world.enemyUpper.state]);
}