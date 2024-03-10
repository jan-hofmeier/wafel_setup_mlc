#include "sysprod.h"
#include "setup.h"

// Both MCP_GetSysProdSettings and MCP_SetSysProdSettings are from recovery_menu and were modified for use with stroopwafel.
int MCP_GetSysProdSettings(int fd, MCPSysProdSettings* out_sysProdSettings)
{
    uint8_t* buf = allocIobuf(sizeof(iovec_s) + sizeof(*out_sysProdSettings));

    iovec_s* vecs = (iovec_s*)buf;
    vecs[0].ptr = buf + sizeof(iovec_s);
    vecs[0].len = sizeof(*out_sysProdSettings);

    int res = iosIoctlv(fd, 0x40, 0, 1, vecs);
    if (res >= 0) {
        memcpy(out_sysProdSettings, vecs[0].ptr, sizeof(*out_sysProdSettings));
    }

    freeIobuf(buf);

    return res;
}

int MCP_SetSysProdSettings(int fd, const MCPSysProdSettings* sysProdSettings)
{
    uint8_t* buf = allocIobuf(sizeof(iovec_s) + sizeof(*sysProdSettings));
    memcpy(&buf[sizeof(iovec_s)], sysProdSettings, sizeof(*sysProdSettings));

    iovec_s* vecs = (iovec_s*)buf;
    vecs[0].ptr = buf + sizeof(iovec_s);
    vecs[0].len = sizeof(*sysProdSettings);

    int res = iosIoctlv(fd, 0x41, 1, 0, vecs);
    freeIobuf(buf);

    return res;
}
