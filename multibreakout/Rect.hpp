#ifndef Rect_hpp
#define Rect_hpp

#include <SDL2/SDL_rect.h>

#include "MultiBreakout.hpp"

struct Rect {
    int x;
    int y;
    int w;
    int h;
};

SDL_Rect toSdlRect(const Rect& rect);
bool contains(const Rect& rect, const Vec2& vec);

#endif
