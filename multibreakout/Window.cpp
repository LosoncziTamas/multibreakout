#include "Window.hpp"

Window::Window() {
    sdlWindow = SDL_CreateWindow("Multibreakout",
                                 SDL_WINDOWPOS_CENTERED_MASK,
                                 SDL_WINDOWPOS_CENTERED_MASK,
                                 640,
                                 480,
                                 SDL_WINDOW_RESIZABLE);
    SDL_assert(sdlWindow);
}

Window::~Window() {
    SDL_DestroyWindow(sdlWindow);
}
