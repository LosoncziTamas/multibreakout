#ifndef Memory_hpp
#define Memory_hpp

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_assert.h>

#define pushStruct(memory, type) (type *)pushSize(memory, sizeof(type))
#define pushArray(memory, count, type) (type *)pushSize(memory, (count)*sizeof(type))

#define clearStruct(instance) clearSize(sizeof(instance), &(instance))
#define clearArray(instance, count, type) clearSize((count) * sizeof(type), (instance))

struct Memory
{
    Uint8 *base;
    size_t size;
    size_t used;
};

void* pushSize(Memory *memory, size_t size);

void clearSize(size_t size, void *ptr);

#endif
