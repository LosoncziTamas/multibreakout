#ifndef Physics_hpp
#define Physics_hpp

#include "Vec2.hpp"
#include "Paddle.hpp"

bool circleRectIntersect(Vec2 circleCenter, float radius, Vec2 rectCenter, float rectWidth, float rectHeight);
Vec2 reflect(const Vec2& vec, const Vec2& norm);
void acceleratePaddle(Vec2& acceleration, Paddle& paddle, float delta);

#endif
