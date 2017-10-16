#ifndef MultiBreakout_hpp
#define MultiBreakout_hpp

#include <SDL2/SDL_stdinc.h>

#include "Renderer.hpp"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

struct GameState;

struct GameInput
{
    union
    {
        SDL_bool buttons[6];
        struct
        {
            SDL_bool space;
            SDL_bool up;
            SDL_bool down;
            SDL_bool left;
            SDL_bool right;
            SDL_bool pause;
        };
    };
    SDL_bool mouseLeft;
    SDL_bool mouseRight;
    int32_t mouseX;
    int32_t mouseY;
};

void gameUpdate(GameState& gameState, Renderer& renderer);

#endif