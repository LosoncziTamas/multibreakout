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
    SDL_Texture *paddleTexture = createTexture("paddle.png", renderer);
    renderer.textures.push_back(paddleTexture);
    gameState.paddle.textureIndex = 0;
    gameState.enemyLeft.paddle.textureIndex = 0;
    
    SDL_Texture *ballTexture = createTexture("ball.png", renderer);
    renderer.textures.push_back(ballTexture);
    for (auto& ball : gameState.balls) {
        ball.textureIndex = 1;
    };
    
    SDL_Texture *brickTexture = createTexture("brick_yellow.png", renderer);
    renderer.textures.push_back(brickTexture);
    for (auto& brick : gameState.bricks) {
        brick.textureIndex = 2;
    };
    
    SDL_Texture *enemyTexture = createTexture("enemy_paddle.png", renderer);
    renderer.textures.push_back(enemyTexture);
    gameState.enemyUpper.paddle.textureIndex = 3;
    gameState.enemyRight.paddle.textureIndex = 3;
    
    SDL_Texture *obstacleTexture = createTexture("obstacle.png", renderer);
    renderer.textures.push_back(obstacleTexture);
    for (int i = 0; i < OBSTACLES_SIZE; ++i) {
        gameState.obstacles.content[i].textureIndex = 4;
    }

}


