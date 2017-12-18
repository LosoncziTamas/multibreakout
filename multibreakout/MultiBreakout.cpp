#include <iostream>

#include "MultiBreakout.hpp"
#include "NinePatch.hpp"
#include "GameState.hpp"
#include "Menu.hpp"
#include "Entity.hpp"

void generatePatches(GameState& gameState) {
    NinePatchBase base;
    for (Uint32 textureIndex = 0; textureIndex < gameState.textureCount; ++textureIndex) {
        SDL_DestroyTexture(gameState.ninePatchTextures[textureIndex]);
    }
    gameState.textureCount = 0;
    initNinePatcheBase(base, gameState.renderer);
    generateTextureFromNinePatchBase(base, gameState.gameUi.leftPanel, gameState);
    generateTextureFromNinePatchBase(base, gameState.gameUi.rightPanel, gameState);
    generateTextureFromNinePatchBase(base, gameState.menu.menuPanel, gameState);
    SDL_FreeSurface(base.surface);
}

void gamePlayUpdate(GameState *gameState)
{
    updateEntities(gameState);
}

extern "C" void gameUpdate(GameState &gameState) {
    if (!gameState.initialized) {
        srand(SDL_static_cast(unsigned int, time(NULL)));
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
            gamePlayUpdate(&gameState);
            break;
        default:
            break;
    }
}
