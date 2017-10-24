#include "Texture.hpp"
#include "Renderer.hpp"

void readFile(const char* path, std::string& content) {
    char *fileContent = NULL;
    SDL_RWops *file = SDL_RWFromFile(path, "r");
    if (file) {
        Sint64 size = SDL_RWsize(file);
        fileContent = (char*) malloc(size + 1);
        if(SDL_RWread(file, fileContent, sizeof(char), size)) {
            fileContent[size] = '\0';
            content.assign(fileContent);
        } else {
            printf("Error: Unable to read file! SDL Error: %s\n", SDL_GetError());
            free(fileContent);
        }
        free(fileContent);
        SDL_RWclose(file);
    }
    else {
        printf("Error: Unable to open file! SDL Error: %s\n", SDL_GetError());
    }
}

SDL_Texture *createTexture(const char *path, const Renderer& renderer) {
    SDL_Surface *surface = IMG_Load(path);
    SDL_assert(surface);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, surface);
    SDL_assert(texture);
    SDL_FreeSurface(surface);
    return texture;
}

int getBallTexture(PowerUp powerUp) {
    switch (powerUp) {
        case speedUp:
            return BLUE_BALL;
        case slowDown:
            return BROWN_BALL;
        case strech:
            return GREEN_BALL;
        case shrink:
            return PURPLE_BALL;
        case neutral:
        default:
            return GREY_BALL;
    }
}

int getBrickTexture(PowerUp powerUp) {
    switch (powerUp) {
        case speedUp:
            return BLUE_BRICK;
        case slowDown:
            return BROWN_BRICK;
        case strech:
            return GREEN_BRICK;
        case shrink:
            return PURPLE_BRICK;
        case neutral:
        default:
            return YELLOW_BRICK;
    }
}

void initTextures(Renderer& renderer, GameState& gameState) {
    std::string json;
    readFile("texture_atlas.json", json);
    initAtlas(json, renderer);
    
    gameState.paddle.textureIndex = PLAYER_PADDLE;
    gameState.enemyLeft.paddle.textureIndex = PLAYER_PADDLE;
    for (auto& ball : gameState.balls) {
        ball.textureIndex = GREY_BALL;
    };
    for (auto& brick : gameState.bricks) {
        brick.textureIndex = getBrickTexture(brick.powerUp);
    };
    gameState.enemyUpper.paddle.textureIndex = ENEMY_PADDLE;
    gameState.enemyRight.paddle.textureIndex = ENEMY_PADDLE;
    for (int i = 0; i < OBSTACLES_SIZE; ++i) {
        gameState.obstacles.content[i].textureIndex = GREY_BRICK;
    }
}


