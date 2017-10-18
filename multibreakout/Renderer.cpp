#include <cmath>
#include <sstream>

#include "Renderer.hpp"
#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

void createRenderer(Renderer& renderer, SDL_Window *window) {
    renderer.sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_assert(renderer.sdlRenderer);
    SDL_assert(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG);
    if (!TTF_WasInit()) {
        SDL_assert(TTF_Init() != -1);
    }
    renderer.font = FC_CreateFont();
    FC_LoadFont(renderer.font, renderer.sdlRenderer, "assets/courier_prime_code.ttf", 16, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL);
}

void deleteRenderer(Renderer& renderer) {
    SDL_DestroyRenderer(renderer.sdlRenderer);
    FC_FreeFont(renderer.font);
    for (auto texture : renderer.textures) {
        SDL_DestroyTexture(texture);
    }
}

void clear(const Renderer& renderer) {
    SDL_SetRenderDrawColor(renderer.sdlRenderer, 130, 189, 240, 0);
    SDL_RenderClear(renderer.sdlRenderer);
}

void update(const Renderer& renderer) {
    SDL_RenderPresent(renderer.sdlRenderer);
}

void drawPaddleDebug(const Renderer& renderer, const Paddle& paddle) {
    int w = round(paddle.width);
    int h = round(paddle.height);
    int x = round(paddle.newPos.x - paddle.width * 0.5f);
    int y = round(SCREEN_HEIGHT - (paddle.newPos.y + paddle.height * 0.5f));
    
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer.sdlRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer.sdlRenderer, &rect);
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

void drawBricksDebug(const Renderer& renderer, const std::vector<Brick>& bricks) {
    for (auto& brick : bricks) {
        if (!brick.active) {
            continue;
        }
        SDL_Color color = getDrawColor(brick.powerUp);
        SDL_SetRenderDrawColor(renderer.sdlRenderer, color.r, color.g, color.b, color.a);
        int x = brick.center.x - brick.width * 0.5f;
        int y = SCREEN_HEIGHT - (brick.center.y + brick.height * 0.5f);
        SDL_Rect sdlRect = {x, y, brick.width, brick.height};
        SDL_RenderDrawRect(renderer.sdlRenderer, &sdlRect);
    }
}

void drawBricks(const Renderer& renderer, const std::vector<Brick>& bricks) {
    for (auto& brick : bricks) {
        if (!brick.active) {
            continue;
        }
        SDL_Color color = getDrawColor(brick.powerUp);
        SDL_SetRenderDrawColor(renderer.sdlRenderer, color.r, color.g, color.b, color.a);
        int x = brick.center.x - brick.width * 0.5f;
        int y = SCREEN_HEIGHT - (brick.center.y + brick.height * 0.5f);
        SDL_Rect sdlRect = {x, y, brick.width, brick.height};
        SDL_Texture* texture = renderer.textures.at(brick.textureIndex);
        SDL_RenderCopy(renderer.sdlRenderer, texture, NULL, &sdlRect);
    }
}

void drawBoundaries(const Renderer& renderer, int left, int right) {
    SDL_SetRenderDrawColor(renderer.sdlRenderer, BEIGE.r, BEIGE.g, BEIGE.b, BEIGE.a);
    SDL_Rect leftRect = {0, 0, left, SCREEN_HEIGHT};
    SDL_Rect rightRect = {right, 0, SCREEN_WIDTH - right, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer.sdlRenderer, &leftRect);
    SDL_RenderFillRect(renderer.sdlRenderer, &rightRect);
}

void drawBalls(const Renderer& renderer, const std::vector<Ball>& balls, float delta) {
    for (auto& ball : balls) {
        int x = ball.newPos.x - ball.radius;
        int y = SCREEN_HEIGHT - (ball.newPos.y + ball.radius);
        SDL_Rect rect = {x, y, static_cast<int>(ball.radius * 2), static_cast<int>(ball.radius * 2)};
        SDL_Texture* texture = renderer.textures.at(ball.textureIndex);
        SDL_RenderCopy(renderer.sdlRenderer, texture, NULL, &rect);
    }
}

void drawBallsDebug(const Renderer& renderer, const std::vector<Ball>& balls) {
    for (auto& ball : balls) {
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
            SDL_SetRenderDrawColor(renderer.sdlRenderer, color.r, color.g, color.b, color.a);
            SDL_RenderDrawPoint(renderer.sdlRenderer, x0 + x, y0 + y);
            SDL_RenderDrawPoint(renderer.sdlRenderer, x0 + y, y0 + x);
            SDL_RenderDrawPoint(renderer.sdlRenderer, x0 - y, y0 + x);
            SDL_RenderDrawPoint(renderer.sdlRenderer, x0 - x, y0 + y);
            SDL_RenderDrawPoint(renderer.sdlRenderer, x0 - x, y0 - y);
            SDL_RenderDrawPoint(renderer.sdlRenderer, x0 - y, y0 - x);
            SDL_RenderDrawPoint(renderer.sdlRenderer, x0 + y, y0 - x);
            SDL_RenderDrawPoint(renderer.sdlRenderer, x0 + x, y0 - y);
            
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
}

void drawPoint(const Renderer& renderer, const Vec2& vec, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer.sdlRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer.sdlRenderer, round(vec.x), round(SCREEN_HEIGHT - vec.y));
}

void drawPoint(const Renderer& renderer, float x, float y, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer.sdlRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer.sdlRenderer, round(x), round(SCREEN_HEIGHT - y));
}

void drawUpperPaddle(const Renderer& renderer, const Paddle& paddle) {
    int x = round(paddle.newPos.x - paddle.width * 0.5f);
    int y = round(SCREEN_HEIGHT - (paddle.newPos.y + paddle.height * 0.5f));
    SDL_Rect rect = {x, y, static_cast<int>(paddle.width), static_cast<int>(paddle.height)};
    SDL_Texture* texture = renderer.textures.at(paddle.textureIndex);
    SDL_RenderCopyEx(renderer.sdlRenderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_HORIZONTAL);
}

void drawLeftPaddle(const Renderer& renderer, const Paddle& paddle) {
    int x = round(paddle.newPos.x - paddle.height * 0.5f);
    int y = round(SCREEN_HEIGHT - (paddle.newPos.y + paddle.width * 0.5f));
    
    SDL_Rect rect = {x, y, static_cast<int>(paddle.height), static_cast<int>(paddle.width)};
    SDL_Texture* texture = renderer.textures.at(paddle.textureIndex);
    SDL_RenderCopyEx(renderer.sdlRenderer, texture, NULL, &rect, 90, NULL, SDL_FLIP_NONE);
    //TODO: correct debug draw
    SDL_SetRenderDrawColor(renderer.sdlRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer.sdlRenderer, &rect);
}

void drawRightPaddle(const Renderer& renderer, const Paddle& paddle) {
    int x = round(paddle.newPos.x - paddle.height * 0.5f);
    int y = round(SCREEN_HEIGHT - (paddle.newPos.y + paddle.width * 0.5f));
    
    SDL_Rect rect = {x, y, static_cast<int>(paddle.height), static_cast<int>(paddle.width)};
    SDL_Texture* texture = renderer.textures.at(paddle.textureIndex);
    SDL_RenderCopyEx(renderer.sdlRenderer, texture, NULL, &rect, 270, NULL, SDL_FLIP_VERTICAL);
    
    //TODO: correct debug draw
    SDL_SetRenderDrawColor(renderer.sdlRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer.sdlRenderer, &rect);
}

void drawLowerPaddle(const Renderer& renderer, const Paddle& paddle) {
    int x = round(paddle.newPos.x - paddle.width * 0.5f);
    int y = round(SCREEN_HEIGHT - (paddle.newPos.y + paddle.height * 0.5f));
    SDL_Rect rect = {x, y, static_cast<int>(paddle.width), static_cast<int>(paddle.height)};
    SDL_Texture* texture = renderer.textures.at(paddle.textureIndex);
    SDL_RenderCopy(renderer.sdlRenderer, texture, NULL, &rect);
}

const char *aiStates[] =
{
    "none",
    "steering",
    "defending"
};

void drawDebugInfo(const Renderer& renderer, const GameState& gameState) {
    FC_Draw(renderer.font, renderer.sdlRenderer, 0, 0, "delta: %f\nai: %s", gameState.delta, aiStates[gameState.enemyUpper.state]);
}
