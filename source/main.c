#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wafel/dynamic.h>
#include <wafel/ios_dynamic.h>
#include <wafel/utils.h>
#include "setup.h"
#include "ios/svc.h"
#include "ios/memory.h"


#define _ASM_PATCH(_addr, _str, _copy_fn) { \
    __asm__ volatile (           \
    ".globl pre_" #_addr "\n"    \
    ".globl post_" #_addr "\n"   \
    "b post_" #_addr "\n"        \
    "pre_" #_addr ":\n"          \
    ".arm\n"                     \
    _str "\n"                    \
     ".arm\n"                    \
    "post_" #_addr ":\n");       \
    extern void pre_##_addr();   \
    extern void post_##_addr();  \
    _copy_fn((uintptr_t)_addr, (uintptr_t)pre_##_addr, (u32)post_##_addr - (u32)pre_##_addr); \
}

static void _patch_copy_k(uintptr_t dst, uintptr_t src, size_t sz)
{
    memcpy((void*)ios_elf_vaddr_to_paddr(dst), (void*)src, sz);
}

#define ASM_PATCH_K(_addr, _str) _ASM_PATCH(_addr, _str, _patch_copy_k)

// This fn runs before everything else in kernel mode.
// It should be used to do extremely early patches
// (ie to BSP and kernel, which launches before MCP)
// It jumps to the real IOS kernel entry on exit.
__attribute__((target("arm")))
void kern_main()
{
    // Make sure relocs worked fine and mappings are good
    debug_printf("we in here setup mlc plugin kern %p\n", kern_main);

    debug_printf("init_linking symbol at: %08x\n", wafel_find_symbol("init_linking"));

    // Disable panic in kernel
    ASM_PATCH_K(0x08129ce0, "bx lr\n");

    // format MLC if needed
    ASM_PATCH_K(0x05027D24, ".thumb\nnop\nnop\n");

    debug_printf("setup patches applied\n");
}

// This fn runs before MCP's main thread, and can be used
// to perform late patches and spawn threads under MCP.
// It must return.
void mcp_main()
{
    // Make sure relocs worked fine and mappings are good
	debug_printf("we in setup mlc MCP %p\n", mcp_main);

    // Start up setup thread
    u8* setup_stack = (u8*)malloc_global(0x1000);
    int setup_threadhand = iosCreateThread(setup_main, NULL, (u32*)(setup_stack+0x1000), 0x1000, 0x78, 1);
    if (setup_threadhand >= 0) {
        iosStartThread(setup_threadhand);
    }

    debug_printf("done\n");
}