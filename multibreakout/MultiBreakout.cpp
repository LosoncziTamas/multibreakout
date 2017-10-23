#include <iostream>

#include "MultiBreakout.hpp"
#include "Renderer.hpp"
#include "Atlas.hpp"

void readFile(const char* path, std::string& content) {
    char *fileContent = NULL;
    SDL_RWops *file = SDL_RWFromFile(path, "r");
    if (file) {
        Sint64 size = SDL_RWsize(file);
        fileContent = (char*) malloc(size + 1);
        if(SDL_RWread(file, fileContent, sizeof(char), size)) {
            fileContent[size] = '\0';
            content.assign(fileContent);
        } else {
            printf("Error: Unable to read file! SDL Error: %s\n", SDL_GetError());
            free(fileContent);
        }
        free(fileContent);
        SDL_RWclose(file);
    }
    else {
        printf("Error: Unable to open file! SDL Error: %s\n", SDL_GetError());
    }
}

void initGameState(GameState& gameState) {
    gameState.leftBoundary = 160;
    gameState.rightBoundary = SCREEN_WIDTH - 160;
    gameState.init = true;
    initPaddle(gameState.paddle);
    initUpperEnemy(gameState.enemyUpper);
    initLeftEnemy(gameState.enemyLeft, gameState.leftBoundary);
    initRightEnemy(gameState.enemyRight, gameState.rightBoundary);
    initBricks(gameState.bricks);
    Ball ball;
    initBall(ball, gameState.balls, gameState.paddle);
    Ball ball2;
    initBall(ball2, gameState.balls, gameState.enemyUpper.paddle);
    Ball ball3;
    initBall(ball3, gameState.balls, gameState.enemyLeft.paddle);
    Ball ball4;
    initBall(ball4, gameState.balls, gameState.enemyRight.paddle);
    initObstacles(gameState.obstacles);
}

static Atlas atlas;

void gameUpdate(GameState& gameState, Renderer& renderer) {
    if (!gameState.init) {
        srand(time(NULL));
        initGameState(gameState);
        initTextures(renderer, gameState);
        std::string json;
        readFile("texture_atlas.json", json);
        initAtlas(atlas, json, renderer);
    }
    
    if (gameState.input.pause) {
        gameState.paused = gameState.paused == true ? false : true;
    }
    
    if (gameState.paused) {
        return;
    }
    
    updateBalls(gameState);
    updatePaddle(gameState);
    updateEnemy(gameState.enemyUpper, gameState.obstacles, gameState.balls, gameState.delta);
    updateEnemy(gameState.enemyLeft, gameState.obstacles, gameState.balls, gameState.delta);
    updateEnemy(gameState.enemyRight, gameState.obstacles, gameState.balls, gameState.delta);
    
    
    collideWithBrick(gameState.balls, gameState.bricks);
    collideWithObstacle(gameState.balls, gameState.obstacles);
    resolveCollision(gameState.balls, gameState.enemyUpper.paddle, gameState.delta);
    resolveCollision(gameState.balls, gameState.paddle, gameState.delta);
    resolveCollision(gameState.balls, gameState.enemyLeft.paddle, gameState.delta);
    resolveCollision(gameState.balls, gameState.enemyRight.paddle, gameState.delta);
    collideBalls(gameState.balls);
    
    clear(renderer);
    drawLeftPaddle(renderer, gameState.enemyLeft.paddle);
    drawRightPaddle(renderer, gameState.enemyRight.paddle);
    drawLowerPaddle(renderer, gameState.paddle);
    drawUpperPaddle(renderer, gameState.enemyUpper.paddle);
    drawBalls(renderer, gameState.balls, gameState.delta);
    drawBoundaries(renderer, gameState.leftBoundary, gameState.rightBoundary);
    
    drawBallFromTextureAtlas(renderer, gameState.balls[0], atlas);
    
    drawBricks(renderer, gameState.bricks);
    drawObstacles(renderer, gameState.obstacles);
    
    drawBricksDebug(renderer, gameState.bricks);
    drawPaddleDebug(renderer, gameState.paddle);
    drawPaddleDebug(renderer, gameState.enemyUpper.paddle);
    drawPaddleDebug(renderer, gameState.enemyLeft.paddle);
    drawPaddleDebug(renderer, gameState.enemyRight.paddle);
    drawBallsDebug(renderer, gameState.balls);
    drawDebugInfo(renderer, gameState);
    
    drawPoint(renderer, gameState.enemyUpper.steeringPos);
    drawPoint(renderer, gameState.enemyRight.steeringPos);
    drawPoint(renderer, gameState.enemyLeft.steeringPos);
    
    update(renderer);
}
