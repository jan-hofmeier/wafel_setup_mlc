#ifndef SETUP_H
#define SETUP_H

#include <wafel/types.h>
#include <wafel/ios/svc.h>
#include <wafel/services/fsa.h>
#include <assert.h>

// This stuff was taken directly from Garys recovery_menu
typedef enum
{
    MCP_REGION_JAPAN = 0x01,
    MCP_REGION_USA = 0x02,
    MCP_REGION_EUROPE = 0x04,
    MCP_REGION_CHINA = 0x10,
    MCP_REGION_KOREA = 0x20,
    MCP_REGION_TAIWAN = 0x40,

    // Force MCPRegion to be 32-bit.
    MCP_REGION_U32	= 0xFFFFFFFF,
} MCPRegion;

typedef struct __attribute__((packed))
{
    MCPRegion product_area;
    uint16_t eeprom_version;
    uint8_t pad1[2];
    MCPRegion game_region;
    uint8_t unknown1[4];
    char ntsc_pal[5];

    //! 5ghz_country_code in xml
    char wifi_5ghz_country_code[4];

    //! 5ghz_country_code_revision in xml
    uint8_t wifi_5ghz_country_code_revision;

    char code_id[8];
    char serial_id[12];
    uint8_t unknown2[4];
    char model_number[16];
    uint32_t version;
} MCPSysProdSettings;
static_assert(sizeof(MCPSysProdSettings) == 0x46, "MCPSysProdSettings: different size than expected");

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
