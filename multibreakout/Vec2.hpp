#ifndef Vec2_hpp
#define Vec2_hpp

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

};

Vec2 operator*(const Vec2& lhs, float rhs);
Vec2 operator*(float lhs, const Vec2& rhs);

#endif
