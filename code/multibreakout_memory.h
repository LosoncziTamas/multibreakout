#ifndef MULTIBREAKOUT_MEMORY_H
#define MULTIBREAKOUT_MEMORY_H

#include <SDL2/SDL.h>

struct MemoryPartition
{
    Uint8 *base;
    size_t size;
    size_t used;
};

#define pushStruct(memory, type) (type *)pushSize(memory, sizeof(type))
#define pushArray(memory, count, type) (type *)pushSize(memory, (count)*sizeof(type))

#define clearStruct(instance) clearSize(sizeof(instance), &(instance))
#define clearArray(memory, instance, count, type) clearSize(memory, (count) * sizeof(type), (instance))

void initializeMemoryPartition(MemoryPartition *memory, size_t size, void *base);
void* pushSize(MemoryPartition *memory, size_t size);
void clearSize(MemoryPartition *memory, size_t size, void *ptr);

#endif