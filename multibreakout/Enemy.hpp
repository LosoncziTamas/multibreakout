#ifndef Enemy_hpp
#define Enemy_hpp

#include "Vec2.hpp"
#include "Paddle.hpp"
#include "Obstacle.hpp"

enum State {none, steering, defending};

struct Enemy {
    Paddle paddle;
    State state;
    Vec2 steeringPos;
};

void initLeftEnemy(Enemy& enemy, float leftBoundary);
void initRightEnemy(Enemy& enemy, float rightBoundary);
void initUpperEnemy(Enemy& enemy);
void updateEnemy(Enemy& enemy, Obstacles& obstacles, std::vector<Ball>& balls, float delta);

#endif 
