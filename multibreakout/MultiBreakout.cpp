#include <iostream>
#include "MultiBreakout.hpp"

void gameUpdate(const GameInput& input, const Renderer& renderer, float delta){
    
    std::cout << delta << std::endl;
    
    renderer.clear();
    renderer.update();
}
