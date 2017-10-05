#include <math.h>

#include "Brick.hpp"
#include "MultiBreakout.hpp"

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
            Brick brick = {Vec2(pivotX + (i * brickWidth) + halfWidth, pivotY - (j * brickHeight) + halfHeight), brickWidth, brickHeight, true};
            bricks.push_back(brick);
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

void collideWithBrick(Ball& ball, Brick& brick) {
    if (!brick.active)
        return;
    if (collide(ball, brick)) {
        Vec2 reflectionNorm = ball.newPos - brick.center;
        reflectionNorm.normalize();
        ball.velocity = reflectionNorm;
        ball.movementDelta += reflectionNorm;
        brick.active = false;
    }
}