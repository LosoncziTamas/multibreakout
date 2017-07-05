#include "Vec2.hpp"
#include "Intrinsics.hpp"

Vec2::Vec2(float x, float y) : x(x), y(y) {}

Vec2::Vec2() : x(0.0f), y(0.0f) {}

Vec2 Vec2::operator+(const Vec2& rhs) const {
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs) const {
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2& Vec2::operator+=(const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vec2& Vec2::operator*=(float rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
}

float Vec2::dotProduct(const Vec2& rhs) const{
    return x * rhs.x + y * rhs.y;
}

float Vec2::length() const {
    return sqrt(x * x  + y * y);
}

float Vec2::sqrLength() const {
    return x * x  + y * y;
}

Vec2 operator*(const Vec2& lhs, float rhs) {
    return Vec2(rhs * lhs.x, rhs * lhs.y);
}

Vec2 operator*(float lhs, const Vec2& rhs) {
    return Vec2(lhs * rhs.x, lhs * rhs.y);
}