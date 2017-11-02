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

struct NinePatch {
    BitmapBuffer buffer;
    
    int targetWidth;
    int targetHeight;
    
    SDL_Rect topLeft;
    SDL_Rect bottomLeft;
    SDL_Rect topRight;
    SDL_Rect bottomRight;
    SDL_Rect fillableLeft;
    SDL_Rect fillableTop;
    SDL_Rect fillableRight;
    SDL_Rect fillableBottom;
    SDL_Rect fillableCenter;
};

void readNinePatch(NinePatch& ninePatch, Renderer& renderer) {
    SDL_Surface* img = IMG_Load("panelhdpitest.9.png");
    BitmapBuffer& buffer = ninePatch.buffer;
    
    buffer.texture = SDL_CreateTexture(renderer.sdlRenderer, img->format->format, SDL_TEXTUREACCESS_STREAMING, img->w,img->h);
    SDL_SetTextureBlendMode(buffer.texture, SDL_BLENDMODE_BLEND);
    buffer.w = img->w;
    buffer.h = img->h;
    buffer.pixels = malloc(img->w * img->h * img->format->BytesPerPixel);
    buffer.pitch = img->pitch;
    buffer.bytesPerPixel = img->format->BytesPerPixel;
    
    SDL_LockTexture(buffer.texture, &img->clip_rect, &buffer.pixels, &img->pitch);
    memcpy(buffer.pixels, img->pixels, img->w * img->h * img->format->BytesPerPixel);
    SDL_UnlockTexture(buffer.texture);
    Uint32 black = SDL_MapRGBA(img->format, 0, 0, 0, 255);
    SDL_FreeSurface(img);
    
    Uint32 *pixels = (Uint32*) buffer.pixels;
    
    int leftRectWidth = 0;
    int rightRectStart = 0;
    int topRectHeight = 0;
    int bottomRectStart = 0;
    
    for (int i = 0; i < buffer.w - 1; ++i) {
        if (pixels[i] == black) {
            leftRectWidth = leftRectWidth == 0 ? i : leftRectWidth;
            rightRectStart = leftRectWidth == 0 ? 0 : i + 1;
        }
    }
    
    for (int i = 0; i < buffer.h - 1; ++i) {
        if (pixels[i * buffer.w] == black) {
            topRectHeight = topRectHeight == 0 ? i : topRectHeight;
            bottomRectStart = topRectHeight == 0 ? 0 : i + 1;
        }
    }
    
    ninePatch.topLeft.x = 1;
    ninePatch.topLeft.y = 1;
    ninePatch.topLeft.w = leftRectWidth - 1;
    ninePatch.topLeft.h = topRectHeight - 1;
    
    ninePatch.bottomLeft.x = 1;
    ninePatch.bottomLeft.y = bottomRectStart;
    ninePatch.bottomLeft.w = leftRectWidth - 1;
    ninePatch.bottomLeft.h = (buffer.h - 1) - bottomRectStart;
    
    ninePatch.topRight.x = rightRectStart;
    ninePatch.topRight.y = 1;
    ninePatch.topRight.w = (buffer.w - 1) - rightRectStart;
    ninePatch.topRight.h = topRectHeight - 1;
    
    ninePatch.bottomRight.x = rightRectStart;
    ninePatch.bottomRight.y = bottomRectStart;
    ninePatch.bottomRight.w = (buffer.w - 1) - rightRectStart;
    ninePatch.bottomRight.h = (buffer.h - 1) - bottomRectStart;
    
    ninePatch.fillableRight.x = rightRectStart;
    ninePatch.fillableRight.y = topRectHeight;
    ninePatch.fillableRight.w = (buffer.w - 1) - rightRectStart;
    ninePatch.fillableRight.h = bottomRectStart - topRectHeight;
    
    ninePatch.fillableTop.x = leftRectWidth;
    ninePatch.fillableTop.y = 1;
    ninePatch.fillableTop.w = rightRectStart - leftRectWidth;
    ninePatch.fillableTop.h = topRectHeight - 1;
    
    ninePatch.fillableLeft.x = 1;
    ninePatch.fillableLeft.y = topRectHeight;
    ninePatch.fillableLeft.w = leftRectWidth - 1;
    ninePatch.fillableLeft.h = bottomRectStart - topRectHeight;
    
    ninePatch.fillableBottom.x = leftRectWidth;
    ninePatch.fillableBottom.y = bottomRectStart;
    ninePatch.fillableBottom.w = rightRectStart - leftRectWidth;
    ninePatch.fillableBottom.h = (buffer.h - 1) - bottomRectStart;
    
    ninePatch.fillableCenter.x = leftRectWidth;
    ninePatch.fillableCenter.y = topRectHeight;
    ninePatch.fillableCenter.w = rightRectStart - leftRectWidth;
    ninePatch.fillableCenter.h = bottomRectStart - topRectHeight;
    
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

static NinePatch ninePatch;

void renderRect(SDL_Rect& srcRect, SDL_Rect& dstRect, Renderer& renderer) {
    
    SDL_UpdateTexture(ninePatch.buffer.texture,
                      NULL,
                      ninePatch.buffer.pixels,
                      ninePatch.buffer.w * ninePatch.buffer.bytesPerPixel);
    
    SDL_RenderCopy(renderer.sdlRenderer,
                   ninePatch.buffer.texture,
                   &srcRect,
                   &dstRect);
}

static SDL_Texture* finalTexture;

void blitNinePatch(Renderer& renderer) {
    int targetWidth = 400;
    int targetHeight = 400;
    
    int repeatHorizontal = (targetWidth - (ninePatch.topLeft.w + ninePatch.topRight.w)) / ninePatch.fillableTop.w;
    int repeatVertical = (targetHeight - (ninePatch.topLeft.h + ninePatch.bottomLeft.h)) / ninePatch.fillableLeft.h;
    
    Uint32 rmask, gmask, bmask, amask;
    
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    
    SDL_Surface *final = SDL_CreateRGBSurface(0, targetWidth, targetHeight, 32, rmask, gmask, bmask, amask);
    SDL_assert(final);
    SDL_Surface* img = IMG_Load("panelhdpitest.9.png");
    SDL_assert(img);
    
    SDL_Rect dstRectTopLeft = {0, 0, ninePatch.topLeft.w, ninePatch.topLeft.h};
    
    SDL_BlitSurface(img, &ninePatch.topLeft, final, &dstRectTopLeft);
    
    SDL_Rect dstRectFillableTop;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableTop = {dstRectTopLeft.w + i * (ninePatch.fillableTop.w), 0, ninePatch.fillableTop.w, ninePatch.fillableTop.h};
        SDL_BlitSurface(img, &ninePatch.fillableTop, final, &dstRectFillableTop);
    }
    
    SDL_Rect dstRectTopRight = {dstRectFillableTop.x + dstRectFillableTop.w, 0, ninePatch.topRight.w, ninePatch.topRight.h};
    SDL_BlitSurface(img, &ninePatch.topRight, final, &dstRectTopRight);
    
    SDL_Rect dstRectFillableLeft;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableLeft = {0, dstRectTopLeft.h + i * (ninePatch.fillableLeft.h), ninePatch.fillableLeft.w, ninePatch.fillableLeft.h};
        SDL_BlitSurface(img, &ninePatch.fillableLeft, final, &dstRectFillableLeft);
    }
    
    SDL_Rect dstRectBottomLeft = {0, dstRectFillableLeft.y + dstRectFillableLeft.h, ninePatch.bottomLeft.w, ninePatch.bottomLeft.h};
    SDL_BlitSurface(img, &ninePatch.bottomLeft, final, &dstRectBottomLeft);
    
    SDL_Rect dstRectFillableBottom;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableBottom = {dstRectBottomLeft.x + dstRectBottomLeft.w + i * (ninePatch.fillableTop.w), dstRectFillableLeft.y + dstRectFillableLeft.h, ninePatch.fillableBottom.w, ninePatch.fillableBottom.h};
        SDL_BlitSurface(img, &ninePatch.fillableBottom, final, &dstRectFillableBottom);
    }
    
    SDL_Rect dstRectBottomRight = {dstRectFillableBottom.x + dstRectFillableBottom.w, dstRectFillableBottom.y, ninePatch.bottomRight.w, ninePatch.bottomRight.h};
    SDL_BlitSurface(img, &ninePatch.bottomRight, final, &dstRectBottomRight);

    SDL_Rect dstRectFillableRight;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableRight = {dstRectTopRight.x, dstRectTopRight.y + dstRectTopRight.h + i * (ninePatch.fillableLeft.h), ninePatch.fillableRight.w, ninePatch.fillableRight.h};
        SDL_BlitSurface(img, &ninePatch.fillableRight, final, &dstRectFillableRight);
    }
    
    for (int i = 0; i < repeatVertical; ++i) {
        for (int j = 0; j < repeatHorizontal; ++j) {
            SDL_Rect dstRectFillableCenter = {dstRectFillableLeft.x + dstRectFillableLeft.w + j * (ninePatch.fillableTop.w), dstRectFillableTop.y + dstRectFillableTop.h + i * (ninePatch.fillableLeft.h), ninePatch.fillableCenter.w, ninePatch.fillableCenter.h};
            SDL_BlitSurface(img, &ninePatch.fillableCenter, final, &dstRectFillableCenter);
        }
    }
    
    finalTexture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, final);
}


void renderNinePatch(Renderer& renderer) {
    int targetWidth = 400;
    int targetHeight = 400;
    int minWidth = ninePatch.buffer.w - 2; //TODO: add fillable width
    SDL_assert(targetWidth >= minWidth);
    int minHeight = ninePatch.buffer.h - 2;
    SDL_assert(targetHeight >= minHeight);
    
    int repeatHorizontal = (targetWidth - (ninePatch.topLeft.w + ninePatch.topRight.w)) / ninePatch.fillableTop.w;
    int repeatVertical = (targetHeight - (ninePatch.topLeft.h + ninePatch.bottomLeft.h)) / ninePatch.fillableLeft.h;
    
    //top
    SDL_Rect dstRectTopLeft = {0, 0, ninePatch.topLeft.w, ninePatch.topLeft.h};
    renderRect(ninePatch.topLeft, dstRectTopLeft, renderer);
    
    SDL_Rect dstRectFillableTop;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableTop = {dstRectTopLeft.w + i * (ninePatch.fillableTop.w), 0, ninePatch.fillableTop.w, ninePatch.fillableTop.h};
        renderRect(ninePatch.fillableTop, dstRectFillableTop, renderer);
    }
    
    SDL_Rect dstRectTopRight = {dstRectFillableTop.x + dstRectFillableTop.w, 0, ninePatch.topRight.w, ninePatch.topRight.h};
    renderRect(ninePatch.topRight, dstRectTopRight, renderer);
    
    //left
    SDL_Rect dstRectFillableLeft;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableLeft = {0, dstRectTopLeft.h + i * (ninePatch.fillableLeft.h), ninePatch.fillableLeft.w, ninePatch.fillableLeft.h};
        renderRect(ninePatch.fillableLeft, dstRectFillableLeft, renderer);
    }
    
    SDL_Rect dstRectBottomLeft = {0, dstRectFillableLeft.y + dstRectFillableLeft.h, ninePatch.bottomLeft.w, ninePatch.bottomLeft.h};
    renderRect(ninePatch.bottomLeft, dstRectBottomLeft, renderer);
    
    //bottom
    SDL_Rect dstRectFillableBottom;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableBottom = {dstRectBottomLeft.x + dstRectBottomLeft.w + i * (ninePatch.fillableTop.w), dstRectFillableLeft.y + dstRectFillableLeft.h, ninePatch.fillableBottom.w, ninePatch.fillableBottom.h};
        renderRect(ninePatch.fillableBottom, dstRectFillableBottom, renderer);
    }
    
    SDL_Rect dstRectBottomRight = {dstRectFillableBottom.x + dstRectFillableBottom.w, dstRectFillableBottom.y, ninePatch.bottomRight.w, ninePatch.bottomRight.h};
    renderRect(ninePatch.bottomRight, dstRectBottomRight, renderer);
    
    //right
    SDL_Rect dstRectFillableRight;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableRight = {dstRectTopRight.x, dstRectTopRight.y + dstRectTopRight.h + i * (ninePatch.fillableLeft.h), ninePatch.fillableRight.w, ninePatch.fillableRight.h};
        renderRect(ninePatch.fillableRight, dstRectFillableRight, renderer);
    }
    
    //TODO: use the correct width and height values
    for (int i = 0; i < repeatVertical; ++i) {
        for (int j = 0; j < repeatHorizontal; ++j) {
            SDL_Rect dstRectFillableCenter = {dstRectFillableLeft.x + dstRectFillableLeft.w + j * (ninePatch.fillableTop.w), dstRectFillableTop.y + dstRectFillableTop.h + i * (ninePatch.fillableLeft.h), ninePatch.fillableCenter.w, ninePatch.fillableCenter.h};
            renderRect(ninePatch.fillableCenter, dstRectFillableCenter, renderer);
        }
    }
    
}

void gameUpdate(GameState& gameState, Renderer& renderer) {
    if (!gameState.init) {
        srand(time(NULL));
        initGameState(gameState);
        initTextures(renderer, gameState);
        initFontButton(fontButton, renderer, 400, 100, "multiline \n string", onClick);
        readNinePatch(ninePatch, renderer);
        blitNinePatch(renderer);
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
    //    renderNinePatch(renderer);
    SDL_RenderCopy(renderer.sdlRenderer, finalTexture, NULL, NULL);
    update(renderer);
}
