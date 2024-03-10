#ifndef SETUP_H
#define SETUP_H

#include <wafel/types.h>
#include <wafel/services/fsa.h>

static void* allocIobuf(size_t size)
{
    void* ptr = iosAlloc(0xCAFF, size);

    memset(ptr, 0x00, size);

    return ptr;
}

static void freeIobuf(void* ptr)
{
	iosFree(0xCAFF, ptr);
}

u32 setup_main(void* arg);

#endif
