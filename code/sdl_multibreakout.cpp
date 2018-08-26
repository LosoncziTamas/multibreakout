#include <SDL2/SDL.h>
#include <sys/stat.h>

#include "sdl_multibreakout.h"
#include "multibreakout.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 480;
const float TARGET_UPDATE_HZ = 30.0f;

time_t getLastWriteTime(const char *fileName)
{
    struct stat fileAttrs;
    
    if (stat(fileName, &fileAttrs) < 0)
    {
        return 0;
    }
    
    return fileAttrs.st_mtime;
}

bool loadDll(GameCode& appDll, const char *dllPath)
{
    time_t time = getLastWriteTime(dllPath);
    
    if (appDll.lastLoadTime >= time)
    {
        return false;
    }
    else
    {
        if (appDll.dll)
        {
            SDL_UnloadObject(appDll.dll);
            appDll.dll = 0;
        }
    }
    
    void *lib = SDL_LoadObject(dllPath);
    
    if (!lib)
    {
        return false;
    }
    
    appDll.dll = lib;
    appDll.update = (gameUpdateFn)SDL_LoadFunction(lib, "gameUpdate");
    appDll.lastLoadTime = time;
    
    return true;
}

static float secondsElapsed(Uint64 old, Uint64 current)
{
    return SDL_static_cast(float, current - old) / SDL_static_cast(float, SDL_GetPerformanceFrequency());
}

void startRecordingInput(PlatformState* platformState)
{
    platformState->recording = true;
    platformState->inputRecordHandler = SDL_RWFromFile("build/input", "w");
    
    size_t bytesToWrite = platformState->totalSize;
    size_t writtenObjects = SDL_RWwrite(platformState->inputRecordHandler, platformState->gameMemoryBlock, bytesToWrite, 1);
    
    SDL_assert(writtenObjects == 1);
}

void stopRecordingInput(PlatformState* platformState)
{
    SDL_assert(platformState->recording && platformState->inputRecordHandler);
    SDL_RWclose(platformState->inputRecordHandler);
    
    platformState->recording = false;
}

void recordInput(PlatformState* platformState, GameInput* gameInput)
{
    size_t writtenObjects = SDL_RWwrite(platformState->inputRecordHandler, gameInput, sizeof(*gameInput), 1);
    SDL_assert(writtenObjects == 1);
}

void startInputPlayback(PlatformState* platformState)
{
    platformState->replaying = true;
    platformState->inputPlaybackHandler = SDL_RWFromFile("build/input","r");
    
    SDL_assert(platformState->inputPlaybackHandler);
    SDL_RWread(platformState->inputPlaybackHandler, platformState->gameMemoryBlock, platformState->totalSize, 1);
}

void stopInputPlayback(PlatformState* platformState)
{
    SDL_assert(platformState->replaying && platformState->inputPlaybackHandler);
    SDL_RWclose(platformState->inputPlaybackHandler);
    platformState->replaying = false;
}

void playbackInput(PlatformState* platformState, GameInput* gameInput)
{
    size_t bytesRead = SDL_RWread(platformState->inputPlaybackHandler, gameInput, sizeof(*gameInput), 1);
    
    if (bytesRead == 0)
    {
        stopInputPlayback(platformState);
        startInputPlayback(platformState);
        SDL_RWread(platformState->inputPlaybackHandler, gameInput, sizeof(*gameInput), 1);
    }
}

static SDL_bool handleEvent(SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_QUIT:
            return SDL_FALSE;
        default:
            break;
    }
    
    return SDL_TRUE;
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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    
    float secondsPerFrame = 1.0f / TARGET_UPDATE_HZ;

    Uint64 startCounter = SDL_GetPerformanceCounter();
    Uint64 perfCountFreq = SDL_GetPerformanceFrequency();
        
    GameMemory gameMemory = {};
    gameMemory.permanentMemorySize = 1024 * 1024 * 16;
    gameMemory.transientMemorySize = 1024 * 1024 * 32;

    PlatformState platformState = {};
    platformState.totalSize = gameMemory.permanentMemorySize + gameMemory.transientMemorySize;
    platformState.gameMemoryBlock = SDL_malloc(platformState.totalSize);

    gameMemory.permanentMemory = platformState.gameMemoryBlock;
    gameMemory.transientMemory = ((Uint8 *)gameMemory.permanentMemory + gameMemory.permanentMemorySize);
    
    GameCode gameCode = {};
    time_t lastWrite = 0;
    bool loadingDll = true;
    const char *dllPath = "build/game.so";
    
    SDL_bool running = SDL_TRUE;

    GameInput input = {};

    while(running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            running = handleEvent(event);
        }
        
        GameInput oldInput = input;
        input = {};
        
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        
        input.record = state[SDL_SCANCODE_R];
        input.replay = state[SDL_SCANCODE_P];
        
        if (input.record && !oldInput.record)
        {
            if (!platformState.recording)
            {
                startRecordingInput(&platformState);
            }
            else
            {
                stopRecordingInput(&platformState);
                startInputPlayback(&platformState);
            }
        }
        
        if (input.replay && !oldInput.replay)
        {
            if (!platformState.replaying)
            {
                startInputPlayback(&platformState);
            }
            else
            {
                stopInputPlayback(&platformState);
            }
        }
        
        input.left = state[SDL_SCANCODE_LEFT];
        input.down = state[SDL_SCANCODE_DOWN];
        input.up = state[SDL_SCANCODE_UP];
        input.right = state[SDL_SCANCODE_RIGHT];
        input.space = state[SDL_SCANCODE_SPACE];
        input.pause = state[SDL_SCANCODE_P];
        
        Uint32 mouseVal = SDL_GetMouseState(&input.mouseX, &input.mouseY);
        input.mouseLeft = mouseVal & SDL_BUTTON(SDL_BUTTON_LEFT);
        input.mouseRight = mouseVal & SDL_BUTTON(SDL_BUTTON_RIGHT);
        
        while (secondsElapsed(startCounter, SDL_GetPerformanceCounter()) < secondsPerFrame);
        
        Uint64 end_counter = SDL_GetPerformanceCounter();
        input.deltaTime = SDL_static_cast(float, (end_counter - startCounter)) / SDL_static_cast(float, perfCountFreq);
        
        if (platformState.recording)
        {
            recordInput(&platformState, &input);
        }
        
        if (platformState.replaying)
        {
            playbackInput(&platformState, &input);
        }
        
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
            gameCode.update(&gameMemory, &input, renderer);
        }
        startCounter = end_counter;
    }
    
    SDL_free(platformState.gameMemoryBlock);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
