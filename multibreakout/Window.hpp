#ifndef Window_hpp
#define Window_hpp

#include <SDL2/SDL.h>

class Window {
    
private:
    Window(const Window& other);
    Window& operator=(const Window&);
public:
    SDL_Window *sdlWindow;
    Window(int width, int height);
    ~Window();
};

#endif