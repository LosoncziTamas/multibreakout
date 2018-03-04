#include <cmath>
#include <sstream>

#include "Renderer.hpp"
#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

void clear(SDL_Renderer* renderer, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

SDL_Renderer* createRenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_assert(renderer);
    return renderer;
}

FC_Font* createFont(SDL_Renderer* renderer) {
    if (!TTF_WasInit()) {
        SDL_assert(TTF_Init() != -1);
    }
    FC_Font* font = FC_CreateFont();
    FC_LoadFont(font, renderer, "assets/courier_prime_code.ttf", 14, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL);
    SDL_assert(font);
    return font;
}

int flipY(int y, int height) {
    return SCREEN_HEIGHT - (y + height);
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

void drawBoundaries(SDL_Renderer* renderer, int left, int right) {
    SDL_SetRenderDrawColor(renderer, BEIGE.r, BEIGE.g, BEIGE.b, BEIGE.a);
    SDL_Rect leftRect = {0, 0, left, SCREEN_HEIGHT};
    SDL_Rect rightRect = {right, 0, SCREEN_WIDTH - right, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &leftRect);
    SDL_RenderFillRect(renderer, &rightRect);
}

void drawPoint(SDL_Renderer* renderer, Vec2& vec, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, round(vec.x), round(SCREEN_HEIGHT - vec.y));
}

void drawPoint(SDL_Renderer* renderer, float x, float y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, round(x), round(SCREEN_HEIGHT - y));
}