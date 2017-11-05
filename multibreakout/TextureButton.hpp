#ifndef TextureButton_hpp
#define TextureButton_hpp

#include <SDL2/SDL_render.h>

struct Atlas;
struct GameInput;

struct TextureButton {
    int x;
    int y;
    int w;
    int h;
    int textureIndex;
    void (*onclick)(GameInput& input);
};

void drawButton(SDL_Renderer* renderer, Atlas& atlas, TextureButton& button);
void updateButton(TextureButton& button, GameInput& gameInput);

#endif
