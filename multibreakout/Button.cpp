#include "Button.hpp"
#include "Renderer.hpp"

void drawButton(Button& button, SDL_Renderer* renderer) {
    SDL_Rect rect;
    rect.w = button.w;
    rect.h = button.h;
    rect.x = button.x;
    rect.y = flipY(button.y, button.h);
    
    if (button.state == unpressed) {
        SDL_SetRenderDrawColor(renderer, button.unpressedColor.r, button.unpressedColor.g, button.unpressedColor.b, button.unpressedColor.a);
    } else if (button.state == pressed) {
        SDL_SetRenderDrawColor(renderer, button.pressedColor.r, button.pressedColor.g, button.pressedColor.b, button.pressedColor.a);
    } else if (button.state == hover) {
        SDL_SetRenderDrawColor(renderer, button.hoverColor.r, button.hoverColor.g, button.hoverColor.b, button.hoverColor.a);
    }
    
    SDL_RenderFillRect(renderer, &rect);
}

void updateButton(Button& button, GameInput& gameInput) {
    button.state = unpressed;
    float dx = gameInput.mouseX - button.x;
    float dy = flipY(gameInput.mouseY) - button.y;
    if (dx >= 0 && dx <= button.w && dy >= 0 && dy <= button.h) {
        if (gameInput.mouseLeft) {
            button.state = pressed;
        } else  {
            button.state = hover;
        }
    }
}