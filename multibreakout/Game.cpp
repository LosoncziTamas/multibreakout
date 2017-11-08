#include "Game.hpp"
#include "Texture.hpp"
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

void gamePlayUpdate(GameState& gameState) {
    if (!gameState.world.initialized) {
        initGameWorld(gameState.world);
        gameState.gameScreen.leftButton = {15, 30, 130, 130, LEFT_BUTTON, onLeftClick};
        gameState.gameScreen.rightButton = {SCREEN_WIDTH - 145, 30, 130, 130, RIGHT_BUTTON, onRightClick};
    }
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == true ? false : true;
    }
    
    if (gameState.paused) {
        return;
    }
    
    updateButton(gameState.gameScreen.leftButton, gameState.input);
    updateButton(gameState.gameScreen.rightButton, gameState.input);
    
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
    
    drawNinePatch(gameState.ninePatchTextures, gameState.gameScreen.leftPanel, renderer);
    drawNinePatch(gameState.ninePatchTextures, gameState.gameScreen.rightPanel, renderer);
    
    drawButton(renderer, atlas, gameState.gameScreen.leftButton);
    drawButton(renderer, atlas, gameState.gameScreen.rightButton);
    
    drawDebugInfo(renderer, gameState.font, gameState.world, gameState.delta);
    
    SDL_RenderPresent(renderer);
    
}
