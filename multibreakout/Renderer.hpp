#ifndef Renderer_hpp
#define Renderer_hpp

#include <SDL2/SDL_render.h>

#include "Window.hpp"
#include "Ball.hpp"
#include "Brick.hpp"

struct Paddle;

const SDL_Color RED = {255, 0, 0, 255};
const SDL_Color GREEN = {0, 255, 0, 255};
const SDL_Color BLUE = {0, 0, 255, 255};
const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color YELLOW = {255, 255, 0, 0};
const SDL_Color WHITE = {0, 0, 0, 0};

struct Renderer {
    
    SDL_Renderer *sdlRenderer;
    
    Renderer(const Window &window);
    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer();
    
    void clear() const;
    void update() const;
    void drawPaddle(const Paddle& paddle) const;
    void drawBoundaries(int left, int right) const;
    void drawBrick(const Brick& brick) const;
    void drawBall(const Ball& ball) const;
    void drawPoint(const Vec2& vec, SDL_Color color = RED) const;
    void drawPoint(float x, float y, SDL_Color color = RED) const;
};

#endif
