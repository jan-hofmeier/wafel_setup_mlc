#ifndef FSA_H
#define FSA_H


typedef struct
{
    u32 flags;      // 0x0
    u32 permissions;// 0x4
	u32 unk1[0x2];  // 0x8
	u32 size;       // 0x10 size in bytes
	u32 physsize;   // 0x14 physical size on disk in bytes
	u32 unk2[0x13];
}fileStat_s;

typedef struct
{
	fileStat_s dirStat;
	char name[0x100];
}directoryEntry_s;

typedef enum
{
    MEDIA_READY = 0,
    MEDIA_NOT_PRESENT = 1,
    MEDIA_INVALID = 2,
    MEDIA_DIRTY = 3,
    MEDIA_FATAL = 4
} media_states;

typedef struct
{
    u32 flags;
    u32 media_state;
    s32 device_index;
    u32 block_size;
    u32 logical_block_size;
    u32 read_align;
    u32 write_align;
    char dev_type[8]; // sdcard, slc, mlc, etc
    char fs_type[8]; // isfs, wfs, fat, etc
    char vol_label[0x80];
    char vol_id[0x80];
    char dev_node[0x10];
    char mount_path[0x80];
} fsa_volume_info;

#define FSA_MOUNTFLAGS_BINDMOUNT (1 << 0)
#define FSA_MOUNTFLAGS_GLOBAL (1 << 1)

int MCP_InstallGetInfo(int fd, const char* path);
int MCP_Install(int fd, const char* path);
int MCP_InstallTarget(int fd, int target);

int FSA_Open();

int FSA_Mount(int fd, const char* device_path, const char* volume_path, u32 flags, char* arg_string, int arg_string_len);
int FSA_Unmount(int fd, const char* path, u32 flags);
int FSA_FlushVolume(int fd, const char* volume);

int FSA_GetDeviceInfo(int fd, const char* device_path, int type, u32* out_data);
int FSA_GetVolumeInfo(int fd, const char* volume_path, int type, fsa_volume_info* out_data);

int FSA_MakeDir(int fd, const char* path, u32 flags);
int FSA_OpenDir(int fd, const char* path, int* outHandle);
int FSA_ReadDir(int fd, int handle, directoryEntry_s* out_data);
int FSA_CloseDir(int fd, int handle);

int FSA_MakeQuota(int fd, const char* path, u32 mode, u64 size);

int FSA_OpenFile(int fd, const char* path, const char* mode, int* outHandle);
int FSA_ReadFile(int fd, void* data, u32 size, u32 cnt, int fileHandle, u32 flags);
int FSA_WriteFile(int fd, void* data, u32 size, u32 cnt, int fileHandle, u32 flags);
int FSA_StatFile(int fd, int handle, fileStat_s* out_data);
int FSA_CloseFile(int fd, int fileHandle);

int FSA_ChangeMode(int fd, const char* path, int mode);

int FSA_Format(int fd, const char* device, const char* fs_format, int flags, u64 what);

int FSA_RawOpen(int fd, const char* device_path, int* outHandle);
int FSA_RawRead(int fd, void* data, u32 size_bytes, u32 cnt, u64 sector_offset, int device_handle);
int FSA_RawWrite(int fd, void* data, u32 size_bytes, u32 cnt, u64 sector_offset, int device_handle);
int FSA_RawClose(int fd, int device_handle);

#endif
