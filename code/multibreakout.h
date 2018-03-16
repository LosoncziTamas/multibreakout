#ifndef MULTIBREAKOUT_H
#define MULTIBREAKOUT_H

#include <SDL2/SDL.h>

#include "sdl_multibreakout.h"
#include "multibreakout_entity.h"
#include "multibreakout_math.h"
#include "multibreakout_memory.h"

extern "C" void gameUpdate(GameMemory *gameMemory, GameInput *GameInput, SDL_Renderer *renderer);

struct GameState
{
    MemoryPartition entityMemory;

    Entity entities[256];
    Uint32 entityCount;

    Vec2 worldDimensions;
    float tileSideInMeters;
    Uint32 tilesPerWidth;
    Uint32 tilesPerHeight;

    Vec2 screenDimensions;
    Rect worldInScreenSpace;
    Uint32 tileSideInPixels;
    float metersToPixels;
};

#endif