#include "TextureButton.hpp"
#include "Renderer.hpp"

void drawButton(SDL_Renderer* renderer, Atlas& atlas, TextureButton& button) {
    SDL_Rect dstRec;
    dstRec.w = button.w;
    dstRec.h = button.h;
    dstRec.x = button.x;
    dstRec.y = flipY(button.y, button.h);
    
    SDL_Rect srcRect = atlas.frames[button.textureIndex];
    SDL_RenderCopy(renderer, atlas.texture, &srcRect, &dstRec);
}

void updateButton(TextureButton& button, GameInput& gameInput) {
    float dx = gameInput.mouseX - button.x;
    float dy = flipY(gameInput.mouseY) - button.y;
    if (dx >= 0 && dx <= button.w && dy >= 0 && dy <= button.h) {
        if (gameInput.mouseLeft) {
            button.onclick(gameInput);
        }
    }
}