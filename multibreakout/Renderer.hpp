#ifndef Renderer_hpp
#define Renderer_hpp

#include <SDL2/SDL_render.h>

#include "Window.hpp"
#include "Ball.hpp"
#include "Brick.hpp"
#include "Texture.hpp"

struct Paddle;

const SDL_Color RED = {255, 0, 0, 255};
const SDL_Color GREEN = {0, 255, 0, 255};
const SDL_Color BLUE = {0, 0, 255, 255};
const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color YELLOW = {255, 255, 0, 0};
const SDL_Color WHITE = {0, 0, 0, 0};

struct Renderer {
    
    SDL_Renderer *sdlRenderer;
    std::vector<Texture*> textures;
    
    Renderer(const Window &window);
    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer();
    
};

void clear(const Renderer& renderer);
void update(const Renderer& renderer);
void drawPaddleDebug(const Renderer& renderer, const Paddle& paddle);
void drawBoundaries(const Renderer& renderer, int left, int right);
void drawBricks(const Renderer& renderer, const std::vector<Brick>& bricks);
void drawBricksDebug(const Renderer& renderer, const std::vector<Brick>& bricks);
void drawBallsDebug(const Renderer& renderer, const std::vector<Ball>& balls);
void drawBalls(const Renderer& renderer, const std::vector<Ball>& balls, float delta);
void drawPoint(const Renderer& renderer, const Vec2& vec, SDL_Color color = RED);
void drawPoint(const Renderer& renderer, float x, float y, SDL_Color color = RED);
void drawLowerPaddle(const Renderer& renderer, const Paddle& paddle);
void drawUpperPaddle(const Renderer& renderer, const Paddle& paddle);

#endif
