#include "Ball.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"

const float BALL_RADIUS = 10.0f;
const float BALL_SPEED = 100.0f;

void initBall(Ball& ball, std::vector<Ball>& balls, Paddle& paddle) {
    ball.radius = BALL_RADIUS;
    ball.speed = BALL_SPEED;
    ball.powerUp = neutral;
    ball.assignedPaddle = &paddle;
    paddle.ballIndex = balls.size();
    ball.newPos = paddle.newPos;
    ball.textureIndex = INVALID_INDEX;
    if (paddle.orientation == lower) {
        ball.newPos.y += ball.radius + paddle.height * 0.5f + 1;
    } else if (paddle.orientation == upper) {
        ball.newPos.y -= ball.radius + paddle.height * 0.5f + 1;
    }
    balls.push_back(ball);
}

void collideBalls(std::vector<Ball>& balls) {
    size_t n = balls.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            Ball& ball1 = balls.at(i);
            Ball& ball2 = balls.at(j);
            float distance = ball1.newPos.distance(ball2.newPos);
            //TODO: use quadtree or bounding boxes
            if (distance <= ball1.radius + ball2.radius) {
                Vec2 reflection1 = ball1.newPos - ball2.newPos;
                Vec2 reflection2 = ball2.newPos - ball1.newPos;
                ball1.velocity = reflection1.normalize();
                ball2.velocity = reflection2.normalize();
                ball1.movementDelta += reflection1 * ball1.movementDelta.length();
                ball2.movementDelta += reflection2 * ball2.movementDelta.length();
            }
        }
    }
}

void updateBalls(GameState& gameState) {
    GameInput& input = gameState.input;
    
    for (auto& ball : gameState.balls) {
        
        if (input.mouseRight) {
            Vec2 newVelocity(input.mouseX - ball.newPos.x, SCREEN_HEIGHT - input.mouseY - ball.newPos.y);
            ball.velocity = newVelocity.normalize();
        }
        
        if (input.space && ball.assignedPaddle != nullptr && ball.assignedPaddle->orientation == lower) {
            ball.velocity = Vec2(0.0f, 1.0f);
            ball.movementDelta = ball.velocity * ball.speed * gameState.delta;
            ball.assignedPaddle->ballIndex = INVALID_INDEX;
            ball.assignedPaddle = nullptr;
        } else if (ball.assignedPaddle != nullptr) {
            ball.movementDelta = ball.assignedPaddle->movementDelta;
            ball.velocity = Vec2(0.0f, 0.0f);
        } else {
            ball.movementDelta = ball.velocity * ball.speed * gameState.delta;
        }
                
        ball.oldPos = ball.newPos;
        ball.newPos = ball.oldPos + ball.movementDelta;
        
        float ballTop = ball.newPos.y + ball.radius;
        if (ballTop >= SCREEN_HEIGHT) {
            Vec2 wallNorm(0.0f, -1.0f);
            ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
            ball.movementDelta.y += SCREEN_HEIGHT - ballTop;
        }
        
        float ballBottom = ball.newPos.y - ball.radius;
        if (ball.newPos.y - ball.radius <= 0) {
            Vec2 wallNorm(0.0f, 1.0f);
            ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
            ball.movementDelta.y -= ballBottom;
        }
        
        float ballLeft = ball.newPos.x - ball.radius;
        if (ballLeft <= gameState.leftBoundary) {
            Vec2 wallNorm(1.0f, 0.0f);
            ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
            ball.movementDelta.x += gameState.leftBoundary - ballLeft;
        }
        
        float ballRight = ball.newPos.x + ball.radius;
        if (ballRight >= gameState.rightBoundary) {
            Vec2 wallNorm(-1.0f, 0.0f);
            ball.velocity = ball.velocity - 2 * ball.velocity.dotProduct(wallNorm) * wallNorm;
            ball.movementDelta.x += gameState.rightBoundary - ballRight;
        }
    }
}