#include "Renderer.hpp"
#include <cmath>

Renderer::Renderer(const Window &window) {
    sdlRenderer = SDL_CreateRenderer(window.sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_assert(sdlRenderer);
}

void Renderer::clear() const {
    SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 0);
    SDL_RenderClear(sdlRenderer);
}

void Renderer::update() const {
    SDL_RenderPresent(sdlRenderer);
}

void Renderer::drawPaddle(const Paddle& paddle) const {
    int w = round(paddle.width);
    int h = round(paddle.height);
    
    int x = round(paddle.centerPos.x - paddle.width * 0.5f);
    int y = round(paddle.centerPos.y - paddle.height);
    
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderFillRect(sdlRenderer, &rect);
    //drawBall({Vec2(x, y), Vec2(), 20});

}

void Renderer::drawRectangle(const SDL_Rect& rectangle) const {
    SDL_SetRenderDrawColor(sdlRenderer, 128, 0, 0, 255);
    SDL_RenderFillRect(sdlRenderer, &rectangle);
}

void Renderer::drawBall(const Ball& ball) const {
    int x0 = round(ball.center.x);
    int y0 = round(ball.center.y);
    int radius = round(ball.radius);
    
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    
    while (x >= y)
    {
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderDrawPoint(sdlRenderer, x0 + x, y0 + y);
        SDL_RenderDrawPoint(sdlRenderer, x0 + y, y0 + x);
        SDL_RenderDrawPoint(sdlRenderer, x0 - y, y0 + x);
        SDL_RenderDrawPoint(sdlRenderer, x0 - x, y0 + y);
        SDL_RenderDrawPoint(sdlRenderer, x0 - x, y0 - y);
        SDL_RenderDrawPoint(sdlRenderer, x0 - y, y0 - x);
        SDL_RenderDrawPoint(sdlRenderer, x0 + y, y0 - x);
        SDL_RenderDrawPoint(sdlRenderer, x0 + x, y0 - y);
        
        if (err <= 0)
        {
            y++;
            err += dy;
            dy +=2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-radius << 1) + dx;
        }
    }
}

void Renderer::drawPoint(const Vec2& vec, SDL_Color color) const {
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(sdlRenderer, round(vec.x), round(vec.y));
}

void Renderer::drawPoint(float x, float y, SDL_Color color) const {
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(sdlRenderer, round(x), round(y));
}


Renderer::~Renderer() {
    SDL_DestroyRenderer(sdlRenderer);
}