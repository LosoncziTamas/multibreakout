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

static SDL_Rect fillableLeft = {};
static SDL_Rect fillableTop = {};
static SDL_Rect fillableRight = {};
static SDL_Rect fillableBottom = {};
static SDL_Rect fillableCenter = {};

static BitmapBuffer buffer = {};

void readNinePatch(Renderer& renderer) {
    SDL_Surface* img = IMG_Load("panelhdpitest.9.png");
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
    
    //TODO: reorganize the loop
    
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
    
    topLeft.x = 1;
    topLeft.y = 1;
    topLeft.w = leftRectWidth - 1;
    topLeft.h = topRectHeight - 1;
    
    bottomLeft.x = 1;
    bottomLeft.y = bottomRectStart;
    bottomLeft.w = leftRectWidth - 1;
    bottomLeft.h = (buffer.h - 1) - bottomRectStart;
    
    topRight.x = rightRectStart;
    topRight.y = 1;
    topRight.w = (buffer.w - 1) - rightRectStart;
    topRight.h = topRectHeight - 1;
    
    bottomRight.x = rightRectStart;
    bottomRight.y = bottomRectStart;
    bottomRight.w = (buffer.w - 1) - rightRectStart;
    bottomRight.h = (buffer.h - 1) - bottomRectStart;
    
    fillableRight.x = rightRectStart;
    fillableRight.y = topRectHeight;
    fillableRight.w = (buffer.w - 1) - rightRectStart;
    fillableRight.h = bottomRectStart - topRectHeight;
    
    fillableTop.x = leftRectWidth;
    fillableTop.y = 1;
    fillableTop.w = rightRectStart - leftRectWidth;
    fillableTop.h = topRectHeight - 1;
    
    fillableLeft.x = 1;
    fillableLeft.y = topRectHeight;
    fillableLeft.w = leftRectWidth - 1;
    fillableLeft.h = bottomRectStart - topRectHeight;
    
    fillableBottom.x = leftRectWidth;
    fillableBottom.y = bottomRectStart;
    fillableBottom.w = rightRectStart - leftRectWidth;
    fillableBottom.h = (buffer.h - 1) - bottomRectStart;
    
    fillableCenter.x = leftRectWidth;
    fillableCenter.y = topRectHeight;
    fillableCenter.w = rightRectStart - leftRectWidth;
    fillableCenter.h = bottomRectStart - topRectHeight;
    
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

void renderRect(SDL_Rect& srcRect, SDL_Rect& dstRect, Renderer& renderer) {
    
    SDL_UpdateTexture(buffer.texture,
                      NULL,
                      buffer.pixels,
                      buffer.w * buffer.bytesPerPixel);
    
    SDL_RenderCopy(renderer.sdlRenderer,
                   buffer.texture,
                   &srcRect,
                   &dstRect);
}

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
    clear(renderer, WHITE);
#if 0
    drawButton(button, renderer);
    drawButton(textureButton, renderer);
    drawButton(fontButton, renderer);
    updateButton(button, gameState.input);
    updateButton(textureButton, gameState.input);
    updateButton(fontButton, gameState.input);
    
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
    
    int multiplier = 1;
    int targetWidth = 400;
    int targetHeight = 400;
    int minWidth = buffer.w - 2; //TODO: add fillable width
    SDL_assert(targetWidth >= minWidth);
    int minHeight = buffer.h - 2;
    
    int repeatHorizontal = (targetWidth - (topLeft.w + topRight.w)) / fillableTop.w;
    int repeatVertical = (targetHeight - (topLeft.h + bottomLeft.h)) / fillableLeft.h;
    
    //top
    SDL_Rect dstRectTopLeft = {0, 0, topLeft.w * multiplier, topLeft.h * multiplier};
    renderRect(topLeft, dstRectTopLeft, renderer);
    
    SDL_Rect dstRectFillableTop;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableTop = {dstRectTopLeft.w + i * (fillableTop.w * multiplier), 0, fillableTop.w * multiplier, fillableTop.h * multiplier};
        renderRect(fillableTop, dstRectFillableTop, renderer);
    }
    
    SDL_Rect dstRectTopRight = {dstRectFillableTop.x + dstRectFillableTop.w, 0, topRight.w * multiplier, topRight.h * multiplier};
    renderRect(topRight, dstRectTopRight, renderer);
    
    //left
    SDL_Rect dstRectFillableLeft;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableLeft = {0, dstRectTopLeft.h + i * (fillableLeft.h * multiplier), fillableLeft.w * multiplier, fillableLeft.h * multiplier};
        renderRect(fillableLeft, dstRectFillableLeft, renderer);
    }
    
    SDL_Rect dstRectBottomLeft = {0, dstRectFillableLeft.y + dstRectFillableLeft.h, bottomLeft.w * multiplier, bottomLeft.h * multiplier};
    renderRect(bottomLeft, dstRectBottomLeft, renderer);
    
    //bottom
    SDL_Rect dstRectFillableBottom;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableBottom = {dstRectBottomLeft.x + dstRectBottomLeft.w + i * (fillableTop.w * multiplier), dstRectFillableLeft.y + dstRectFillableLeft.h, fillableBottom.w * multiplier, fillableBottom.h * multiplier};
        renderRect(fillableBottom, dstRectFillableBottom, renderer);
    }
    
    SDL_Rect dstRectBottomRight = {dstRectFillableBottom.x + dstRectFillableBottom.w, dstRectFillableBottom.y, bottomRight.w * multiplier, bottomRight.h * multiplier};
    renderRect(bottomRight, dstRectBottomRight, renderer);
    
    //right
    SDL_Rect dstRectFillableRight;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableRight = {dstRectTopRight.x, dstRectTopRight.y + dstRectTopRight.h + i * (fillableLeft.h * multiplier), fillableRight.w * multiplier, fillableRight.h * multiplier};
        renderRect(fillableRight, dstRectFillableRight, renderer);
    }
    
    //TODO: use the correct width and height values
    for (int i = 0; i < repeatVertical; ++i) {
        for (int j = 0; j < repeatHorizontal; ++j) {
            SDL_Rect dstRectFillableCenter = {dstRectFillableLeft.x + dstRectFillableLeft.w + j * (fillableTop.w * multiplier), dstRectFillableTop.y + dstRectFillableTop.h + i * (fillableLeft.h * multiplier), fillableCenter.w * multiplier, fillableCenter.h * multiplier};
            renderRect(fillableCenter, dstRectFillableCenter, renderer);
        }
    }
    
    update(renderer);
}
