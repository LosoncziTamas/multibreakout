#include "Menu.hpp"
#include "GameState.hpp"
#include "FontButton.hpp"

static SDL_Rect titleRect;
static SDL_Rect srcRect;

static FontButton startGame;
static FontButton signIn;
static FontButton leaderBoards;

static bool redraw = true;

void generatePatches(GameState& gameState) {
    NinePatchBase base;
    gameState.ninePatchTextures.clear();
    initNinePatcheBase(base, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.leftPanel, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.rightPanel, gameState.renderer, gameState.ninePatchTextures);
    generateTextureFromNinePatchBase(base, gameState.menuPanel, gameState.renderer, gameState.ninePatchTextures);
    SDL_FreeSurface(base.surface);
}


void onStartClick(GameInput& input) {
    printf("Start\n");
}

void onSignInClick(GameInput& input) {
    printf("Sign in\n");
}

void onLeaderBoardsClick(GameInput& input) {
    printf("Leaderboards\n");
}

void menuUpdate(GameState& gameState) {
    clear(gameState.renderer, SKY_BLUE);
    if (redraw) {
        int w = 400;
        int x = SCREEN_WIDTH * 0.5f - w * 0.5f;
        int h = 250;
        int y = SCREEN_HEIGHT * 0.5f - h * 0.5f;
        gameState.menuPanel = {x, y, w, h, MENU_PANEL};
        
        const char* startGameText = "Start game";
        startGame.w = FC_GetWidth(gameState.font, startGameText);
        startGame.h = FC_GetHeight(gameState.font, startGameText);
        int topPadding = 75;
        startGame.x = x + w * 0.5f - startGame.w * 0.5f;
        startGame.y = y + h - startGame.h - topPadding;
        startGame.text = startGameText;
        startGame.onclick = onStartClick;
        
        const char* signInText = "Sign in";
        signIn.w = FC_GetWidth(gameState.font, signInText);
        signIn.h = FC_GetHeight(gameState.font, signInText);
        int padding = 50;
        signIn.x = x + w * 0.5f - signIn.w * 0.5f;
        signIn.y = startGame.y - padding;
        signIn.text = signInText;
        signIn.onclick = onSignInClick;
        
        const char* leaderBoardsText = "Leaderboards";
        leaderBoards.text = leaderBoardsText;
        leaderBoards.w = FC_GetWidth(gameState.font, leaderBoardsText);
        leaderBoards.h = FC_GetHeight(gameState.font, leaderBoardsText);
        
        leaderBoards.x = x + w * 0.5f - leaderBoards.w * 0.5f;
        leaderBoards.y = signIn.y - padding;
        leaderBoards.onclick = onLeaderBoardsClick;
        
        srcRect = gameState.atlas.frames[GAME_TITLE];
        
        titleRect.w = srcRect.w;
        titleRect.h = srcRect.h;
        titleRect.x = x + w * 0.5f - srcRect.w * 0.5f;
        titleRect.y = y + h - (titleRect.h * 0.5f);
        titleRect.y = flipY(titleRect.y, titleRect.h);
        
        generatePatches(gameState);
        redraw = false;
    }
    updateFontButton(startGame, gameState.input);
    updateFontButton(signIn, gameState.input);
    updateFontButton(leaderBoards, gameState.input);

    drawNinePatch(gameState.ninePatchTextures, gameState.menuPanel, gameState.renderer);
    drawFontButton(startGame, gameState.font, gameState.renderer);
    drawFontButton(signIn, gameState.font, gameState.renderer);
    drawFontButton(leaderBoards, gameState.font, gameState.renderer);

    SDL_RenderCopy(gameState.renderer, gameState.atlas.texture, &srcRect, &titleRect);
    
    SDL_RenderPresent(gameState.renderer);
}