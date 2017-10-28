#ifndef FontButton_hpp
#define FontButton_hpp

#include <string>

struct Renderer;
struct GameInput;

struct FontButton {
    const char* text;
    int x;
    int y;
    int w;
    int h;
    void (*onclick)(void);
};

void initFontButton(FontButton& button, Renderer& renderer, int x, int y, const char* text, void (*onclick)(void));
void drawButton(FontButton& button, Renderer& renderer);
void updateButton(FontButton& button, GameInput& gameInput);

#endif
