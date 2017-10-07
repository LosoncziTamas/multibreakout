#include <cmath>

#include "Renderer.hpp"
#include "Paddle.hpp"
#include "MultiBreakout.hpp"

Renderer::Renderer(const Window &window) {
    sdlRenderer = SDL_CreateRenderer(window.sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_assert(sdlRenderer);
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(sdlRenderer);
}

void Renderer::clear() const {
    SDL_SetRenderDrawColor(sdlRenderer, 130, 189, 240, 0);
    SDL_RenderClear(sdlRenderer);
}

void Renderer::update() const {
    SDL_RenderPresent(sdlRenderer);
}

void Renderer::drawPaddle(const Paddle& paddle) const {
    int w = round(paddle.width);
    int h = round(paddle.height);
    int x = round(paddle.newPos.x - paddle.width * 0.5f);
    int y = round(SCREEN_HEIGHT - (paddle.newPos.y + paddle.height * 0.5f));
    
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(sdlRenderer, &rect);
}

SDL_Color getDrawColor(PowerUp powerUp) {
    switch (powerUp) {
        case speedUp:
            return BLUE;
        case slowDown:
            return RED;
        case strech:
            return GREEN;
        case shrink:
            return YELLOW;
        case neutral:
        default:
            return BLACK;
    }
}

void Renderer::drawBrick(const Brick& brick) const {
    if (!brick.active) {
        return;
    }
    SDL_Color color = getDrawColor(brick.powerUp);
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    int x = brick.center.x - brick.width * 0.5f;
    int y = SCREEN_HEIGHT - (brick.center.y + brick.height * 0.5f);
    SDL_Rect sdlRect = {x, y, brick.width, brick.height};
    SDL_RenderDrawRect(sdlRenderer, &sdlRect);
}

void Renderer::drawBoundaries(int left, int right) const {
    SDL_SetRenderDrawColor(sdlRenderer, 128, 0, 0, 255);
    SDL_Rect leftRect = {0, 0, left, SCREEN_HEIGHT};
    SDL_Rect rightRect = {right, 0, SCREEN_WIDTH - right, SCREEN_HEIGHT};
    SDL_RenderFillRect(sdlRenderer, &leftRect);
    SDL_RenderFillRect(sdlRenderer, &rightRect);
}

void Renderer::drawBall(const Ball& ball) const {
    SDL_Color color = getDrawColor(ball.powerUp);
    int x0 = round(ball.newPos.x);
    int y0 = round(SCREEN_HEIGHT - ball.newPos.y);
    int radius = round(ball.radius);
    
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    
    while (x >= y)
    {
        SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
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
    SDL_RenderDrawPoint(sdlRenderer, round(vec.x), round(SCREEN_HEIGHT - vec.y));
}

void Renderer::drawPoint(float x, float y, SDL_Color color) const {
    SDL_SetRenderDrawColor(sdlRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(sdlRenderer, round(x), round(SCREEN_HEIGHT - y));
}