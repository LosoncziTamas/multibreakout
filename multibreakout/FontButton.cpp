#include "FontButton.hpp"
#include "Renderer.hpp"

void initFontButton(FontButton& button, FC_Font* font, int x, int y, const char* text, void (*onclick)(GameInput& gameInput)) {
    button.x = x;
    button.y = y;
    button.w = FC_GetWidth(font, text);
    button.h = FC_GetHeight(font, text);
    button.text = text;
    button.onclick = onclick;
}

void drawFontButton(FontButton& button, FC_Font* font, SDL_Renderer* renderer) {
    FC_Draw(font, renderer, button.x, flipY(button.y, button.h), button.text);
}

void updateFontButton(FontButton& button, GameInput& gameInput) {
    float dx = gameInput.mouseX - button.x;
    float dy = flipY(gameInput.mouseY) - button.y;
    if (dx >= 0 && dx <= button.w && dy >= 0 && dy <= button.h) {
        if (gameInput.mouseLeft) {
            button.onclick(gameInput);
        }
    }
}