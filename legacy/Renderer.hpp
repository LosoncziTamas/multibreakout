#ifndef Renderer_hpp
#define Renderer_hpp

#include <SDL2/SDL_render.h>
#include <SDL2_ttf/SDL_ttf.h>

#include "SDL_FontCache.h"

#include "Ball.hpp"
#include "Brick.hpp"
#include "Texture.hpp"
#include "Paddle.hpp"

const SDL_Color RED = {255, 0, 0, 255};
const SDL_Color GREEN = {0, 255, 0, 255};
const SDL_Color BLUE = {0, 0, 255, 255};
const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color YELLOW = {255, 255, 0, 0};
const SDL_Color WHITE = {255, 255, 255, 255};
const SDL_Color BEIGE = {248, 232, 176, 255};
const SDL_Color SKY_BLUE = {130, 189, 240, 0};

SDL_Renderer* createRenderer(SDL_Window* window);
FC_Font* createFont(SDL_Renderer* renderer);
int flipY(int y, int height = 0);
SDL_Color getDrawColor(PowerUp powerUp);
void clear(SDL_Renderer* renderer, SDL_Color color = SKY_BLUE);
void drawBoundaries(SDL_Renderer* renderer, int left, int right);
void drawPoint(SDL_Renderer* renderer, Vec2& vec, SDL_Color color);
void drawPoint(SDL_Renderer* renderer, float x, float y, SDL_Color color);

#endif
