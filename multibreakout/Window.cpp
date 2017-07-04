#include "Window.hpp"

Window::Window(int width, int height) {
    sdlWindow = SDL_CreateWindow("Multibreakout",
                                 SDL_WINDOWPOS_CENTERED_MASK,
                                 SDL_WINDOWPOS_CENTERED_MASK,
                                 width,
                                 height,
                                 SDL_WINDOW_RESIZABLE);
    SDL_assert(sdlWindow);
}

Window::~Window() {
    SDL_DestroyWindow(sdlWindow);
}
