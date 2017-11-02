#include <SDL2/SDL_assert.h>
#include <SDL2_image/SDL_image.h>

#include "Renderer.hpp"
#include "NinePatch.hpp"

void initNinePatch(NinePatch& ninePatch, Renderer& renderer) {
    SDL_Surface* img = IMG_Load("panelhdpi.9.png");
    SDL_assert(img);
    SDL_Texture* texture = SDL_CreateTexture(renderer.sdlRenderer, img->format->format, SDL_TEXTUREACCESS_STREAMING, img->w, img->h);
    SDL_assert(texture);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    int bufferWidth = img->w;
    int bufferHeight = img->h;
    void* bitmapBuffer = malloc(bufferWidth * bufferHeight * img->format->BytesPerPixel);
    SDL_LockTexture(texture, &img->clip_rect, &bitmapBuffer, &img->pitch);
    memcpy(bitmapBuffer, img->pixels, bufferWidth * bufferHeight * img->format->BytesPerPixel);
    SDL_UnlockTexture(texture);
    
    int leftRectWidth = 0;
    int rightRectStart = 0;
    int topRectHeight = 0;
    int bottomRectStart = 0;
    
    Uint32 black = SDL_MapRGBA(img->format, 0, 0, 0, 255);
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
    
    SDL_Rect topLeft = {1, 1, leftRectWidth - 1, topRectHeight - 1};
    SDL_Rect bottomLeft = {1, bottomRectStart, leftRectWidth - 1, (bufferHeight - 1) - bottomRectStart};
    SDL_Rect topRight = {rightRectStart, 1, (bufferWidth - 1) - rightRectStart, topRectHeight - 1};
    SDL_Rect bottomRight = {rightRectStart, bottomRectStart, (bufferWidth - 1) - rightRectStart, (bufferHeight - 1) - bottomRectStart};
    SDL_Rect fillableRight = {rightRectStart, topRectHeight, (bufferWidth - 1) - rightRectStart, bottomRectStart - topRectHeight};
    SDL_Rect fillableTop = {leftRectWidth, 1, rightRectStart - leftRectWidth, topRectHeight - 1};
    SDL_Rect fillableLeft = {1, topRectHeight, leftRectWidth - 1, bottomRectStart - topRectHeight};
    SDL_Rect fillableBottom = {leftRectWidth, bottomRectStart, rightRectStart - leftRectWidth, (bufferHeight - 1) - bottomRectStart};
    SDL_Rect fillableCenter = {leftRectWidth, topRectHeight, rightRectStart - leftRectWidth, bottomRectStart - topRectHeight};
    
    SDL_assert(ninePatch.w >= 0);
    SDL_assert(ninePatch.h >= 0);
    
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
    
    SDL_Surface *ninePatchSurface = SDL_CreateRGBSurface(0, ninePatch.w, ninePatch.h, 32, rMask, gMask, bMask, aMask);
    SDL_assert(ninePatchSurface);
    
    SDL_Rect dstRectTopLeft = {0, 0, topLeft.w, topLeft.h};
    SDL_BlitSurface(img, &topLeft, ninePatchSurface, &dstRectTopLeft);
    
    int repeatHorizontal = (ninePatch.w - (topLeft.w + topRight.w)) / fillableTop.w;
    int repeatVertical = (ninePatch.h - (topLeft.h + bottomLeft.h)) / fillableLeft.h;
    
    SDL_Rect dstRectFillableTop;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableTop = {dstRectTopLeft.w + i * (fillableTop.w), 0, fillableTop.w, fillableTop.h};
        SDL_BlitSurface(img, &fillableTop, ninePatchSurface, &dstRectFillableTop);
    }
    
    SDL_Rect dstRectTopRight = {dstRectFillableTop.x + dstRectFillableTop.w, 0, topRight.w, topRight.h};
    SDL_BlitSurface(img, &topRight, ninePatchSurface, &dstRectTopRight);
    
    SDL_Rect dstRectFillableLeft;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableLeft = {0, dstRectTopLeft.h + i * (fillableLeft.h), fillableLeft.w, fillableLeft.h};
        SDL_BlitSurface(img, &fillableLeft, ninePatchSurface, &dstRectFillableLeft);
    }
    
    SDL_Rect dstRectBottomLeft = {0, dstRectFillableLeft.y + dstRectFillableLeft.h, bottomLeft.w, bottomLeft.h};
    SDL_BlitSurface(img, &bottomLeft, ninePatchSurface, &dstRectBottomLeft);
    
    SDL_Rect dstRectFillableBottom;
    for (int i = 0; i < repeatHorizontal; ++i) {
        dstRectFillableBottom = {dstRectBottomLeft.x + dstRectBottomLeft.w + i * (fillableTop.w), dstRectFillableLeft.y + dstRectFillableLeft.h, fillableBottom.w, fillableBottom.h};
        SDL_BlitSurface(img, &fillableBottom, ninePatchSurface, &dstRectFillableBottom);
    }
    
    SDL_Rect dstRectBottomRight = {dstRectFillableBottom.x + dstRectFillableBottom.w, dstRectFillableBottom.y, bottomRight.w, bottomRight.h};
    SDL_BlitSurface(img, &bottomRight, ninePatchSurface, &dstRectBottomRight);
    
    SDL_Rect dstRectFillableRight;
    for (int i = 0; i < repeatVertical; ++i) {
        dstRectFillableRight = {dstRectTopRight.x, dstRectTopRight.y + dstRectTopRight.h + i * (fillableLeft.h), fillableRight.w, fillableRight.h};
        SDL_BlitSurface(img, &fillableRight, ninePatchSurface, &dstRectFillableRight);
    }
    
    for (int i = 0; i < repeatVertical; ++i) {
        for (int j = 0; j < repeatHorizontal; ++j) {
            SDL_Rect dstRectFillableCenter = {dstRectFillableLeft.x + dstRectFillableLeft.w + j * (fillableTop.w), dstRectFillableTop.y + dstRectFillableTop.h + i * (fillableLeft.h), fillableCenter.w, fillableCenter.h};
            SDL_BlitSurface(img, &fillableCenter, ninePatchSurface, &dstRectFillableCenter);
        }
    }
    
    ninePatch.texture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, ninePatchSurface);
    SDL_assert(ninePatch.texture);
    
    SDL_FreeSurface(img);
    SDL_FreeSurface(ninePatchSurface);
}

void drawNinePatch(NinePatch& ninePatch, Renderer& renderer) {
    SDL_Rect dst = {ninePatch.x, flipY(ninePatch.y, ninePatch.h), ninePatch.w, ninePatch.h};
    SDL_RenderCopy(renderer.sdlRenderer, ninePatch.texture, NULL, &dst);
}

void deleteNinePatch(NinePatch& ninePatch) {
    SDL_DestroyTexture(ninePatch.texture);
}
