#ifndef Renderer_hpp
#define Renderer_hpp

#include <SDL2/SDL.h>
#include "Window.hpp"
#include "Paddle.hpp"
#include "Ball.hpp"

class Renderer {
    
public:
    Renderer(const Window &window);
    ~Renderer();
    
    void clear() const;
    void update() const;
    void drawPaddle(const Paddle& paddle) const;
    void drawRectangle(const SDL_Rect& rectangle) const;
    void drawBall(const Ball& ball) const;
    void drawPoint(const Vec2& vec, SDL_Color color = {255, 0, 0, 255}) const;
    void drawPoint(float x, float y, SDL_Color color = {255, 0, 0, 255}) const;
    
private:
    Renderer(const Renderer& other);
    Renderer& operator=(const Renderer&);
    
    SDL_Renderer *sdlRenderer;
};

#endif
