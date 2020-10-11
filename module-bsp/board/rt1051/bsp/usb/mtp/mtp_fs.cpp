#include <string.h>
#include <assert.h>
#include "FreeRTOS.h"
#include <vfs.hpp>

#include "ff_eMMC_user_disk.hpp"

extern "C" {
#   include "mtp_responder.h"
#   include "mtp_db.h"
}

#define LOG(...) LOG_INFO(__VA_ARGS__)

#define ROOT "/sys/user/music"

static mtp_storage_properties_t disk_properties =  {
    .type = MTP_STORAGE_FIXED_RAM,
    .fs_type = MTP_STORAGE_FILESYSTEM_FLAT,
    .access_caps = 0x0000,
    .capacity = 0,
    .description = "SD Card",
    .volume_id = "1234567890abcdef",
};

struct mtp_fs {
    struct mtp_db *db;
    FF_FindData_t find_data;
    FF_FILE *file;
};

typedef struct {
    uint32_t read;
    uint64_t capacity;
    uint64_t freespace;
} fs_data_t;

static int is_dot(const char *name) {
    bool rv = ((strlen(name) == 1) && (name[0] == '.')) ||
        ((strlen(name) == 2) && (name[0] == '.') && (name[1] == '.'));
    LOG("is_dot: %s: %s", name, rv ? "true" : "false");
    return rv;
}

static uint32_t count_files(FF_FindData_t *find_data)
{
    uint32_t count = 0;
    memset(find_data, 0, sizeof(FF_FindData_t));
    if (!ff_findfirst(ROOT, find_data)) {
        do {
            if (is_dot(find_data->pcFileName))
                continue;
           count++;
        } while (!ff_findnext(find_data));
    }
    return count;
}
static const char *abspath(const char *filename)
{
    static char abs[128];
    abs[0] = '/';
    strncpy(&abs[1], filename, 64);
    snprintf(abs, 128, "%s/%s", ROOT, filename);
    return abs;
}

static const mtp_storage_properties_t* get_disk_properties(void* arg)
{
    fs_data_t *data = (fs_data_t*)arg;
    vfs::FilesystemStats stats;

    stats = vfs.getFilesystemStats();
    // TODO: stats are for entire storage. If MTP is intended to expose
    // only one directory, these stats should be recalculated
    data->freespace = stats.freeMbytes*1024llu*1024llu;
    data->capacity = stats.totalMbytes*1024llu*1024llu;

    disk_properties.capacity = data->capacity;

    //LOG("Capacity: %llu B, free: %llu", data->capacity, data->freespace);

    return &disk_properties;
}

static uint64_t get_free_space(void *arg)
{
    // TODO: see get_disk_properties
    vfs::FilesystemStats stats;
    uint64_t size = 0;
    stats = vfs.getFilesystemStats();
    size = stats.freeMbytes*1024llu*1024llu;
    //LOG("Free space: %llu KB", size / 1024);
    return size;
}

static uint32_t fs_find_first(void *arg, uint32_t root, uint32_t *count)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    uint32_t handle;
    int result;
    // TODO: list dir is not a good choose, as it allocates
    // memory for all files from directory
    // auto list = vfs.listdir(ROOT);
    // it would be better to have iterator mapped to
    // filesystem.

    if (root != 0 && root != 0xFFFFFFFF)
        return 0;

    FF_FindData_t *find_data = &fs->find_data;
    *count = count_files(find_data);

    // empty directory
    if (*count == 0)
        return 0;

    LOG("found: %u files:", (unsigned int) *count);

    memset(find_data, 0, sizeof(FF_FindData_t));
    if (ff_findfirst(ROOT, find_data)) {
        LOG("ff_findfirst failed");
        return 0;
    }

    while (is_dot(find_data->pcFileName)) {
        LOG("skip: %s", find_data->pcFileName);
        if (ff_findnext(find_data)) {
            LOG("No files found");
            *count = 0;
            return 0;
        }
    }

    handle = mtp_db_add(fs->db, find_data->pcFileName);
    return handle;
}

static uint32_t fs_find_next(void* arg)
{
    uint32_t handle;
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    FF_FindData_t *find_data = &fs->find_data;

    while (!ff_findnext(find_data)) {
        if (is_dot(find_data->pcFileName))
            continue;
        handle = mtp_db_add(fs->db, find_data->pcFileName);
        return handle;
    }
    LOG("Done. No more files");
    return 0;
}

static uint16_t ext_to_format_code(const char *filename)
{
    if (strstr(".jpg", filename))
        return MTP_FORMAT_EXIF_JPEG;
    if (strstr(".txt", filename))
        return MTP_FORMAT_TEXT;
    if (strstr(".wav", filename))
        return MTP_FORMAT_WAV;
    return MTP_FORMAT_UNDEFINED;
}

static int fs_stat(void *arg, uint32_t handle, mtp_object_info_t *info)
{
    FF_Stat_t stat;
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    const char *filename = mtp_db_get(fs->db, handle);

    if (!filename) {
        // TODO: invalid handle
        return -1;
    }

    LOG("[%u]: Get info for: %s", (unsigned int)handle, filename);

    if (!ff_stat(abspath(filename), &stat)) {
        memset(info, 0, sizeof(mtp_object_info_t));
        info->storage_id = 0x00010001;
        info->created = 1580371617;
        info->modified = 1580371617;
        info->format_code = ext_to_format_code(filename);
        info->size = (uint64_t)stat.st_size;
        *(uint32_t*)(info->uuid) = handle;
        strncpy(info->filename, filename, 64);
        return 0;
    } else {
        LOG("[%u]: Stat error: %s", (unsigned int)handle, filename);
    }

    return -1;
}

static int fs_rename(void *arg, uint32_t handle, const char *new_name)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    int status;
    const char *filename = mtp_db_get(fs->db, handle);

    if (!filename)
        return -1;

    status = ff_rename(filename, new_name, pdFALSE);
    if (!status) {
        mtp_db_update(fs->db, handle, new_name);
        LOG("[%u]: Rename: %s -> %s", (unsigned int)handle, filename, new_name);
    }

    LOG("[%u]: Rename: %s -> %s FAILED", (unsigned int)handle, filename, new_name);
    return status;
}

static int fs_create(void *arg, const mtp_object_info_t *info, uint32_t *handle)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    uint32_t new_handle;

    if (fs->file) {
        LOG("Can't create new file. Some file is still open");
        return -1;
    }

    new_handle = mtp_db_add(fs->db, info->filename);
    if (!new_handle) {
        LOG("Map is full. Can't create: %s", info->filename);
        return -1;
    }

    while(!(fs->file = ff_fopen(abspath(info->filename), "w"))) {
        LOG("[]: freertos-fat error - ff_open(w) (create). Flush and wait");
        // TODO: FF_SDDiskFlush(fs->disk);
    }

    ff_fclose(fs->file);
    fs->file = NULL;
    *handle = new_handle;
    LOG("[%u]: Created: %s", (unsigned int)*handle, info->filename);
    return 0;
}

static int fs_remove(void *arg, uint32_t handle)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    const char *filename = mtp_db_get(fs->db, handle);
    if (!filename) {
        return -1;
    }

    while(!ff_remove(abspath(filename))) {
        //TODO: FF_SDDiskFlush(fs->disk);
    }

    LOG("[%u]: Removed: %s", (unsigned int)handle, filename);
    mtp_db_del(fs->db, handle);
    return 0;
}

static int fs_open(void *arg, uint32_t handle, const char *mode)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    const char *filename = mtp_db_get(fs->db, handle);
    if (!filename) {
        return -1;
    }

    if (fs->file) {
        LOG("Can't open %s. Some file is still open", filename);
        return -1;
    }

    while(!(fs->file = ff_fopen(abspath(filename), mode))) {
        LOG("[%u]: Fail to open: %s [%s]. Flush and wait", (unsigned int)handle, filename, mode);
        // TODO: FF_SDDiskFlush(fs->disk);
    }

    LOG("[%u]: Opened: %s [%s]", (unsigned int)handle, filename, mode);

    return 0;
}

static int fs_read(void *arg, void *buffer, size_t count)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    size_t read;

    if (!fs->file)
        return -1;

    while ((read = ff_fread(buffer, 1, count, fs->file)) <= 0) {
        LOG("[]: Fail to read");
        //TODO: FF_SDDiskFlush(fs->disk);
    }
    return read;
}

static int fs_write(void *arg, void *buffer, size_t count)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    if (!fs->file)
        return -1;

    while(ff_fwrite(buffer, count, 1, fs->file) != 1) {
        LOG("[]: Fail to write");
        //TODO: FF_SDDiskFlush(fs->disk);
    }
    return 0;
}

static void fs_close(void *arg)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    if (fs->file) {
        ff_fclose(fs->file);
        LOG("[]: Closed");
        fs->file = NULL;
    }
}

extern "C" const struct mtp_storage_api simple_fs_api =
{
    .get_properties = get_disk_properties,
    .find_first = fs_find_first,
    .find_next = fs_find_next,
    .get_free_space = get_free_space,
    .stat = fs_stat,
    .rename = fs_rename,
    .create = fs_create,
    .remove = fs_remove,
    .open = fs_open,
    .read = fs_read,
    .write = fs_write,
    .close = fs_close
};

extern "C" struct mtp_fs* mtp_fs_alloc(void *disk)
{
    struct mtp_fs* fs = (struct mtp_fs*)pvPortMalloc(sizeof(struct mtp_fs));
    if (fs) {
        memset(fs, 0, sizeof(struct mtp_fs));
        if (!(fs->db = mtp_db_alloc())) {
            vPortFree(fs);
            return NULL;
        }
    }
    return fs;
}

