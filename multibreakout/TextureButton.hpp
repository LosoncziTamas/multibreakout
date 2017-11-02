#ifndef TextureButton_hpp
#define TextureButton_hpp

struct Renderer;
struct GameInput;

struct TextureButton {
    int x;
    int y;
    int w;
    int h;
    int textureIndex;
    void (*onclick)(GameInput& input);
};

void drawButton(TextureButton& button, Renderer& renderer);
void updateButton(TextureButton& button, GameInput& gameInput);

#endif
