#include "Paddle.hpp"

std::ostream& operator<< (std::ostream& stream, const Paddle& paddle) {
    stream << "vel: " << paddle.velocity << " pos: " << paddle.centerPos;
    return stream;
}