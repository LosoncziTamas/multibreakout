#ifndef Physics_hpp
#define Physics_hpp

#include "Vec2.hpp"
#include "Paddle.hpp"

struct Rectangle {
    Vec2 bottomLeft;
    Vec2 topRight;
};

Vec2 getCenter(Rectangle* rect);

Rectangle fromDimAndCenter(Vec2 center, float width, float height);

bool isInRectangle(Rectangle rectangle, Vec2 test);

bool aabb(Rectangle rectA, Rectangle rectB);

bool circleRectIntersect(Vec2 circleCenter, float radius, Vec2 rectCenter, float rectWidth, float rectHeight);

Vec2 reflect(const Vec2& vec, const Vec2& norm);

#endif
