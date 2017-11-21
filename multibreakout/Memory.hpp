#ifndef Memory_hpp
#define Memory_hpp

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_assert.h>

#define PushStruct(memory, type) (type *)pushSize(memory, sizeof(type))
#define PushArray(memory, count, type) (type *)pushSize(memory, (count)*sizeof(type))

struct Memory {
    Uint8 *base;
    size_t size;
    size_t used;
};

void* pushSize(Memory *memory, size_t size);

#endif
