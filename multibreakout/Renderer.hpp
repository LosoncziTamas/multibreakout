#ifndef Renderer_hpp
#define Renderer_hpp

#include <SDL2/SDL.h>
#include "Window.hpp"

class Renderer {
    
public:
    Renderer(const Window &window);
    ~Renderer();
    
    void clear() const;
    void update() const;
    void drawRectangle(const SDL_Rect& rect) const;
    
private:
    Renderer(const Renderer& other);
    Renderer& operator=(const Renderer&);
    
    SDL_Renderer *sdlRenderer;
};

#endif
