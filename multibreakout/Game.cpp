#include "Game.hpp"
#include "Texture.hpp"
#include "GameState.hpp"
#include "Paddle.hpp"

void initalizeGameWorld(World& world) {
    world.bounds.bottomLeft = Vec2(160, 0);
    world.bounds.topRight = Vec2(SCREEN_WIDTH - 160, SCREEN_HEIGHT);
    
    initPaddle(world.paddle);
    initUpperEnemy(world.enemyUpper);
    initBricks(world);
    initObstacles(world.obstacles);
    addBall(world, world.paddle);
    
    world.paddle.textureIndex = PLAYER_PADDLE;
    world.enemyUpper.paddle.textureIndex = ENEMY_PADDLE;
    
    if (world.type == twoVsTwo) {
        initLeftEnemy(world.enemyLeft, world.bounds.bottomLeft.x);
        initRightEnemy(world.enemyRight, world.bounds.topRight.x);
        addBall(world, world.enemyRight.paddle);
        
        world.enemyRight.paddle.textureIndex = ENEMY_PADDLE;
        world.enemyLeft.paddle.textureIndex = PLAYER_PADDLE;
    }
    
}

void onLeftClick(GameInput& gameInput) {
    gameInput.left = true;
}

void onRightClick(GameInput& gameInput) {
    gameInput.right = true;
}

void initializeUi(GameUi& gameUi) {
    gameUi.leftButton = {15, 30, 130, 130, LEFT_BUTTON, onLeftClick};
    gameUi.rightButton = {SCREEN_WIDTH - 145, 30, 130, 130, RIGHT_BUTTON, onRightClick};
}

void updateUi(GameUi& gameUi, GameInput& input) {
    updateButton(gameUi.leftButton, input);
    updateButton(gameUi.rightButton, input);
}

void updateGame(World& world, GameInput& input, float delta) {
    updateBalls(world, input, delta);
    updatePaddle(world, input, delta);
    updateEnemy(world, world.enemyUpper, delta);
    
    if (world.type == twoVsTwo) {
        updateEnemy(world, world.enemyLeft, delta);
        updateEnemy(world, world.enemyRight, delta);
    }
    
    collideWithBrick(world);
    resolveCollision(world, world.enemyUpper.paddle, delta);
    resolveCollision(world, world.paddle, delta);
    
    if (world.type == twoVsTwo) {
        resolveCollision(world, world.enemyLeft.paddle, delta);
        resolveCollision(world, world.enemyRight.paddle, delta);
    }
    collideWithObstacle(world, world.obstacles);
    collideBalls(world);
    
    if (input.mouseLeft) {
        addProjectile(Vec2(input.mouseX, SCREEN_HEIGHT - input.mouseY), &world);
    }
}

void drawGame(SDL_Renderer* renderer, Atlas& atlas, World& world, float delta) {
    
    drawLowerPaddle(renderer, atlas, world.paddle);
    drawUpperPaddle(renderer, atlas, world.enemyUpper.paddle);
    drawBalls(renderer, atlas, world);
    drawBricks(renderer, atlas, world);
    
    if (world.type == twoVsTwo) {
        drawLeftPaddle(renderer, atlas, world.enemyLeft.paddle);
        drawRightPaddle(renderer, atlas, world.enemyRight.paddle);
        drawPaddleDebug(renderer, world.enemyLeft.paddle);
        drawPaddleDebug(renderer, world.enemyRight.paddle);
    }
    drawObstacles(renderer, atlas, world.obstacles);

    drawBricksDebug(renderer, world);
    drawPaddleDebug(renderer, world.paddle);
    drawPaddleDebug(renderer, world.enemyUpper.paddle);
    drawBallsDebug(renderer, world);
    drawPoint(renderer, world.enemyUpper.steeringPos, RED);
    
    if (world.type == twoVsTwo) {
        drawPoint(renderer, world.enemyRight.steeringPos, RED);
        drawPoint(renderer, world.enemyLeft.steeringPos, RED);
    }
}

void drawUi(SDL_Texture** ninePatchTextures, GameUi& gameUi, SDL_Renderer* renderer, Atlas& atlas) {
    drawNinePatch(ninePatchTextures, gameUi.leftPanel, renderer);
    drawNinePatch(ninePatchTextures, gameUi.rightPanel, renderer);
    drawButton(renderer, atlas, gameUi.leftButton);
    drawButton(renderer, atlas, gameUi.rightButton);
}

void gamePlayUpdate(GameState& gameState) {
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == true ? false : true;
    }
    if (gameState.paused) {
        return;
    }
    
    clear(gameState.renderer, SKY_BLUE);
    updateUi(gameState.gameUi, gameState.input);
    updateGame(gameState.world, gameState.input, gameState.delta);
    drawGame(gameState.renderer, gameState.atlas, gameState.world, gameState.delta);
    drawUi(gameState.ninePatchTextures, gameState.gameUi, gameState.renderer, gameState.atlas);
    drawDebugInfo(gameState.renderer, gameState.font, gameState.world, gameState.delta);
    updateProjectiles(&gameState.world, gameState.renderer, gameState.delta);
    SDL_RenderPresent(gameState.renderer);
}
