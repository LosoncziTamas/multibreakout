#include "multibreakout_memory.h"

void initializeMemoryPartition(MemoryPartition *memory, size_t size, void *base)
{
    memory->size = size;
    memory->base = (Uint8 *)base;
    memory->used = 0;
}

void* pushSize(MemoryPartition *memory, size_t size) 
{
    SDL_assert((memory->used + size) <= memory->size);

    void *result = memory->base + memory->used;
    memory->used += size;
    
    return result;
}

void clearSize(MemoryPartition *memory, size_t size, void *ptr)
{
    memory->used -= size;

    Uint8 *byte = (Uint8 *)ptr;
    while(size--)
    {
        *byte++ = 0;
    }
}