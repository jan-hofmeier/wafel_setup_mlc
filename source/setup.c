#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <wafel/utils.h>
#include <wafel/services/fsa.h>
#include <wafel/ios/thread.h>
#include <wafel/ios/svc.h>

#include "setup.h"
#include "sci.h"
#include "led.h"
#include "sysprod.h"

#define MAX_LOG_LINE_LENGHT 512
#define CROSS_PROCESS_HEAP_ID 0xcaff

char *folders_to_create[] = { "/vol/storage_mlc01/usr", "/vol/storage_mlc01/usr/boss", "/vol/storage_mlc01/usr/save", "/vol/storage_mlc01/usr/save/00050010",
                     "/vol/storage_mlc01/usr/packages", "/vol/storage_mlc01/usr/tmp", NULL };


static int log_printf(int fsaHandle, int logHandle, const char* fmt, ...){
    if(!logHandle) {
        return -1;
    }

    void *dataBuffer = iosAllocAligned(CROSS_PROCESS_HEAP_ID, MAX_LOG_LINE_LENGHT, 0x40);
    if(!dataBuffer){
        debug_printf("Error allocating log buffer\n");
        return -1;
    }

    va_list args;
    va_start(args, fmt);
    int res = vsnprintf(dataBuffer, MAX_LOG_LINE_LENGHT, fmt, args);
    va_end(args);

    if (res < 0) {
      iosFree(CROSS_PROCESS_HEAP_ID, dataBuffer);
      return -1;
    }

    res = FSA_WriteFile(fsaHandle, dataBuffer, res, 1, logHandle, 0);
    iosFree(CROSS_PROCESS_HEAP_ID, dataBuffer);
    if(res == 1)
        res = FSA_FlushFile(fsaHandle, logHandle);
    if(res<0){
      debug_printf("Error writing log: -%08X\n", -res);
      return -1;
    }

    return 0;
}

void mount_sd(int fd, char* path)
{
    // Mount sd to /vol/sdcard
    int ret = FSA_Mount(fd, "/dev/sdcard01", path, 0, NULL, 0);;
    int i = 1;
    while(ret < 0)
    {
        ret = FSA_Mount(fd, "/dev/sdcard01", path, 0, NULL, 0);
        debug_printf("Mount SD attempt %d, %X\n", i++, ret);
        usleep(1000);

        //if (ret == 0xFFFCFFEA || ret == 0xFFFCFFE6 || i >= 0x100) break;
    }
    debug_printf("Mounted SD...\n");
}

void wait_mlc_ready(int fd){
    int i = 1;
    int ret;
    int dir = 0;
    do
    {
        usleep(1000000);
        ret = FSA_OpenDir(fd, "/vol/storage_mlc01/", &dir);
        debug_printf("MLC open attempt %d %X\n", i++, ret);
    }while(ret < 0);
    FSA_CloseDir(fd, dir);
    debug_printf("MLC ready!\n");
}

int flush_mlc(int fsaHandle){
    int ret = FSA_FlushVolume(fsaHandle, "/vol/storage_mlc01");
    debug_printf("Flush MLC returned %X\n", ret);
    return ret;
}

int flush_slc(int fsaHandle){
    int ret = FSA_FlushVolume(fsaHandle, "/vol/system");
    debug_printf("Flush SLC returned %X\n", ret);
    return ret;
}


int install_title(int mcp_handle, char *install_dir){
        int ret = MCP_InstallTarget(mcp_handle, 0);
        debug_printf("installtarget : %08x\n", ret);

        ret = MCP_Install(mcp_handle, install_dir);
        debug_printf("install : %08x\n", ret);
        return ret;
}

int error_state = 0;

void update_error_state(int value, int level){
    if(value){
        if(level > error_state){
            if(level = 1) {
                SetNotificationLED(NOTIF_LED_ORANGE | NOTIF_LED_ORANGE_BLINKING);
                debug_printf("WARNING WARNING WARNING WARNING WARNING WARNING\n");
            }else{
                SetNotificationLED(NOTIF_LED_RED | NOTIF_LED_RED_BLINKING);
                debug_printf("ERROR ERROR ERROR ERROR ERROR ERROR ERROR\n");
            }
            error_state = level;
        }
    }
}


void install_all_titles(int fd, char *directory, int logHandle){
    int dir = 0;
    int ret = FSA_OpenDir(fd, directory, &dir);
    log_printf(fd,logHandle, "OpenDir %s: %X\n", directory, ret);
    if(ret)
    {
        update_error_state(1, 2);
        debug_printf("Dir %s open failed: %X Aborting...\n", directory, ret);
        return;
    }

    int mcp_handle = iosOpen("/dev/mcp", 0);
    log_printf(fd,logHandle, "OpenMCP %s: %X\n", directory, ret);
    if(mcp_handle <= 0)
    {
        update_error_state(1, 2);
        debug_printf("Failed to open MCP : -%08X\n", mcp_handle);
        FSA_CloseDir(fd, dir);
        return;
    }

    directoryEntry_s *dir_entry = iosAlloc(0x00001, sizeof(directoryEntry_s));
    if(dir_entry == NULL)
    {
        update_error_state(1, 2);
        debug_printf("Dir entry alloc failed, Aborting...\n");
        iosClose(mcp_handle);
        FSA_CloseDir(fd, dir);
        return;
    }
    debug_printf("allocated direntry\n");

    char *install_dir = iosAlloc(0x00001,0x100);
    if(install_dir == NULL)
    {
        update_error_state(1, 2);
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
            debug_printf("installinfo %s: %08x\n", dir_entry->name, ret);
            update_error_state(ret, 1);
            log_printf(fd,logHandle, "InstallInfo %s: %08x\n", dir_entry->name, ret);
            if(!ret){
                ret = install_title(mcp_handle, install_dir);
                update_error_state(ret, 2);
                log_printf(fd, logHandle, "Install %s: %08x\n", dir_entry->name, ret);
            }
        }
    }
    iosFree(0x00001, install_dir);
    iosFree(0x00001, dir_entry);
    iosClose(mcp_handle);
    FSA_CloseDir(fd, dir);
}

void fix_region(int fsaHandle, int logHandle){

    uint64_t coldbootTitle = *(vu64*)(0x050b817c);

    debug_printf("Current coldboot title:    %08lx-%08lx",
            (uint32_t)(coldbootTitle >> 32), (uint32_t)(coldbootTitle & 0xFFFFFFFFU));

    if(coldbootTitle & 0xFFFFFFFFFFFFF0FF != 0005001010040000UL){
        update_error_state(0, 1);
        debug_printf("Unknown coldboot title: %llX\n", coldbootTitle);
        log_printf(fsaHandle, logHandle, "Unknown coldboot title: %llX\n", coldbootTitle);
        )
    }

    int region_idx = (coldbootTitle>>8)&0xF;

    if(region_idx>=6){
        update_error_state(0, 1);
        debug_printf("Unknown coldboot title region: %llX\n", coldbootTitle);
        log_printf(fsaHandle, logHandle, "Unknown coldboot title region: %llX\n", coldbootTitle);
    }

    int coldbootRegion = 1<<region_idx;

    debug_printf("Colboot Title Region: 0x%X\n", coldbootRegion);

    // Massive props to Gary
    int mcp_handle = iosOpen("/dev/mcp", 0);

    MCPSysProdSettings sysProdSettings;
    int ret = MCP_GetSysProdSettings(mcp_handle, &sysProdSettings);
    debug_printf("MCP_GetSysProdSettings: %X\n", ret);
    if (ret != 0) {
      // handle failure to not corrupt
      update_error_state(0, 1);
      debug_printf("MCP_GetSysProdSettings failed: %X. Skipping setting sys_prod values.\n", ret);
      log_printf(fsaHandle, logHandle, "MCP_GetSysProdSettings failed: %X. Skipping setting sys_prod values.\n", ret);
      return;
    }

    if(sysProdSettings.product_area == coldbootRegion && 
        sysProdSettings.game_region == sysProdSettings.product_area)
        return; //Region already matches

    sysProdSettings.game_region = sysProdSettings.product_area = coldbootRegion;
    ret = MCP_SetSysProdSettings(mcp_handle, &sysProdSettings);
    debug_printf("Set Region to %X: %X\n", sysProdSettings.game_region, ret);
    log_printf(fsaHandle, logHandle, "Set region to %X:, %X\n", sysProdSettings.game_region, ret);
}

u32 setup_main(void* arg){

    bool warning = 0;
    bool error = 0;

    debug_printf("START MLC SETUP\n");

    int fsaHandle = -1;
    int i = 1;
    while(fsaHandle < 0)
    {
        usleep(1000);
        fsaHandle = FSA_Open();
        debug_printf("FSA open attempt %d %X\n", i++, fsaHandle);
    }

    wait_mlc_ready(fsaHandle);

    // give system time to create folders and init
    usleep(5000000);

    if(!error_state)
        SetNotificationLED(NOTIF_LED_BLUE | NOTIF_LED_BLUE_BLINKING);
    
    mount_sd(fsaHandle, "/vol/sdcard/");

    int logHandle = 0;
    int ret = FSA_OpenFile(fsaHandle, "/vol/sdcard/wafel_setup_mlc.log", "w", &logHandle);
    debug_printf("Open logfile -%X\n", -ret);
    update_error_state(ret, 1);

    install_all_titles(fsaHandle, "/vol/sdcard/wafel_install", logHandle);
    int flush_ret = flush_mlc(fsaHandle);
    update_error_state(flush_ret, 2);
    log_printf(fsaHandle, logHandle, "Flush MLC: %X\n", flush_ret);

    fix_region(fsaHandle, logHandle);

    ret = SCISetInitialLaunch(0);
    debug_printf("Set InitalLaunch returned %X\n", ret);
    update_error_state(ret<0, 2);
    log_printf(fsaHandle, logHandle, "SetInitialLaunch 0: %X\n", ret);
    ret = flush_slc(fsaHandle);
    update_error_state(ret, 2);
    log_printf(fsaHandle, logHandle, "Flush SLC: %X\n", ret);

    ret = FSA_CloseFile(fsaHandle, logHandle);
    debug_printf("Close logfile returned -%X\n", -ret);
    ret = FSA_Unmount(fsaHandle, "/vol/sdcard", 0);
    debug_printf("Unmount SD -%X\n", -ret);

    debug_printf("Re-enabling Power Transitions\n");
    *(vu32*)0x0501f2bc = 1;

    iosClose(fsaHandle);

    if(!error_state){
        SetNotificationLED(NOTIF_LED_BLUE);
        debug_printf("MLC SETUP FINISHED!\n");
    }else if(error_state == 1) {
        SetNotificationLED(NOTIF_LED_ORANGE);
        debug_printf("MLC SETUP FINISHED with WARNING!\n");
    }else{
        debug_printf("MLC SETUP FINISHED with ERROR!\n");
        // Keep red blinking to differentiate power off
        //SetNotificationLED(NOTIF_LED_RED);
    }



    return 0;
}
