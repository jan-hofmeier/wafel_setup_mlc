#include <string.h>
#include <wafel/ios/svc.h>
#include <wafel/utils.h>

#include "bsp.h"

#define CROSS_PROCESS_HEAP_ID 0xcaff

static void* allocIobuf()
{
    void* ptr = iosAlloc(CROSS_PROCESS_HEAP_ID, 0x260);

    memset(ptr, 0, 0x260);

    return ptr;
}

static void freeIobuf(void* ptr)
{
    iosFree(CROSS_PROCESS_HEAP_ID, ptr);
}

int bspWrite(const char* entity, uint32_t instance, const char* attribute, uint32_t size, const void* buffer)
{
    debug_printf("bspWrite begin\n");
    
    int handle = iosOpen("/dev/bsp", 0);
    if (handle < 0) {
        return handle;
    }

    uint32_t* buf = (uint32_t*) allocIobuf();
    strncpy((char*) buf, entity, 0x20);
    buf[8] = instance;
    strncpy((char*) (buf + 9), attribute, 0x20);
    buf[17] = size;
    memcpy((char*) (buf + 18), buffer, size);
    int res = iosIoctl(handle, 6, buf, 0x48 + size, NULL, 0);
    freeIobuf(buf);
    iosClose(handle);

    debug_printf("bspWrite done\n");
    return res;
}
