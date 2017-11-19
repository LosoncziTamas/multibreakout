#ifndef MultiBreakout_hpp
#define MultiBreakout_hpp

#include <SDL2/SDL_stdinc.h>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

struct GameState;

struct GameInput
{
    bool mouseLeft;
    bool mouseRight;
    int32_t mouseX;
    int32_t mouseY;
    
    union
    {
        bool buttons[6];
        struct
        {
            bool space;
            bool up;
            bool down;
            bool left;
            bool right;
            bool pause;
        };
    };
};

extern "C" void gameUpdate(GameState &gameState);

#endif