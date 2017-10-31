#include <iostream>

#include "MultiBreakout.hpp"
#include "Renderer.hpp"
#include "Atlas.hpp"

#include "Button.hpp"
#include "TextureButton.hpp"
#include "FontButton.hpp"

struct BitmapBuffer {
    SDL_Texture* texture;
    int w;
    int h;
    int pitch;
    void *pixels;
    Uint8 bytesPerPixel;
};

static SDL_Rect topLeft = {};
static SDL_Rect bottomLeft = {};
static SDL_Rect topRight = {};
static SDL_Rect bottomRight = {};

static BitmapBuffer buffer = {};

void readNinePatch(Renderer& renderer) {
    SDL_Surface* img = IMG_Load("panelhdpi.9.png");
    buffer.texture = SDL_CreateTexture(renderer.sdlRenderer,
                                       img->format->format,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       img->w,
                                       img->h);
    
    SDL_SetTextureBlendMode(buffer.texture, SDL_BLENDMODE_BLEND);
    
    buffer.w = img->w;
    buffer.h = img->h;
    buffer.pixels = malloc(img->w * img->h * img->format->BytesPerPixel);
    buffer.pitch = img->pitch;
    buffer.bytesPerPixel = img->format->BytesPerPixel;
    
    SDL_LockTexture(buffer.texture, &img->clip_rect, &buffer.pixels, &img->pitch);
    
    memcpy(buffer.pixels, img->pixels, img->w * img->h * img->format->BytesPerPixel);
    
    SDL_UnlockTexture(buffer.texture);
    
    Uint32 *pixels = (Uint32*) buffer.pixels;
    
    int leftRectWidth = 0;
    int rightRectStart = 0;
    int topRectHeight = 0;
    int bottomRectStart = 0;
    
    for (int i = 0; i < buffer.h - 1; ++i) {
        for (int j = 0; j < buffer.w - 1; ++j) {
            Uint32 color = pixels[i * buffer.w + j];
            Uint8 r, g, b, a;
            SDL_GetRGBA(color, img->format, &r, &g, &b, &a);
            if (r == 0 && g == 0 && b == 0 && a == 255) {
                if (i == 0) {
                    leftRectWidth = leftRectWidth == 0 ? j : leftRectWidth;
                    rightRectStart = leftRectWidth == 0 ? 0 : j + 1;
                } else if (j == 0) {
                    topRectHeight = topRectHeight == 0 ? i : topRectHeight;
                    bottomRectStart = topRectHeight == 0 ? 0 : i + 1;
                }
                printf("i: %d j: %d \n", i, j);
            }
        }
    }
    
    topLeft.x = 0;
    topLeft.y = 0;
    topLeft.w = leftRectWidth;
    topLeft.h = topRectHeight;
    
    bottomLeft.x = 0;
    bottomLeft.y = bottomRectStart;
    bottomLeft.w = leftRectWidth;
    bottomLeft.h = (buffer.h - 1) - bottomRectStart;
    
    topRight.x = rightRectStart;
    topRight.y = 0;
    topRight.w = leftRectWidth - 1;
    topRight.h = topRectHeight;
    
    bottomRight.x = rightRectStart;
    bottomRight.y = bottomRectStart;
    bottomRight.w = leftRectWidth - 1;
    bottomRight.h = (buffer.h - 1) - bottomRectStart;
    
    printf("leftRectWidth: %d rightRectStart: %d topRectHeight: %d bottomRectStart: %d \n", leftRectWidth, rightRectStart, topRectHeight, bottomRectStart);

    
}

void initGameState(GameState& gameState) {
    gameState.leftBoundary = 160;
    gameState.rightBoundary = SCREEN_WIDTH - 160;
    gameState.init = true;
    initPaddle(gameState.paddle);
    initUpperEnemy(gameState.enemyUpper);
    initLeftEnemy(gameState.enemyLeft, gameState.leftBoundary);
    initRightEnemy(gameState.enemyRight, gameState.rightBoundary);
    initBricks(gameState.bricks);
    Ball ball;
    initBall(ball, gameState.balls, gameState.paddle);
    Ball ball2;
    initBall(ball2, gameState.balls, gameState.enemyUpper.paddle);
    Ball ball3;
    initBall(ball3, gameState.balls, gameState.enemyLeft.paddle);
    Ball ball4;
    initBall(ball4, gameState.balls, gameState.enemyRight.paddle);
    initObstacles(gameState.obstacles);
}

void onClick() {
    printf("onClick");
}

static Button button = {200, 200, 100, 40, BEIGE, WHITE, YELLOW, unpressed};
static TextureButton textureButton = {300, 300, 100, 100, LEFT_BUTTON, onClick};
static FontButton fontButton;

void gameUpdate(GameState& gameState, Renderer& renderer) {
    if (!gameState.init) {
        srand(time(NULL));
        initGameState(gameState);
        initTextures(renderer, gameState);
        initFontButton(fontButton, renderer, 400, 100, "multiline \n string", onClick);
        readNinePatch(renderer);
    }
    
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == true ? false : true;
    }
    
    if (gameState.paused) {
        return;
    }
    clear(renderer);
    drawButton(button, renderer);
    drawButton(textureButton, renderer);
    drawButton(fontButton, renderer);
    updateButton(button, gameState.input);
    updateButton(textureButton, gameState.input);
    updateButton(fontButton, gameState.input);
#if 0
    updateBalls(gameState);
    updatePaddle(gameState);
    updateEnemy(gameState.enemyUpper, gameState.obstacles, gameState.balls, gameState.delta);
    updateEnemy(gameState.enemyLeft, gameState.obstacles, gameState.balls, gameState.delta);
    updateEnemy(gameState.enemyRight, gameState.obstacles, gameState.balls, gameState.delta);
    
    
    collideWithBrick(gameState.balls, gameState.bricks);
    collideWithObstacle(gameState.balls, gameState.obstacles);
    resolveCollision(gameState.balls, gameState.enemyUpper.paddle, gameState.delta);
    resolveCollision(gameState.balls, gameState.paddle, gameState.delta);
    resolveCollision(gameState.balls, gameState.enemyLeft.paddle, gameState.delta);
    resolveCollision(gameState.balls, gameState.enemyRight.paddle, gameState.delta);
    collideBalls(gameState.balls);
    
    drawLeftPaddle(renderer, gameState.enemyLeft.paddle);
    drawRightPaddle(renderer, gameState.enemyRight.paddle);
    drawLowerPaddle(renderer, gameState.paddle);
    drawUpperPaddle(renderer, gameState.enemyUpper.paddle);
    drawBalls(renderer, gameState.balls, gameState.delta);
    drawBoundaries(renderer, gameState.leftBoundary, gameState.rightBoundary);
    
    drawBricks(renderer, gameState.bricks);
    drawObstacles(renderer, gameState.obstacles);
    
    
    drawBricksDebug(renderer, gameState.bricks);
    drawPaddleDebug(renderer, gameState.paddle);
    drawPaddleDebug(renderer, gameState.enemyUpper.paddle);
    drawPaddleDebug(renderer, gameState.enemyLeft.paddle);
    drawPaddleDebug(renderer, gameState.enemyRight.paddle);
    drawBallsDebug(renderer, gameState.balls);
    
    drawDebugInfo(renderer, gameState);
    
    drawPoint(renderer, gameState.enemyUpper.steeringPos);
    drawPoint(renderer, gameState.enemyRight.steeringPos);
    drawPoint(renderer, gameState.enemyLeft.steeringPos);
#endif
    
    SDL_Rect dstRect = {0, 0, bottomRight.w * 4, bottomRight.h * 4};
    
    SDL_UpdateTexture(buffer.texture,
                      NULL,
                      buffer.pixels,
                      buffer.w * buffer.bytesPerPixel);
    
    SDL_RenderCopy(renderer.sdlRenderer,
                   buffer.texture,
                   &bottomRight,
                   &dstRect);
    
    update(renderer);
}
