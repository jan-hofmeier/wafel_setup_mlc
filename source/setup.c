#include <stdio.h>

#include <wafel/utils.h>

#include "setup.h"
#include "fsa.h"
#include "sci.h"
#include "ios/thread.h"
#include "ios/svc.h"

static const char *folders_to_create[] = { "/vol/storage_mlc01/usr", "/vol/storage_mlc01/usr/boss", "/vol/storage_mlc01/usr/save", "/vol/storage_mlc01/usr/save/00050010",
                     "/vol/storage_mlc01/usr/packages", "/vol/storage_mlc01/usr/tmp", NULL };

static void mount_sd(int fd, const char* path)
{
    // Mount sd to /vol/sdcard
    int ret = FSA_Mount(fd, "/dev/sdcard01", path, 0, NULL, 0);;
    int i = 1;
    while(ret < 0)
    {
        ret = FSA_Mount(fd, "/dev/sdcard01", path, 0, NULL, 0);
        debug_printf("Mount SD attempt %d, %X\n", i++, ret);
        usleep(1000);

        if (ret == 0xFFFCFFEA || ret == 0xFFFCFFE6 || i >= 0x100) break;
    }
    debug_printf("Mounted SD...\n");
}

static void wait_mlc_ready(int fd){
    int i = 1;
    int ret;
    int dir = 0;
    do
    {
        usleep(1000);
        ret = FSA_OpenDir(fd, "/vol/storage_mlc01/", &dir);
        debug_printf("MLC open attempt %d %X\n", i++, ret);
    }while(ret < 0);
    FSA_CloseDir(fd, dir);
    debug_printf("MLC ready!\n");
}

static inline void flush_mlc(int fd){
    int ret = FSA_FlushVolume(fd, "/vol/storage_mlc01");
    debug_printf("Flush MLC returned %X\n", ret);
}

static inline void flush_slc(int fd){
    int ret = FSA_FlushVolume(fd, "/vol/system");
    debug_printf("Flush SLC returned %X\n", ret);
}


static void install_title(int mcp_handle, const char *install_dir){
        int ret = MCP_InstallTarget(mcp_handle, 0);
        debug_printf("installtarget : %08x", ret);

        ret = MCP_Install(mcp_handle, install_dir);
        debug_printf("install : %08x", ret);
}

static void install_all_titles(int fd, const char *directory){
    int dir = 0;
    int ret = FSA_OpenDir(fd, directory, &dir);
    if(ret)
    {
        debug_printf("Dir %s open failed: %X Aborting...\n", directory, ret);
        return;
    }

    int mcp_handle = iosOpen("/dev/mcp", 0);
    if(mcp_handle <= 0)
    {
        debug_printf("Failed to open MCP : -%08X\n", mcp_handle);
        FSA_CloseDir(fd, dir);
        return;
    }

    directoryEntry_s *dir_entry = iosAlloc(0x00001, sizeof(directoryEntry_s));
    if(dir_entry == NULL)
    {
        debug_printf("Dir entry alloc failed, Aborting...\n");
        iosClose(mcp_handle);
        FSA_CloseDir(fd, dir);
        return;
    }
    debug_printf("allocated direntry\n");

    char *install_dir = iosAlloc(0x00001,0x100);
    if(install_dir == NULL)
    {
        debug_printf("Failed to allocate copy stringname!\n");
        iosFree(0x00001, dir_entry);
        iosClose(mcp_handle);
        FSA_CloseDir(fd, dir);
        return;
    }

    while(!FSA_ReadDir(fd, dir, dir_entry))
    {
        if(dir_entry->dirStat.flags & 0x80000000)
        {
            // get new dir str
            snprintf(install_dir, 0x100, "%s/%s", directory, dir_entry->name);

            // test if installable
            ret = MCP_InstallGetInfo(mcp_handle, install_dir);
            debug_printf("installinfo %s: %08x", dir_entry->name, ret);
            if(!ret)
                install_title(mcp_handle, install_dir);
        }
    }
    iosFree(0x00001, install_dir);
    iosFree(0x00001, dir_entry);
    iosClose(mcp_handle);
    FSA_CloseDir(fd, dir);
}

u32 setup_main(void* arg){
    int fd = -1;
    int i = 1;
    while(fd < 0)
    {
        usleep(1000);
        fd = FSA_Open();
        debug_printf("FSA open attempt %d %X\n", i++, fd);
    }

    wait_mlc_ready(fd);

    int ret = FSA_MakeQuota(fd, "/vol/storage_mlc01/sys", 0, 3221225472);
    debug_printf("MakeQuota /vol/storage_mlc01/sys -%X\n", -ret);

    for(i = 0; folders_to_create[i]; i++){
        int ret = FSA_MakeDir(fd, folders_to_create[i], 0);
        debug_printf("Create %s -%X\n", folders_to_create[i] -ret);
    }

    flush_mlc(fd);
    
    mount_sd(fd, "/vol/sdcard/");
    install_all_titles(fd, "/vol/sdcard/wafel_install");
    flush_mlc(fd);

    ret = SCISetInitialLaunch(255);
    debug_printf("Set InitalLaunch returned %X\n", ret);
    flush_slc(fd);

    iosClose(fd);

    debug_printf("FINISHED!");

    return 0;
}
