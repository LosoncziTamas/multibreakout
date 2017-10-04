#include "Paddle.hpp"
#include "MultiBreakout.hpp"
#include "Rect.hpp"
#include "GameState.hpp"

void initPaddle(Paddle &paddle);
void update(Paddle& paddle, Ball& ball, const GameInput& input, float delta, const SDL_Rect& leftWall, const SDL_Rect& rightWall);
void collide(float delta, Paddle& paddle, Ball& ball, const Renderer& renderer);

void initPaddle(Paddle &paddle) {
    paddle.width = 120.0f;
    paddle.height = 40.0f;
    paddle.centerPos = Vec2(SCREEN_WIDTH / 2, 250);
    paddle.speed = 50.0f;
}

void updatePaddle(GameState& gameState, const Renderer& renderer) {
    Paddle &paddle = gameState.paddle;
    float delta = gameState.delta;
    
    Vec2 acceleration;
    if (gameState.input.left) {
        if (paddle.velocity.x > 0){
            paddle.velocity.x = 0.0f;
        }
        acceleration.x = -1.0;
    } else if (gameState.input.right) {
        if (paddle.velocity.x < 0){
            paddle.velocity.x = 0.0f;
        }
        acceleration.x = 1.0;
    } else {
        paddle.velocity += - 0.02 * paddle.velocity;
    }
    
    acceleration *= paddle.speed;

    paddle.oldPos = paddle.centerPos;
    paddle.movementDelta = (0.5f * acceleration * pow(delta, 2) + paddle.velocity * delta);
    paddle.velocity += acceleration * delta;
    paddle.newPos = paddle.oldPos + paddle.movementDelta;
    
    float offset = paddle.width * 0.5f;
    if (paddle.newPos.x - offset < gameState.leftWall.w) {
        Vec2 wallNorm(1, 0);
        Vec2 reflection = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.velocity = reflection;
        paddle.movementDelta.x += 1.0f;
    } else if (paddle.newPos.x + offset > gameState.rightWall.x) {
        Vec2 wallNorm(-1, 0);
        paddle.velocity = paddle.velocity - 2 * paddle.velocity.dotProduct(wallNorm) * wallNorm;
        paddle.movementDelta.x -= 1.0f;
    }
}


void reflect(const Vec2& norm, Ball& ball, const Vec2& playerDelta) {
    ball.velocity = norm;
    ball.center += norm * playerDelta.length();
}

bool collide(Vec2& ballPos, Vec2& paddlePos, GameState& gameState) {
    //TODO: paddle center pos
    
    float verticalDist = fabsf(ballPos.y - (paddlePos.y + gameState.paddle.height / 2));
    float horizontalDist = fabsf(ballPos.x - paddlePos.x);
    
    if (horizontalDist > gameState.ball.radius + gameState.paddle.width / 2) {
        return false;
    }
    
    if (verticalDist > gameState.ball.radius + gameState.paddle.height / 2) {
        return false;
    }
    
    if (horizontalDist <= gameState.paddle.width / 2) {
        return true;
    }

    if (verticalDist <= gameState.paddle.height / 2) {
        return true;
    }
    
    float dx = horizontalDist - gameState.paddle.width / 2;
    float dy = verticalDist - gameState.paddle.height / 2;
    
    return (dx * dx + dy * dy <= (gameState.ball.radius * gameState.ball.radius));
}

void debugCollision(GameState& gameState, const Renderer& renderer) {
    Ball& ball = gameState.ball;
    Paddle& paddle = gameState.paddle;
    SDL_Color collisionColor = {0, 0, 0, 255};
    
    if (collide(ball.newPos, paddle.newPos, gameState)) {
        for (int i = 1; i <= 4; ++i) {
            float scale1 = (1.0f - 1.0f / i);
            float scale2 = (1.0f / i);
            Vec2 ballCollisionLocation = scale2 * ball.oldPos + scale1 * ball.newPos;
            Vec2 paddleCollisionLocation = scale2 * paddle.oldPos + scale1 * paddle.newPos;
            if (collide(ballCollisionLocation, paddleCollisionLocation, gameState)) {
                Vec2 reflection = ballCollisionLocation - paddleCollisionLocation;
                reflection.y = ballCollisionLocation.y - (paddleCollisionLocation.y + paddle.height / 2);
                reflection.normalize();
                reflect(reflection, ball, paddle.movementDelta);
                collisionColor = {255, 0, 0, 255};
                break;
            }
        }
    }
    ball.center += ball.velocity * ball.speed * gameState.delta;
    renderer.drawBall(ball, collisionColor);
    paddle.centerPos = paddle.newPos;
}