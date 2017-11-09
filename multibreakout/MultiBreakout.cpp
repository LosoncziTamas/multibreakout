#include <iostream>

#include "MultiBreakout.hpp"
#include "NinePatch.hpp"
#include "GameState.hpp"
#include "Menu.hpp"

void generatePatches(GameState& gameState) {
    NinePatchBase base;
    gameState.ninePatchTextures.clear();
    initNinePatcheBase(base, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.gameUi.leftPanel, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.gameUi.rightPanel, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.menu.menuPanel, gameState.renderer, gameState.ninePatchTextures);
    SDL_FreeSurface(base.surface);
}

extern "C" void gameUpdate(GameState& gameState) {
    if (!gameState.initialized) {
        srand(static_cast<unsigned int>(time(NULL)));
        gameState.font = createFont(gameState.renderer);
        initTextures(gameState.renderer, gameState.atlas, gameState.world);
        gameState.gameUi.leftPanel = {0, 0, 160, SCREEN_HEIGHT, LEFT_PANEL};
        gameState.gameUi.rightPanel = {SCREEN_WIDTH - 160, 0, 160, SCREEN_HEIGHT, RIGHT_PANEL};
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
            if (!gameState.world.initialized) {
                initalizeGameWorld(gameState.world);
                gameState.world.initialized = true;
            }
            if (!gameState.gameUi.initialized) {
                initializeUi(gameState.gameUi);
                gameState.gameUi.initialized = true;
            }
            gamePlayUpdate(gameState);
            break;
        default:
            break;
    }
}
