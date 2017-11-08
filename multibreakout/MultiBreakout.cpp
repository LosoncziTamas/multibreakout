#include <iostream>

#include "MultiBreakout.hpp"
#include "NinePatch.hpp"
#include "GameState.hpp"
#include "Menu.hpp"

void generatePatches(GameState& gameState) {
    NinePatchBase base;
    gameState.ninePatchTextures.clear();
    initNinePatcheBase(base, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.gameScreen.leftPanel, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.gameScreen.rightPanel, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.menu.menuPanel, gameState.renderer, gameState.ninePatchTextures);
    SDL_FreeSurface(base.surface);
}

extern "C" void gameUpdate(GameState& gameState) {
    if (!gameState.initialized) {
        srand(time(NULL));
        gameState.font = createFont(gameState.renderer);
        initTextures(gameState.renderer, gameState.atlas, gameState.world);
        gameState.gameScreen.leftPanel = {0, 0, 160, SCREEN_HEIGHT, LEFT_PANEL};
        gameState.gameScreen.rightPanel = {SCREEN_WIDTH - 160, 0, 160, SCREEN_HEIGHT, RIGHT_PANEL};
        gameState.initialized = true;
        gameState.currScreen = menu;
    }
    
    switch (gameState.currScreen) {
        case menu:
            if (!gameState.menu.initialized) {
                initializeMenu(gameState.menu, gameState.font, gameState.atlas);
                generatePatches(gameState);
                gameState.initialized = true;
            }
            updateMenu(gameState.menu, gameState);
            drawMenu(gameState.menu, gameState.renderer, gameState.font, gameState.atlas, gameState.ninePatchTextures);
            break;
        case game:
            gamePlayUpdate(gameState);
            break;
        default:
            break;
    }
}
