#include "Memory.hpp"

void* pushSize(Memory *memory, size_t size) {
    SDL_assert((memory->used + size) <= memory->size);
    void *result = memory->base + memory->used;
    memory->used += size;
    
    return result;
}

void clearSize(size_t size, void *ptr)
{
    Uint8 *byte = (Uint8 *)ptr;
    while(size--)
    {
        *byte++ = 0;
    }
}