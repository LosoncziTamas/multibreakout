#ifndef MULTIBREAKOUT_MATH_H
#define MULTIBREAKOUT_MATH_H

struct Vec2
{
    float x;
    float y;
};

struct Rect
{
    Vec2 bottomLeftCorner;
    Vec2 topRightCorner;
};

float square(float x);

Rect rectFromDimAndCenter(Vec2 dim, Vec2 center);
Rect addRadiusToRect(Rect rect, Vec2 radius);
bool rectsOverlap(Rect rectA, Rect rectB);

Vec2 vec2(float x, float y);
float dotProduct(Vec2 lhs, Vec2 rhs);
float lengthSquare(Vec2 vec);
float length(Vec2 vec);
Vec2 reflect(Vec2 vec, Vec2 norm);
Vec2 getRectangleDim(Rect rect);

Vec2 operator*(float real, Vec2 vec);
Vec2 operator*(Vec2 vec, float real);
Vec2 operator+(Vec2 lhs, Vec2 rhs);
Vec2 operator-(Vec2 lhs, Vec2 rhs);
Vec2 &operator*=(Vec2 &vec, float real);
Vec2 &operator+=(Vec2 &lhs, Vec2 rhs);

#endif