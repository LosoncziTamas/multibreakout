#include "Physics.hpp"
#include <math.h>

bool circleRectIntersect(Vec2 circleCenter, float radius, Vec2 rectCenter, float rectWidth, float rectHeight) {
    float verticalDist = fabsf(circleCenter.y - rectCenter.y);
    float horizontalDist = fabsf(circleCenter.x - rectCenter.x);
    float halfWidth = rectWidth * 0.5f;
    float halfHeight = rectHeight * 0.5f;
    
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

Vec2 reflect(const Vec2& vec, const Vec2& norm) {
    return vec - 2 * vec.dotProduct(norm) * norm;
}

void acceleratePaddle(Vec2& acceleration, Paddle& paddle, float delta) {
    acceleration *= paddle.speed;
    paddle.oldPos = paddle.newPos;
    paddle.movementDelta = (0.5f * acceleration * pow(delta, 2) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    paddle.newPos = paddle.oldPos + paddle.movementDelta;
}