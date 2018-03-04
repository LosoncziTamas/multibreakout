#include <SDL2/SDL_assert.h>
#include <SDL2_image/SDL_image.h>

#include "Renderer.hpp"
#include "NinePatch.hpp"
#include "GameState.hpp"

void initNinePatcheBase(NinePatchBase& ninePatchBase, SDL_Renderer* renderer) {
    ninePatchBase.surface = IMG_Load("assets/panelhdpi.9.png");
    SDL_assert(ninePatchBase.surface);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             ninePatchBase.surface->format->format,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             ninePatchBase.surface->w,
                                             ninePatchBase.surface->h);
    SDL_assert(texture);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    int bufferWidth = ninePatchBase.surface->w;
    int bufferHeight = ninePatchBase.surface->h;
    size_t bufferSize = bufferWidth * bufferHeight * ninePatchBase.surface->format->BytesPerPixel;
    void* bitmapBuffer = malloc(bufferSize);
    
    SDL_LockTexture(texture, &ninePatchBase.surface->clip_rect, &bitmapBuffer, &ninePatchBase.surface->pitch);
    memcpy(bitmapBuffer, ninePatchBase.surface->pixels, bufferSize);
    SDL_UnlockTexture(texture);
    SDL_DestroyTexture(texture);
    
    int leftRectWidth = 0;
    int rightRectStart = 0;
    int topRectHeight = 0;
    int bottomRectStart = 0;
    
    Uint32 black = SDL_MapRGBA(ninePatchBase.surface->format, 0, 0, 0, 255);
    Uint32 *pixels = (Uint32*) bitmapBuffer;
    for (int i = 0; i < bufferHeight - 1; ++i) {
        if (pixels[i] == black) {
            leftRectWidth = leftRectWidth == 0 ? i : leftRectWidth;
            rightRectStart = leftRectWidth == 0 ? 0 : i + 1;
        }
    }
    
    for (int i = 0; i < bufferHeight - 1; ++i) {
        if (pixels[i * bufferWidth] == black) {
            topRectHeight = topRectHeight == 0 ? i : topRectHeight;
            bottomRectStart = topRectHeight == 0 ? 0 : i + 1;
        }
    }
    
    
    ninePatchBase.topLeft = {1, 1, leftRectWidth - 1, topRectHeight - 1};
    ninePatchBase.bottomLeft = {1, bottomRectStart, leftRectWidth - 1, (bufferHeight - 1) - bottomRectStart};
    ninePatchBase.topRight = {rightRectStart, 1, (bufferWidth - 1) - rightRectStart, topRectHeight - 1};
    ninePatchBase.bottomRight = {rightRectStart, bottomRectStart, (bufferWidth - 1) - rightRectStart, (bufferHeight - 1) - bottomRectStart};
    ninePatchBase.fillableRight = {rightRectStart, topRectHeight, (bufferWidth - 1) - rightRectStart, bottomRectStart - topRectHeight};
    ninePatchBase.fillableTop = {leftRectWidth, 1, rightRectStart - leftRectWidth, topRectHeight - 1};
    ninePatchBase.fillableLeft = {1, topRectHeight, leftRectWidth - 1, bottomRectStart - topRectHeight};
    ninePatchBase.fillableBottom = {leftRectWidth, bottomRectStart, rightRectStart - leftRectWidth, (bufferHeight - 1) - bottomRectStart};
    ninePatchBase.fillableCenter = {leftRectWidth, topRectHeight, rightRectStart - leftRectWidth, bottomRectStart - topRectHeight};
}

void generateTextureFromNinePatchBase(NinePatchBase& ninePatchBase, NinePatch &ninePatch, GameState &gameState) {
    Uint32 rMask, gMask, bMask, aMask;
    
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rMask = 0xff000000;
    gMask = 0x00ff0000;
    bMask = 0x0000ff00;
    aMask = 0x000000ff;
#else
    rMask = 0x000000ff;
    gMask = 0x0000ff00;
    bMask = 0x00ff0000;
    aMask = 0xff000000;
#endif
    
    SDL_Surface *tmpSurface = SDL_CreateRGBSurface(0, ninePatch.w, ninePatch.h, 32, rMask, gMask, bMask, aMask);
    SDL_assert(tmpSurface);
    
    SDL_Rect& topLeft = ninePatchBase.topLeft;
    SDL_Rect& bottomLeft = ninePatchBase.bottomLeft;
    SDL_Rect& topRight = ninePatchBase.topRight;
    SDL_Rect& bottomRight = ninePatchBase.bottomRight;
    SDL_Rect& fillableRight = ninePatchBase.fillableRight;
    SDL_Rect& fillableTop = ninePatchBase.fillableTop;
    SDL_Rect& fillableLeft = ninePatchBase.fillableLeft;
    SDL_Rect& fillableBottom = ninePatchBase.fillableBottom;
    SDL_Rect& fillableCenter = ninePatchBase.fillableCenter;
    
    SDL_Rect dstRectTopLeft = {0, 0, topLeft.w, topLeft.h};
    SDL_BlitSurface(ninePatchBase.surface, &topLeft, tmpSurface, &dstRectTopLeft);
    
    int repeatHorizontal = (ninePatch.w - (topLeft.w + topRight.w)) / fillableTop.w;
    int repeatVertical = (ninePatch.h - (topLeft.h + bottomLeft.h)) / fillableLeft.h;
    
    SDL_Rect dstRectFillableTop;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableTop = {dstRectTopLeft.w + i * (fillableTop.w), 0, fillableTop.w, fillableTop.h};
        SDL_BlitSurface(ninePatchBase.surface, &fillableTop, tmpSurface, &dstRectFillableTop);
    }
    
    SDL_Rect dstRectTopRight = {dstRectFillableTop.x + dstRectFillableTop.w, 0, topRight.w, topRight.h};
    SDL_BlitSurface(ninePatchBase.surface, &topRight, tmpSurface, &dstRectTopRight);
    
    SDL_Rect dstRectFillableLeft;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableLeft = {0, dstRectTopLeft.h + i * (fillableLeft.h), fillableLeft.w, fillableLeft.h};
        SDL_BlitSurface(ninePatchBase.surface, &fillableLeft, tmpSurface, &dstRectFillableLeft);
    }
    
    SDL_Rect dstRectBottomLeft = {0, dstRectFillableLeft.y + dstRectFillableLeft.h, bottomLeft.w, bottomLeft.h};
    SDL_BlitSurface(ninePatchBase.surface, &bottomLeft, tmpSurface, &dstRectBottomLeft);
    
    SDL_Rect dstRectFillableBottom;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableBottom = {dstRectBottomLeft.x + dstRectBottomLeft.w + i * (fillableTop.w), dstRectFillableLeft.y + dstRectFillableLeft.h, fillableBottom.w, fillableBottom.h};
        SDL_BlitSurface(ninePatchBase.surface, &fillableBottom, tmpSurface, &dstRectFillableBottom);
    }
    
    SDL_Rect dstRectBottomRight = {dstRectFillableBottom.x + dstRectFillableBottom.w, dstRectFillableBottom.y, bottomRight.w, bottomRight.h};
    SDL_BlitSurface(ninePatchBase.surface, &bottomRight, tmpSurface, &dstRectBottomRight);
    
    SDL_Rect dstRectFillableRight;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableRight = {dstRectTopRight.x, dstRectTopRight.y + dstRectTopRight.h + i * (fillableLeft.h), fillableRight.w, fillableRight.h};
        SDL_BlitSurface(ninePatchBase.surface, &fillableRight, tmpSurface, &dstRectFillableRight);
    }
    
    for (int i = 0; i < repeatVertical; ++i) {
        for (int j = 0; j < repeatHorizontal; ++j) {
            SDL_Rect dstRectFillableCenter = {dstRectFillableLeft.x + dstRectFillableLeft.w + j * (fillableTop.w), dstRectFillableTop.y + dstRectFillableTop.h + i * (fillableLeft.h), fillableCenter.w, fillableCenter.h};
            SDL_BlitSurface(ninePatchBase.surface, &fillableCenter, tmpSurface, &dstRectFillableCenter);
        }
    }
    SDL_assert(gameState.textureCount < SDL_arraysize(gameState.ninePatchTextures));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gameState.renderer, tmpSurface);
    SDL_assert(texture);
    gameState.ninePatchTextures[gameState.textureCount++] = texture;
    SDL_FreeSurface(tmpSurface);
}

void drawNinePatch(SDL_Texture** ninePatchTextures, NinePatch& ninePatch, SDL_Renderer* renderer) {
    SDL_Rect dst = {ninePatch.x, flipY(ninePatch.y, ninePatch.h), ninePatch.w, ninePatch.h};
    SDL_RenderCopy(renderer, ninePatchTextures[ninePatch.textureId], NULL, &dst);
}

void deleteNinePatches(SDL_Texture** ninePatchTextures, Uint32 textureCount) {
    for (Uint32 textureIndex = 0; textureIndex < textureCount; ++textureCount) {
        SDL_DestroyTexture(ninePatchTextures[textureIndex]);
    }
}
