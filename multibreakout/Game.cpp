#include "Game.hpp"
#include "Texture.hpp"
#include "GameState.hpp"
#include "Paddle.hpp"

void initalizeGameWorld(World* world) {
    world->bounds.bottomLeft = Vec2(160, 0);
    world->bounds.topRight = Vec2(SCREEN_WIDTH - 160, SCREEN_HEIGHT);
    
    initPaddle(&world->paddle);
    initUpperEnemy(&world->enemyUpper);
    initBricks(world);
    initObstacles(world->obstacles);
    addBall(world, &world->paddle);
    
    world->paddle.textureIndex = PLAYER_PADDLE;
    world->enemyUpper.paddle.textureIndex = ENEMY_PADDLE;
    
    if (world->type == twoVsTwo) {
        initLeftEnemy(&world->enemyLeft, world->bounds.bottomLeft.x);
        initRightEnemy(&world->enemyRight, world->bounds.topRight.x);
        addBall(world, &world->enemyRight.paddle);
        
        world->enemyRight.paddle.textureIndex = ENEMY_PADDLE;
        world->enemyLeft.paddle.textureIndex = PLAYER_PADDLE;
    }
    
    world->initialized = true;
}


void onLeftClick(GameInput& gameInput) {
    gameInput.left = true;
}

void onRightClick(GameInput& gameInput) {
    gameInput.right = true;
}

void initializeUi(GameUi* gameUi) {
    gameUi->leftButton = {15, 30, 130, 130, LEFT_BUTTON, onLeftClick};
    gameUi->rightButton = {SCREEN_WIDTH - 145, 30, 130, 130, RIGHT_BUTTON, onRightClick};
    gameUi->initialized = true;
}

void gamePlayUpdate(GameState* gameState) {
    if (gameState->input.pause) {
        gameState->paused = gameState->paused == true ? false : true;
    }
    if (gameState->paused) {
        return;
    }
    
    clear(gameState->renderer, SKY_BLUE);
    
    World& world = gameState->world;
    float delta = gameState->delta;
    
    updateButton(gameState->gameUi.leftButton, gameState->input);
    updateButton(gameState->gameUi.rightButton, gameState->input);
    
    updateBalls(&world, &gameState->input, delta);
    updatePaddle(&world, &gameState->input, &gameState->oldInput, delta);
    updateEnemy(&world, &world.enemyUpper, delta);
    
    if (world.type == twoVsTwo) {
        updateEnemy(&world, &world.enemyLeft, delta);
        updateEnemy(&world, &world.enemyRight, delta);
    }
    
    collideWithBrick(world);
    resolveCollision(world, world.enemyUpper.paddle, delta);
    resolveCollision(world, world.paddle, delta);
    
    if (world.type == twoVsTwo) {
        resolveCollision(world, world.enemyLeft.paddle, delta);
        resolveCollision(world, world.enemyRight.paddle, delta);
    }
    
    collideWithObstacle(world, world.obstacles);
    collideBalls(&world);
    
    drawLowerPaddle(gameState->renderer, &gameState->atlas, &world.enemyUpper.paddle);
    drawUpperPaddle(gameState->renderer, &gameState->atlas, &world.paddle);
    drawBalls(gameState->renderer, &gameState->atlas, &world);
    drawBricks(gameState->renderer, &gameState->atlas, &world);
    
    if (world.type == twoVsTwo) {
        drawLeftPaddle(gameState->renderer, &gameState->atlas, &world.enemyLeft.paddle);
        drawRightPaddle(gameState->renderer, &gameState->atlas, &world.enemyRight.paddle);
        drawPaddleDebug(gameState->renderer, &world.enemyLeft.paddle);
        drawPaddleDebug(gameState->renderer, &world.enemyRight.paddle);
    }
    
    drawObstacles(gameState->renderer, &gameState->atlas, &world.obstacles);
    
    drawBricksDebug(gameState->renderer, world);
    drawPaddleDebug(gameState->renderer, &world.paddle);
    drawPaddleDebug(gameState->renderer, &world.enemyUpper.paddle);
    drawBallsDebug(gameState->renderer, &world);
    drawPoint(gameState->renderer, world.enemyUpper.steeringPos, RED);
    
    if (world.type == twoVsTwo) {
        drawPoint(gameState->renderer, world.enemyRight.steeringPos, RED);
        drawPoint(gameState->renderer, world.enemyLeft.steeringPos, RED);
    }
    
    drawNinePatch(gameState->ninePatchTextures, gameUi.leftPanel, gameState->renderer);
    drawNinePatch(gameState->ninePatchTextures, gameUi.rightPanel, gameState->renderer);
    drawButton(gameState->renderer, gameState->atlas, gameUi.leftButton);
    drawButton(gameState->renderer, gameState->atlas, gameUi.rightButton);
    
    drawDebugInfo(gameState->renderer, gameState->font, &gameState->world, gameState->delta);
    updateProjectiles(&gameState->world, gameState->renderer, gameState->delta);
    SDL_RenderPresent(gameState->renderer);
}
