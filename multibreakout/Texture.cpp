#include "Texture.hpp"
#include "Renderer.hpp"

SDL_Texture *createTexture(const char *path, const Renderer& renderer) {
    SDL_Surface *surface = IMG_Load(path);
    SDL_assert(surface);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, surface);
    SDL_assert(texture);
    SDL_FreeSurface(surface);
    return texture;
}

void initTextures(Renderer& renderer, GameState& gameState) {
    SDL_Texture *paddleTexture = createTexture("assets/paddle.png", renderer);
    renderer.textures.push_back(paddleTexture);
    gameState.paddle.textureIndex = 0;
    SDL_Texture *ballTexture = createTexture("assets/ball.png", renderer);
    renderer.textures.push_back(ballTexture);
    for (auto& ball : gameState.balls) {
        ball.textureIndex = 1;
    };
    SDL_Texture *brickTexture = createTexture("assets/brick_yellow.png", renderer);
    renderer.textures.push_back(brickTexture);
    for (auto& brick : gameState.bricks) {
        brick.textureIndex = 2;
    };
    SDL_Texture *enemyTexture = createTexture("assets/enemy_paddle.png", renderer);
    renderer.textures.push_back(enemyTexture);
    gameState.enemy.paddle.textureIndex = 3;
}


