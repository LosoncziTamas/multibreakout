#ifndef Paddle_hpp
#define Paddle_hpp

#include "Vec2.hpp"

struct Paddle {
    Vec2 centerPos;
    Vec2 velocity;
    float width;
    float height;
};

std::ostream& operator<< (std::ostream& stream, const Paddle& paddle);

#endif