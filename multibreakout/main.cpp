#include <iostream>
#include <SDL2/SDL.h>

#include "Window.hpp"
#include "Renderer.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const float targetUpdateHz = 30.0f;

SDL_bool eval(bool expr)
{
    return expr ? SDL_TRUE : SDL_FALSE;
}

static SDL_bool handleEvent(const SDL_Event& event)
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
    
    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);
    Renderer renderer(window);
    SDL_bool running = SDL_TRUE;
    
    float secondsPerFrame = 1.0f / targetUpdateHz;
    Uint64 startCounter = SDL_GetPerformanceCounter();
    Uint64 perfCountFreq = SDL_GetPerformanceFrequency();
    
    GameState gameState;
    
    while(running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            running = handleEvent(event);
        }
        
        GameInput input = {};
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        input.left = eval(state[SDL_SCANCODE_LEFT]);
        input.down = eval(state[SDL_SCANCODE_DOWN]);
        input.up = eval(state[SDL_SCANCODE_UP]);
        input.right = eval(state[SDL_SCANCODE_RIGHT]);
        input.pause = eval(state[SDL_SCANCODE_SPACE]);

        Uint32 mouseVal = SDL_GetMouseState(&input.mouseX, &input.mouseY);
        
        input.mouseLeft = eval(mouseVal & SDL_BUTTON(SDL_BUTTON_LEFT));
        input.mouseRight = eval(mouseVal & SDL_BUTTON(SDL_BUTTON_RIGHT));
        
        while (secondsElapsed(startCounter, SDL_GetPerformanceCounter()) < secondsPerFrame);
        
        Uint64 end_counter = SDL_GetPerformanceCounter();
        
        gameState.delta = static_cast<float>(end_counter - startCounter) / static_cast<float>(perfCountFreq);
        gameState.input = input;
        
        gameUpdate(gameState, renderer);
        
        startCounter = end_counter;
    }
    
    SDL_Quit();
}
