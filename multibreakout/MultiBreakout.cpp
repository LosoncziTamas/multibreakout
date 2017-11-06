#include <iostream>

#include "MultiBreakout.hpp"
#include "Renderer.hpp"
#include "Button.hpp"
#include "TextureButton.hpp"
#include "FontButton.hpp"
#include "NinePatch.hpp"
#include "GameState.hpp"

void initGameWorld(World& world) {
    world.leftBoundary = 160;
    world.rightBoundary = SCREEN_WIDTH - 160;
    world.initialized = true;
    initPaddle(world.paddle);
    initUpperEnemy(world.enemyUpper);
    initLeftEnemy(world.enemyLeft, world.leftBoundary);
    initRightEnemy(world.enemyRight, world.rightBoundary);
    initBricks(world.bricks);
    Ball ball;
    initBall(ball, world.balls, world.paddle);
    Ball ball2;
    initBall(ball2, world.balls, world.enemyUpper.paddle);
    Ball ball3;
    initBall(ball3, world.balls, world.enemyLeft.paddle);
    Ball ball4;
    initBall(ball4, world.balls, world.enemyRight.paddle);
    initObstacles(world.obstacles);
    
    world.paddle.textureIndex = PLAYER_PADDLE;
    world.enemyLeft.paddle.textureIndex = PLAYER_PADDLE;
    for (auto& ball : world.balls) {
        ball.textureIndex = GREY_BALL;
    };
    for (auto& brick : world.bricks) {
        brick.textureIndex = getBrickTexture(brick.powerUp);
    };
    world.enemyUpper.paddle.textureIndex = ENEMY_PADDLE;
    world.enemyRight.paddle.textureIndex = ENEMY_PADDLE;
    for (int i = 0; i < OBSTACLES_SIZE; ++i) {
        world.obstacles.content[i].textureIndex = GREY_BRICK;
    }
}

void onLeftClick(GameInput& gameInput) {
    gameInput.left = true;
}

void onRightClick(GameInput& gameInput) {
    gameInput.right = true;
}

static TextureButton leftButton = {15, 30, 130, 130, LEFT_BUTTON, onLeftClick};
static TextureButton rightButton = {SCREEN_WIDTH - 145, 30, 130, 130, RIGHT_BUTTON, onRightClick};

void gamePlayUpdate(GameState& gameState) {
    if (!gameState.world.initialized) {
        initGameWorld(gameState.world);
    }
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == true ? false : true;
    }
    
    if (gameState.paused) {
        return;
    }
    
    updateButton(leftButton, gameState.input);
    updateButton(rightButton, gameState.input);
    
    updateBalls(gameState.world, gameState.input, gameState.delta);
    updatePaddle(gameState.world, gameState.input, gameState.delta);
    updateEnemy(gameState.world.enemyUpper, gameState.world.obstacles, gameState.world.balls, gameState.delta);
    updateEnemy(gameState.world.enemyLeft, gameState.world.obstacles, gameState.world.balls, gameState.delta);
    updateEnemy(gameState.world.enemyRight, gameState.world.obstacles, gameState.world.balls, gameState.delta);
    
    collideWithBrick(gameState.world.balls, gameState.world.bricks);
    resolveCollision(gameState.world.balls, gameState.world.enemyUpper.paddle, gameState.delta);
    resolveCollision(gameState.world.balls, gameState.world.paddle, gameState.delta);
    resolveCollision(gameState.world.balls, gameState.world.enemyLeft.paddle, gameState.delta);
    resolveCollision(gameState.world.balls, gameState.world.enemyRight.paddle, gameState.delta);
    collideWithObstacle(gameState.world.balls, gameState.world.obstacles);
    collideBalls(gameState.world.balls);
    
    SDL_Renderer* renderer = gameState.renderer;
    Atlas& atlas = gameState.atlas;
    
    clear(renderer, SKY_BLUE);
    
    drawLeftPaddle(renderer, atlas, gameState.world.enemyLeft.paddle);
    drawRightPaddle(renderer, atlas, gameState.world.enemyRight.paddle);
    drawLowerPaddle(renderer, atlas, gameState.world.paddle);
    drawUpperPaddle(renderer, atlas, gameState.world.enemyUpper.paddle);
    drawBalls(renderer, atlas, gameState.world.balls);
    drawBricks(renderer, atlas, gameState.world.bricks);
    drawObstacles(renderer, atlas, gameState.world.obstacles);
    
    drawBricksDebug(renderer, gameState.world.bricks);
    drawPaddleDebug(renderer, gameState.world.paddle);
    drawPaddleDebug(renderer, gameState.world.enemyUpper.paddle);
    drawPaddleDebug(renderer, gameState.world.enemyLeft.paddle);
    drawPaddleDebug(renderer, gameState.world.enemyRight.paddle);
    drawBallsDebug(renderer, gameState.world.balls);
    
    drawPoint(renderer, gameState.world.enemyUpper.steeringPos, RED);
    drawPoint(renderer, gameState.world.enemyRight.steeringPos, RED);
    drawPoint(renderer, gameState.world.enemyLeft.steeringPos, RED);
    
    drawNinePatch(gameState.ninePatchTextures, gameState.leftPanel, renderer);
    drawNinePatch(gameState.ninePatchTextures, gameState.rightPanel, renderer);
    
    drawButton(renderer, atlas, leftButton);
    drawButton(renderer, atlas, rightButton);
    
    drawDebugInfo(renderer, gameState.font, gameState.world, gameState.delta);
    
    SDL_RenderPresent(renderer);
    
}

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

static FontButton startGame;

void onStartClick(GameInput& input) {
    
}

void menuUpdate(GameState& gameState) {
    clear(gameState.renderer, SKY_BLUE);
    if (redraw) {
        int w = 400;
        int x = SCREEN_WIDTH * 0.5f - w * 0.5f;
        int h = 300;
        int y = SCREEN_HEIGHT * 0.5f - h * 0.5f;
        gameState.menuPanel = {x, y, w, h, MENU_PANEL};
        generatePatches(gameState);
        redraw = false;
        
        const char* text = "Start game";
        startGame.x = x;
        startGame.y = y;
        startGame.w = FC_GetWidth(gameState.font, text);
        startGame.h = FC_GetHeight(gameState.font, text);
        startGame.text = text;
        startGame.onclick = onStartClick;
    }
    updateFontButton(startGame, gameState.input);
    drawNinePatch(gameState.ninePatchTextures, gameState.menuPanel, gameState.renderer);
    drawFontButton(startGame, gameState.font, gameState.renderer);
    SDL_RenderPresent(gameState.renderer);
}

extern "C" void gameUpdate(GameState& gameState) {
    if (!gameState.initialized) {
        srand(time(NULL));
        gameState.font = createFont(gameState.renderer);
        initTextures(gameState.renderer, gameState.atlas, gameState.world);

        gameState.leftPanel = {0, 0, 160, SCREEN_HEIGHT, LEFT_PANEL};
        gameState.rightPanel = {SCREEN_WIDTH - 160, 0, 160, SCREEN_HEIGHT, RIGHT_PANEL};
        gameState.menuPanel = {50, 50, 50, 50, MENU_PANEL};
        generatePatches(gameState);

        gameState.initialized = true;
        gameState.currScreen = menu;
    }
    
    switch (gameState.currScreen) {
        case menu:
            menuUpdate(gameState);
            break;
        case game:
            gamePlayUpdate(gameState);
            break;
        default:
            break;
    }
}
