#ifndef _THREAD_H
#define _THREAD_H

#include "wafel/types.h"

int usleep(u32 amt);

static inline int msleep(u32 amt)
{
    return usleep(amt*1000);
}

#endif // _THREAD_H
