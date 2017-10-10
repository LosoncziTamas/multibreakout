#ifndef Vec2_hpp
#define Vec2_hpp

#include <iostream>

struct Vec2 {
    
    float x;
    float y;
    
    Vec2();
    Vec2(float x, float y);
    Vec2& operator=(const Vec2& other) = default;
    
    Vec2 operator+(const Vec2& rhs) const;
    Vec2 operator-(const Vec2& rhs) const;
    Vec2& operator+=(const Vec2& rhs);
    Vec2& operator-=(const Vec2& rhs);
    Vec2& operator*=(float rhs);
    float dotProduct(const Vec2& rhs) const;
    float length() const;
    float sqrLength() const;
    float distance(const Vec2& other) const;
    Vec2& normalize();

};

Vec2 operator*(const Vec2& lhs, float rhs);
Vec2 operator*(float lhs, const Vec2& rhs);
std::ostream& operator<< (std::ostream& stream, const Vec2& vec);

#endif
