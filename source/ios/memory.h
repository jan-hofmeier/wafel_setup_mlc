#ifndef MEMORY_H
#define MEMORY_H

#include "wafel/types.h"

void* malloc_global(u32 size);
void free_global(void* mem);

#endif
