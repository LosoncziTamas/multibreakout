#include <math.h>
#include <SDL2/SDL_assert.h>

#include "Brick.hpp"
#include "MultiBreakout.hpp"

void callback(Brick& brick, Ball& ball) {
    switch (brick.powerUp) {
        case speedUp:
        case slowDown:
        case strech:
        case shrink:
        case neutral:
            break;
        default:
            SDL_assert(false);
            break;
    }
    ball.powerUp = brick.powerUp;
    brick.active = false;
}

void initBricks(std::vector<Brick>& bricks) {
    int brickWidth = 30;
    int brickHeight = 30;
    int columns = 4;
    int rows = 5;
    float halfWidth = brickWidth * 0.5f;
    float halfHeight = brickHeight * 0.5f;
    float pivotX = SCREEN_WIDTH * 0.5f - columns * halfHeight;
    float pivotY = SCREEN_HEIGHT * 0.5f + rows * halfWidth;
    
    for (int i = 0; i < columns; ++i) {
        for (int j = 0; j < rows; ++j) {
            Brick brick;
            brick.center = Vec2(pivotX + (i * brickWidth) + halfWidth, pivotY - (j * brickHeight) + halfHeight);
            brick.width = brickWidth;
            brick.height = brickHeight;
            brick.active = true;
            brick.callback = callback;
            brick.powerUp = static_cast<PowerUp>(rand() % shrink + 1);
            bricks.push_back(brick);
            brick.textureIndex = INVALID_INDEX;
        }
    }
}


bool collide(Ball& ball, Brick& brick) {
    float verticalDist = fabsf(ball.newPos.y - brick.center.y);
    float horizontalDist = fabsf(ball.newPos.x - brick.center.x);
    float halfWidth = brick.width * 0.5f;
    float halfHeight = brick.height * 0.5f;
    
    if (horizontalDist > ball.radius + halfWidth) {
        return false;
    }
    
    if (verticalDist > ball.radius + halfHeight) {
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
    
    return (dx * dx + dy * dy <= ball.radius * ball.radius);
}

void collideWithBrick(std::vector<Ball>& balls, std::vector<Brick>& bricks) {
    for (auto& ball : balls) {
        for (auto& brick : bricks) {
            if (!brick.active)
                continue;
            if (collide(ball, brick)) {
                Vec2 reflectionNorm = ball.newPos - brick.center;
                reflectionNorm.normalize();
                ball.velocity = reflectionNorm;
                ball.movementDelta += reflectionNorm;
                brick.callback(brick, ball);
            }
        }
    }
}