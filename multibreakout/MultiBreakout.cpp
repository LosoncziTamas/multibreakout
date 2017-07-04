#include <iostream>

#include "MultiBreakout.hpp"
#include "Vec2.hpp"

SDL_Rect rect = {SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2, 120, 40};

struct Paddle {
    float x;
    float y;
};

void gameUpdate(const GameInput& input, const Renderer& renderer, float delta){
    
    std::cout << delta << std::endl;
    
    Vec2 vec(1.0f, 2.0f);
    Vec2 _vec(3.0f, 1.0f);
    
    if (input.left) {
        rect.x -= static_cast<int>(100 * delta);
    }
    if (input.right) {
        rect.x += static_cast<int>(100 * delta);
    }
    
    renderer.clear();
    renderer.drawRectangle(rect);
    renderer.update();
}
