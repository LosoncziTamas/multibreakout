#ifndef Button_hpp
#define Button_hpp

#include <SDL2/SDL_render.h>

struct GameInput;

enum ButtonState {pressed, unpressed, hover};

struct Button {
    int x;
    int y;
    int w;
    int h;
    SDL_Color unpressedColor;
    SDL_Color pressedColor;
    SDL_Color hoverColor;
    ButtonState state;
};

void drawButton(Button& button, SDL_Renderer* renderer);
void updateButton(Button& button, GameInput& gameInput);

#endif