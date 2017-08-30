#ifndef Window_hpp
#define Window_hpp

#include <SDL2/SDL.h>

struct Window {
    Window(const Window& other) = delete;
    Window& operator=(const Window&) = delete;
    SDL_Window *sdlWindow;
    Window(int width, int height);
    ~Window();
};

#endif