#include <iostream>
#include <SDL2/SDL.h>
#include <sys/stat.h>

#include "Renderer.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 480;
const float TARGET_UPDATE_HZ = 30.0f;

typedef void (*gameUpdateFn)(GameState& gameState, Renderer& renderer);

struct GameCode {
    void *dll;
    time_t lastLoadTime;
    gameUpdateFn update;
};

time_t getLastWriteTime(const char *fileName) {
    struct stat fileAttrs;
    if (stat(fileName, &fileAttrs) < 0) {
        return 0;
    }
    
    return fileAttrs.st_mtime;
}

bool loadDll(GameCode& appDll, const char *dllPath)
{
    time_t time = getLastWriteTime(dllPath);
    
    if (appDll.lastLoadTime >= time) {
        return false;
    } else {
        if (appDll.dll) {
            SDL_UnloadObject(appDll.dll);
            appDll.dll = 0;
        }
    }
    void *lib = SDL_LoadObject(dllPath);
    if (!lib) {
        return false;
    }
    
    appDll.dll = lib;
    appDll.update = (gameUpdateFn)SDL_LoadFunction(lib, "gameUpdate");
    appDll.lastLoadTime = time;
    return true;
}

static SDL_bool handleEvent(SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_QUIT:
        {
            return SDL_FALSE;
        }
        case SDL_WINDOWEVENT:
        {
            switch(event.window.event)
            {
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    
                } break;
            }
        } break;
    }
    
    return SDL_TRUE;
}

static float secondsElapsed(Uint64 old, Uint64 current)
{
    return static_cast<float>(current - old) / static_cast<float>(SDL_GetPerformanceFrequency());
}

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Multibreakout",
                                          SDL_WINDOWPOS_CENTERED_MASK,
                                          SDL_WINDOWPOS_CENTERED_MASK,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_RESIZABLE);
    SDL_assert(window);
    
    Renderer renderer;
    createRenderer(renderer, window);
    
    bool running = true;
    float secondsPerFrame = 1.0f / TARGET_UPDATE_HZ;
    Uint64 startCounter = SDL_GetPerformanceCounter();
    Uint64 perfCountFreq = SDL_GetPerformanceFrequency();
    
    GameState gameState = {};
    
    GameCode gameCode = {};
    time_t lastWrite = 0;
    bool loadingDll = true;
    const char *dllPath = "game.so";
    
    while(running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            running = handleEvent(event);
        }
        
        GameInput input = {};
        
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        input.left = state[SDL_SCANCODE_LEFT];
        input.down = state[SDL_SCANCODE_DOWN];
        input.up = state[SDL_SCANCODE_UP];
        input.right = state[SDL_SCANCODE_RIGHT];
        input.space = state[SDL_SCANCODE_SPACE];
        input.pause = state[SDL_SCANCODE_P];
        
        Uint32 mouseVal = SDL_GetMouseState(&input.mouseX, &input.mouseY);
        input.mouseLeft = mouseVal & SDL_BUTTON(SDL_BUTTON_LEFT);
        input.mouseRight = mouseVal & SDL_BUTTON(SDL_BUTTON_RIGHT);
        
        gameState.input = input;
        
        while (secondsElapsed(startCounter, SDL_GetPerformanceCounter()) < secondsPerFrame);
        
        Uint64 end_counter = SDL_GetPerformanceCounter();
        gameState.delta = static_cast<float>(end_counter - startCounter) / static_cast<float>(perfCountFreq);
        
        time_t time = getLastWriteTime(dllPath);
        
        if (time > lastWrite)
        {
            lastWrite = time;
            loadingDll = true;
        }
        
        if (loadingDll)
        {
            if(loadDll(gameCode, dllPath))
            {
                loadingDll = false;
            }
        }
        else if(gameCode.dll)
        {
            gameCode.update(gameState, renderer);
        }
        
        startCounter = end_counter;
    }
    
    deleteRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}
