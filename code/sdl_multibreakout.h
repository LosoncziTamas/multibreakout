#ifndef SDL_MULTIBREAKOUT_H
#define SDL_MULTIBREAKOUT_H

#include <SDL2/SDL.h>

struct GameMemory
{
    bool initialized;
    
    Uint64 permanentMemorySize;
    void *permanentMemory;
    
    Uint64 transientMemorySize;
    void *transientMemory;
};

struct PlatformState
{
    Uint32 totalSize;
    void* gameMemoryBlock;
    
    SDL_RWops *inputRecordHandler;
    SDL_RWops *inputPlaybackHandler;
    
    bool recording;
    bool replaying;
};

struct GameInput
{
    bool mouseLeft;
    bool mouseRight;

    int32_t mouseX;
    int32_t mouseY;

    float deltaTime;
    
    union
    {
        bool buttons[8];
        struct
        {
            bool record;
            bool replay;
            bool space;
            bool up;
            bool down;
            bool left;
            bool right;
            bool pause;
        };
    };
};

typedef void (*gameUpdateFn)(GameMemory *gameMemory, GameInput *GameInput, SDL_Renderer *renderer);

struct GameCode
{
    void *dll;
    time_t lastLoadTime;
    gameUpdateFn update;
};

#endif