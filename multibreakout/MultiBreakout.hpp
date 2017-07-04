#ifndef MultiBreakout_hpp
#define MultiBreakout_hpp

#include <SDL2/SDL.h>

#include "Renderer.hpp"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

struct GameInput
{
    union
    {
        SDL_bool buttons[4];
        struct
        {
            SDL_bool up;
            SDL_bool down;
            SDL_bool left;
            SDL_bool right;
        };
    };
    int32_t mouseX;
    int32_t mouseY;
};

void gameUpdate(const GameInput& input, const Renderer& renderer, float delta);

#endif