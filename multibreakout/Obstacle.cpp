#include "Obstacle.hpp"
#include "MultiBreakout.hpp"

void initObstacles(Obstacles& obstacles) {
    const int width = 30;
    const int height = 30;
    
    obstacles.leftBottom.width = width;
    obstacles.leftBottom.height = height;
    obstacles.leftBottom.center.x = 160 + width * 0.5f;
    obstacles.leftBottom.center.y = height * 0.5f;
    
    obstacles.leftTop.width = width;
    obstacles.leftTop.height = height;
    obstacles.leftTop.center.x = 160 + width * 0.5f;
    obstacles.leftTop.center.y = SCREEN_HEIGHT - height * 0.5f;
    
    obstacles.rightBottom.width = width;
    obstacles.rightBottom.height = height;
    obstacles.rightBottom.center.x = SCREEN_WIDTH - 160 - width * 0.5f;
    obstacles.rightBottom.center.y = height * 0.5f;
    
    obstacles.rightTop.width = width;
    obstacles.rightTop.height = height;
    obstacles.rightTop.center.x = SCREEN_WIDTH - 160 - width * 0.5f;
    obstacles.rightTop.center.y = SCREEN_HEIGHT - height * 0.5f;

}

bool collide(Ball& ball, Obstacle& obstacle) {
    float verticalDist = fabsf(ball.newPos.y - obstacle.center.y);
    float horizontalDist = fabsf(ball.newPos.x - obstacle.center.x);
    float halfWidth = obstacle.width * 0.5f;
    float halfHeight = obstacle.height * 0.5f;
    
    if (horizontalDist > ball.radius + halfWidth) {
        return false;
    }
    
    if (verticalDist > ball.radius + halfHeight) {
        return false;
    }
    
    if (horizontalDist <= halfWidth) {
        return true;
    }
    
    if (verticalDist <= halfHeight) {
        return true;
    }
    
    float dx = horizontalDist - halfWidth;
    float dy = verticalDist - halfHeight;
    
    return (dx * dx + dy * dy <= ball.radius * ball.radius);
}

void collideWithObstacle(std::vector<Ball>& balls, Obstacles& obstacles) {
    for (auto& ball : balls) {
        for (int i = 0; i < OBSTACLES_SIZE; ++i) {
            if (collide(ball, obstacles.content[i])) {
                Vec2 reflectionNorm = ball.newPos - obstacles.content[i].center;
                reflectionNorm.normalize();
                ball.velocity = reflectionNorm;
                ball.movementDelta += reflectionNorm;
            }
        }
    }
}