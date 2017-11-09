#include "Game.hpp"
#include "Texture.hpp"
#include "GameState.hpp"

void initalizeGameWorld(World& world) {
    world.leftBoundary = 160;
    world.rightBoundary = SCREEN_WIDTH - 160;
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
    updateEnemy(world.enemyUpper, world.obstacles, world.balls, delta);
    updateEnemy(world.enemyLeft, world.obstacles, world.balls, delta);
    updateEnemy(world.enemyRight, world.obstacles, world.balls, delta);
    
    collideWithBrick(world.balls, world.bricks);
    resolveCollision(world.balls, world.enemyUpper.paddle, delta);
    resolveCollision(world.balls, world.paddle, delta);
    resolveCollision(world.balls, world.enemyLeft.paddle, delta);
    resolveCollision(world.balls, world.enemyRight.paddle, delta);
    collideWithObstacle(world.balls, world.obstacles);
    collideBalls(world.balls);
}

void drawGame(SDL_Renderer* renderer, Atlas& atlas, World& world, float delta) {
    drawLeftPaddle(renderer, atlas, world.enemyLeft.paddle);
    drawRightPaddle(renderer, atlas, world.enemyRight.paddle);
    drawLowerPaddle(renderer, atlas, world.paddle);
    drawUpperPaddle(renderer, atlas, world.enemyUpper.paddle);
    drawBalls(renderer, atlas, world.balls);
    drawBricks(renderer, atlas, world.bricks);
    drawObstacles(renderer, atlas, world.obstacles);
    
    drawBricksDebug(renderer, world.bricks);
    drawPaddleDebug(renderer, world.paddle);
    drawPaddleDebug(renderer, world.enemyUpper.paddle);
    drawPaddleDebug(renderer, world.enemyLeft.paddle);
    drawPaddleDebug(renderer, world.enemyRight.paddle);
    drawBallsDebug(renderer, world.balls);
    
    drawPoint(renderer, world.enemyUpper.steeringPos, RED);
    drawPoint(renderer, world.enemyRight.steeringPos, RED);
    drawPoint(renderer, world.enemyLeft.steeringPos, RED);
}

void drawUi(std::vector<SDL_Texture*>& ninePatchTextures, GameUi& gameUi, SDL_Renderer* renderer, Atlas& atlas) {
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
    SDL_RenderPresent(gameState.renderer);
}
