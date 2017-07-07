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
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    int x = round(paddle.centerPos.x - paddle.width * 0.5f);
    int y = round(paddle.centerPos.y);
    int w = round(paddle.width);
    int h = round(paddle.height);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(sdlRenderer, &rect);
}

void Renderer::drawRectangle(const SDL_Rect& rectangle) const {
    SDL_SetRenderDrawColor(sdlRenderer, 128, 0, 0, 255);
    SDL_RenderFillRect(sdlRenderer, &rectangle);
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(sdlRenderer);
}