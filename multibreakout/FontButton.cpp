#include "FontButton.hpp"
#include "Renderer.hpp"
#include "GameState.hpp"

void initFontButton(FontButton& button, FC_Font* font, int x, int y, const char* text, void (*onclick)(GameState& gameState)) {
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

void updateFontButton(FontButton& button, GameState& gameState) {
    float dx = gameState.input.mouseX - button.x;
    float dy = flipY(gameState.input.mouseY) - button.y;
    if (dx >= 0 && dx <= button.w && dy >= 0 && dy <= button.h) {
        if (gameState.input.mouseLeft) {
            button.onclick(gameState);
        }
    }
}