#include "multibreakout_math.h"

Vec2 operator*(float real, Vec2 vec)
{
    Vec2 result;

    result.x = real * vec.x;
    result.y = real * vec.y;

    return result;
}

Vec2 operator+(Vec2 lhs, Vec2 rhs)
{
    Vec2 result;

    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;

    return(result);
}

Vec2 operator-(Vec2 lhs, Vec2 rhs)
{
    Vec2 result;

    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;

    return(result);
}

Vec2 operator*(Vec2 vec, float real)
{
    Vec2 result = real * vec;

    return result;
}

Vec2 vec2(float x, float y)
{
    Vec2 result;

    result.x = x;
    result.y = y;

    return result;
}

Rect rectFromDimAndCenter(Vec2 dim, Vec2 center)
{
    Rect result;

    result.bottomLeftCorner.x = center.x - dim.x * 0.5f;
    result.bottomLeftCorner.y = center.y - dim.y * 0.5f;
    result.topRightCorner.x = center.x + dim.x * 0.5f;
    result.topRightCorner.y = center.y + dim.y * 0.5f;

    return result;
}

Rect addRadiusToRect(Rect rect, Vec2 radius)
{
    Rect result;

    result.bottomLeftCorner = rect.bottomLeftCorner - radius;
    result.topRightCorner = rect.topRightCorner + radius;

    return(result);
}

Vec2 getRectangleDim(Rect rect)
{
    Vec2 result;

    result.x = rect.topRightCorner.x - rect.bottomLeftCorner.x;
    result.y = rect.topRightCorner.y - rect.bottomLeftCorner.y;
    
    return result;
}