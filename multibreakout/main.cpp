#include <iostream>
#include <SDL2/SDL.h>
#include <sys/stat.h>

#include "Renderer.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

#define HOTLOAD 1

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 480;
const float TARGET_UPDATE_HZ = 30.0f;

typedef void (*gameUpdateFn)(GameState *gameState);

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

bool loadDll(GameCode& appDll, const char *dllPath){
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

static SDL_bool handleEvent(SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            return SDL_FALSE;
        default:
            break;
    }
    
    return SDL_TRUE;
}

static float secondsElapsed(Uint64 old, Uint64 current) {
    return SDL_static_cast(float, current - old) / SDL_static_cast(float, SDL_GetPerformanceFrequency());
}


int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Multibreakout",
                                          SDL_WINDOWPOS_CENTERED_MASK,
                                          SDL_WINDOWPOS_CENTERED_MASK,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_RESIZABLE);
    SDL_assert(window);
    
    bool running = true;
    float secondsPerFrame = 1.0f / TARGET_UPDATE_HZ;
    Uint64 startCounter = SDL_GetPerformanceCounter();
    Uint64 perfCountFreq = SDL_GetPerformanceFrequency();
    
    size_t totalSize = 1024 * 1024 * 16;
    void *memory = SDL_malloc(totalSize);
    SDL_assert(memory && totalSize > sizeof(GameState));
    
    GameState *gameState = (GameState*)memory;
    
    Memory gameMemory = {};
    gameMemory.size = totalSize - sizeof(GameState);
    gameMemory.used = 0;
    gameMemory.base = (Uint8*) memory + sizeof(GameState);
    
    gameState->gameMemory = gameMemory;
    gameState->renderer = createRenderer(window);
    
    GameCode gameCode = {};
    time_t lastWrite = 0;
    bool loadingDll = true;
    const char *dllPath = "game.so";
    
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            running = handleEvent(event);
        }
        
        gameState->oldInput = gameState->input;
        gameState->input = {};
        
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        gameState->input.left = state[SDL_SCANCODE_LEFT];
        gameState->input.down = state[SDL_SCANCODE_DOWN];
        gameState->input.up = state[SDL_SCANCODE_UP];
        gameState->input.right = state[SDL_SCANCODE_RIGHT];
        gameState->input.space = state[SDL_SCANCODE_SPACE];
        gameState->input.pause = state[SDL_SCANCODE_P];
        
        Uint32 mouseVal = SDL_GetMouseState(&gameState->input.mouseX, &gameState->input.mouseY);
        gameState->input.mouseLeft = mouseVal & SDL_BUTTON(SDL_BUTTON_LEFT);
        gameState->input.mouseRight = mouseVal & SDL_BUTTON(SDL_BUTTON_RIGHT);
        
        while (secondsElapsed(startCounter, SDL_GetPerformanceCounter()) < secondsPerFrame);
        
        Uint64 end_counter = SDL_GetPerformanceCounter();
        gameState->delta = SDL_static_cast(float, (end_counter - startCounter)) / SDL_static_cast(float, perfCountFreq);
        
#if HOTLOAD
        time_t time = getLastWriteTime(dllPath);
        
        if (time > lastWrite) {
            lastWrite = time;
            loadingDll = true;
        }
        
        if (loadingDll) {
            if(loadDll(gameCode, dllPath)) {
                loadingDll = false;
            }
        }
        else if(gameCode.dll) {
            gameCode.update(gameState);
        }
#else
        gameUpdate(gameState);
#endif
        
        startCounter = end_counter;
    }
    
    SDL_free(memory);
    SDL_DestroyRenderer(gameState->renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
