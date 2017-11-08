#include "Menu.hpp"
#include "GameState.hpp"
#include "FontButton.hpp"

void onStartClick(GameState& gameState) {
    gameState.currScreen = game;
}

void onSignInClick(GameState& gameState) {
    printf("Sign in\n");
}

void onLeaderBoardsClick(GameState& gameState) {
    printf("Leaderboards\n");
}

void initializeMenu(Menu& menu, FC_Font* font, Atlas& atlas) {
    int w = 500;
    int x = SCREEN_WIDTH * 0.5f - w * 0.5f;
    int h = 250;
    int y = SCREEN_HEIGHT * 0.5f - h * 0.5f;
    menu.menuPanel = {x, y, w, h, MENU_PANEL};
    
    const char* startGameText = "Start game";
    menu.startGame.w = FC_GetWidth(font, startGameText);
    menu.startGame.h = FC_GetHeight(font, startGameText);
    int topPadding = 75;
    menu.startGame.x = x + w * 0.5f - menu.startGame.w * 0.5f;
    menu.startGame.y = y + h - menu.startGame.h - topPadding;
    menu.startGame.text = startGameText;
    menu.startGame.onclick = onStartClick;
    
    const char* signInText = "Sign in";
    menu.signIn.w = FC_GetWidth(font, signInText);
    menu.signIn.h = FC_GetHeight(font, signInText);
    int padding = 50;
    menu.signIn.x = x + w * 0.5f - menu.signIn.w * 0.5f;
    menu.signIn.y = menu.startGame.y - padding;
    menu.signIn.text = signInText;
    menu.signIn.onclick = onSignInClick;
    
    const char* leaderBoardsText = "Leaderboards";
    menu.leaderBoards.text = leaderBoardsText;
    menu.leaderBoards.w = FC_GetWidth(font, leaderBoardsText);
    menu.leaderBoards.h = FC_GetHeight(font, leaderBoardsText);
    
    menu.leaderBoards.x = x + w * 0.5f - menu.leaderBoards.w * 0.5f;
    menu.leaderBoards.y = menu.signIn.y - padding;
    menu.leaderBoards.onclick = onLeaderBoardsClick;
    
    SDL_Rect &srcRect = atlas.frames[GAME_TITLE];
    
    menu.gameTitleRect.w = srcRect.w;
    menu.gameTitleRect.h = srcRect.h;
    menu.gameTitleRect.x = x + w * 0.5f - srcRect.w * 0.5f;
    menu.gameTitleRect.y = y + h - (menu.gameTitleRect.h * 0.5f);
    menu.gameTitleRect.y = flipY(menu.gameTitleRect.y, menu.gameTitleRect.h);
}

void updateMenu(Menu& menu, GameState& gameState) {
    updateFontButton(menu.startGame, gameState);
    updateFontButton(menu.signIn, gameState);
    updateFontButton(menu.leaderBoards, gameState);
}

void drawMenu(Menu& menu, SDL_Renderer* renderer, FC_Font* font, Atlas& atlas, std::vector<SDL_Texture*>& ninePatchTextures) {
    clear(renderer, SKY_BLUE);
    drawNinePatch(ninePatchTextures, menu.menuPanel, renderer);
    drawFontButton(menu.startGame, font, renderer);
    drawFontButton(menu.signIn, font, renderer);
    drawFontButton(menu.leaderBoards, font, renderer);
    
    SDL_Rect& frame = atlas.frames[GAME_TITLE];
    SDL_RenderCopy(renderer, atlas.texture, &frame, &menu.gameTitleRect);
    SDL_RenderPresent(renderer);
}