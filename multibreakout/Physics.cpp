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

Vec2 getCenter(Rectangle* rect) {
    Vec2 center = 0.5f * (rect->bottomLeft + rect->topRight);
    return center;
}

Rectangle fromDimAndCenter(Vec2 center, float width, float height) {
    Rectangle result;
    result.bottomLeft.x = center.x - width * 0.5f;
    result.bottomLeft.y = center.y - height * 0.5f;
    result.topRight.x = center.x + width * 0.5f;
    result.topRight.y = center.y + height * 0.5f;
    return result;
}

bool isInRectangle(Rectangle rectangle, Vec2 test) {
    bool result = ((test.x >= rectangle.bottomLeft.x) &&
                   (test.y >= rectangle.bottomLeft.y) &&
                   (test.x < rectangle.topRight.x) &&
                   (test.y < rectangle.topRight.y));
    return result;
}


bool aabb(Rectangle rectA, Rectangle rectB) {
    bool result = ((rectA.topRight.x >= rectB.bottomLeft.x) &&
                   (rectA.bottomLeft.x <= rectB.topRight.x) &&
                   (rectA.bottomLeft.y <= rectB.topRight.y) &&
                   (rectA.topRight.y >= rectB.bottomLeft.y));
    return result;
}