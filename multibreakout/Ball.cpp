#include "Ball.hpp"
#include "MultiBreakout.hpp"
#include "GameState.hpp"
#include "Physics.hpp"
#include "Renderer.hpp"

Ball *addBall(World& world, Paddle& paddle) {
    SDL_assert(world.ballCount < SDL_arraysize(world.balls));
    
    Uint32 ballIndex = world.ballCount++;
    Ball* ball = world.balls + ballIndex;
    ball->radius = 10.0f;
    ball->speed = 100.0f;
    ball->powerUp = neutral;
    ball->assignedPaddle = &paddle;
    ball->newPos = paddle.newPos;
    ball->textureIndex = GREY_BALL;
    paddle.ballIndex = ballIndex;
    
    if (paddle.orientation == lower) {
        ball->newPos.y += ball->radius + paddle.height * 0.5f + 1;
    } else if (paddle.orientation == upper) {
        ball->newPos.y -= ball->radius + paddle.height * 0.5f + 1;
    } else if (paddle.orientation == left) {
        ball->newPos.x += ball->radius + paddle.width * 0.5f + 1;
    } else if (paddle.orientation == right) {
        ball->newPos.x -= ball->radius + paddle.width * 0.5f + 1;
    }
    
    return ball;
}

//TODO: use quadtree or bounding boxes
void collideBalls(World& world) {
    for (Uint32 ballIndex = 0; ballIndex < world.ballCount; ++ballIndex) {
        Ball* ball = world.balls + ballIndex;
        for (Uint32 otherBallIndex = ballIndex + 1; otherBallIndex < world.ballCount; ++otherBallIndex) {
            Ball* otherBall = world.balls + otherBallIndex;
            float distance = ball->newPos.distance(otherBall->newPos);
            bool collide = distance <= ball->radius + otherBall->radius;
            if (collide) {
                Vec2 ballRef = ball->newPos - otherBall->newPos;
                ball->velocity = ballRef.normalize();
                ball->movementDelta += ballRef * ball->movementDelta.length();
                
                Vec2 otherBallref = otherBall->newPos - ball->newPos;
                otherBall->velocity = otherBallref.normalize();
                otherBall->movementDelta += otherBallref * otherBall->movementDelta.length();
            }
        }
    }
}

void updateBalls(World& world, GameInput& input, float delta) {
    for (Uint32 ballIndex = 0; ballIndex < world.ballCount; ++ballIndex) {
        Ball* ball = world.balls + ballIndex;
        
        if (input.mouseRight) {
            Vec2 newVelocity(input.mouseX - ball->newPos.x, SCREEN_HEIGHT - input.mouseY - ball->newPos.y);
            ball->velocity = newVelocity.normalize();
        }
        
        if (input.space && ball->assignedPaddle != nullptr && ball->assignedPaddle->orientation == lower) {
            ball->velocity = Vec2(0.0f, 1.0f);
            ball->movementDelta = ball->velocity * ball->speed * delta;
            ball->assignedPaddle->ballIndex = INVALID_INDEX;
            ball->assignedPaddle = nullptr;
        } else if (ball->assignedPaddle != nullptr) {
            ball->movementDelta = ball->assignedPaddle->movementDelta;
            ball->velocity = Vec2(0.0f, 0.0f);
        } else {
            ball->movementDelta = ball->velocity * ball->speed * delta;
        }
        
        ball->oldPos = ball->newPos;
        ball->newPos = ball->oldPos + ball->movementDelta;
        
        float ballTop = ball->newPos.y + ball->radius;
        if (ballTop >= SCREEN_HEIGHT) {
            Vec2 wallNorm(0.0f, -1.0f);
            ball->velocity = reflect(ball->velocity, wallNorm);
            ball->movementDelta.y += SCREEN_HEIGHT - ballTop;
        }
        
        float ballBottom = ball->newPos.y - ball->radius;
        if (ball->newPos.y - ball->radius <= 0) {
            Vec2 wallNorm(0.0f, 1.0f);
            ball->velocity = reflect(ball->velocity, wallNorm);
            ball->movementDelta.y -= ballBottom;
        }
        
        float ballLeft = ball->newPos.x - ball->radius;
        if (ballLeft <= world.leftBoundary) {
            Vec2 wallNorm(1.0f, 0.0f);
            ball->velocity = reflect(ball->velocity, wallNorm);
            ball->movementDelta.x += world.leftBoundary - ballLeft;
        }
        
        float ballRight = ball->newPos.x + ball->radius;
        if (ballRight >= world.rightBoundary) {
            Vec2 wallNorm(-1.0f, 0.0f);
            ball->velocity = reflect(ball->velocity, wallNorm);
            ball->movementDelta.x += world.rightBoundary - ballRight;
        }
    }
}

void drawBalls(SDL_Renderer* renderer, Atlas& atlas, World& world) {
    for (Uint32 ballIndex = 0; ballIndex < world.ballCount; ++ballIndex) {
        Ball* ball = world.balls + ballIndex;
        int x = ball->newPos.x - ball->radius;
        int y = SCREEN_HEIGHT - (ball->newPos.y + ball->radius);
        SDL_Rect dstRec = {x, y, SDL_static_cast(Sint32, (ball->radius * 2)), SDL_static_cast(Sint32, (ball->radius * 2))};
        SDL_Rect srcRect = atlas.frames[ball->textureIndex];
        SDL_RenderCopy(renderer, atlas.texture, &srcRect, &dstRec);
    }
}

void drawBallsDebug(SDL_Renderer* renderer, World& world) {
    for (Uint32 ballIndex = 0; ballIndex < world.ballCount; ++ballIndex) {
        Ball* ball = world.balls + ballIndex;
        
        SDL_Color color = getDrawColor(ball->powerUp);
        int x0 = round(ball->newPos.x);
        int y0 = round(SCREEN_HEIGHT - ball->newPos.y);
        int radius = round(ball->radius);
        
        int x = radius-1;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int err = dx - (radius << 1);
        
        while (x >= y) {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
            SDL_RenderDrawPoint(renderer, x0 + y, y0 + x);
            SDL_RenderDrawPoint(renderer, x0 - y, y0 + x);
            SDL_RenderDrawPoint(renderer, x0 - x, y0 + y);
            SDL_RenderDrawPoint(renderer, x0 - x, y0 - y);
            SDL_RenderDrawPoint(renderer, x0 - y, y0 - x);
            SDL_RenderDrawPoint(renderer, x0 + y, y0 - x);
            SDL_RenderDrawPoint(renderer, x0 + x, y0 - y);
            
            if (err <= 0) {
                y++;
                err += dy;
                dy +=2;
            }
            if (err > 0) {
                x--;
                dx += 2;
                err += (-radius << 1) + dx;
            }
        }
    }
}