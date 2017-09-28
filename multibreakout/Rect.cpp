#include "Rect.hpp"

bool contains(const Rect& rect, const Vec2& vec) {
    bool horizontal = vec.x >= rect.x && vec.x <= rect.x + rect.w;
    bool vertical = vec.y >= rect.y && vec.y <= rect.y + rect.h;
    
    return horizontal && vertical;
}

SDL_Rect toSdlRect(const Rect& rect) {
    SDL_Rect sdlRect = {rect.x, SCREEN_HEIGHT - rect.y - rect.h, rect.w, rect.h};
    return sdlRect;
}