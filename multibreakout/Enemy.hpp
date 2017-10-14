#ifndef Enemy_hpp
#define Enemy_hpp

#include "Vec2.hpp"
#include "Paddle.hpp"

enum State {none, steering, defending};

struct Enemy {
    Paddle paddle;
    State state;
    Vec2 steeringPos;
};

void initEnemy(Enemy& enemy);
void updateEnemy(Enemy& enemy, std::vector<Ball>& balls, float delta, float leftBoundary, float rightBoundary);

#endif 
