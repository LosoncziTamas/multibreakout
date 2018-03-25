#include "multibreakout_math.h"

#include <SDL2/SDL_stdinc.h>

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

    return (result);
}

Vec2 operator-(Vec2 lhs, Vec2 rhs)
{
    Vec2 result;

    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;

    return (result);
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

float dotProduct(Vec2 lhs, Vec2 rhs)
{
    float result = lhs.x * rhs.x + lhs.y * rhs.y;

    return result;
}

float lengthSquare(Vec2 vec)
{
    float result = dotProduct(vec, vec);

    return result;
}

float length(Vec2 vec)
{
    float result = SDL_sqrtf(lengthSquare(vec));
    
    return result;
}

Vec2 normalize(Vec2 vec)
{
    float vecLength = length(vec);
    if (vecLength == 0)
    {
        vecLength = 1.0f;
    }
    Vec2 result = vec * (1.0f / vecLength);

    return result;
}

Vec2 reflect(Vec2 vec, Vec2 norm) {
    return vec - 2 * dotProduct(vec, norm) * norm;
}

float square(float x)
{
    return SDL_pow(x, 2.0f);
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

    return (result);
}

Vec2 getRectangleDim(Rect rect)
{
    Vec2 result;

    result.x = rect.topRightCorner.x - rect.bottomLeftCorner.x;
    result.y = rect.topRightCorner.y - rect.bottomLeftCorner.y;

    return result;
}

bool rectsOverlap(Rect rectA, Rect rectB)
{
    bool result = ((rectA.topRightCorner.x >= rectB.bottomLeftCorner.x) &&
                       (rectA.bottomLeftCorner.x <= rectB.topRightCorner.x) &&
                       (rectA.bottomLeftCorner.y <= rectB.topRightCorner.y) &&
                       (rectA.topRightCorner.y >= rectB.bottomLeftCorner.y));
    return result;
}

Vec2 &operator*=(Vec2 &vec, float real)
{
    vec = vec * real;

    return (vec);
}

Vec2 &operator+=(Vec2 &lhs, Vec2 rhs)
{
    lhs = lhs + rhs;

    return (lhs);
}