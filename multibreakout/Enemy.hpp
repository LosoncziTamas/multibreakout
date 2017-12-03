#ifndef Enemy_hpp
#define Enemy_hpp

#include "Vec2.hpp"
#include "Paddle.hpp"
#include "Obstacle.hpp"

struct FC_Font;

enum State {none, steering, defending};

struct Enemy {
    Paddle paddle;
    State state;
    Vec2 steeringPos;
};

void initLeftEnemy(Enemy* enemy, float leftBoundary);

void initRightEnemy(Enemy* enemy, float rightBoundary);

void initUpperEnemy(Enemy* enemy);

void updateEnemy(World* world, Enemy* enemy, float delta);

void drawDebugInfo(SDL_Renderer* renderer, FC_Font* font, World* world, float delta);

#endif 
