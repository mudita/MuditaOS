#include <string.h>
#include <assert.h>
#include "FreeRTOS.h"
#include <vfs.hpp>

extern "C" {
#   include "mtp_responder.h"
#   include "mtp_db.h"
}

#define LOG(...)

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
    std::vector<vfs::DirectoryEntry> dirList;
    std::vector<vfs::DirectoryEntry>::iterator dirEntry;
};

typedef struct {
    uint32_t read;
    uint64_t capacity;
    uint64_t freespace;
} fs_data_t;

static int is_dot(const char *name) {
    return (strlen(name) == 1) && (name[0] == '.');
}

static const char *abspath(const char *filename)
{
    static char abs[65];
    abs[0] = '/';
    strncpy(&abs[1], filename, 64);
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

    LOG("Capacity: %llu B, free: %llu\n", data->capacity, data->freespace);

    return &disk_properties;
}

static uint64_t get_free_space(void *arg)
{
    // TODO: see get_disk_properties
    vfs::FilesystemStats stats;
    uint64_t size = 0;
    stats = vfs.getFilesystemStats();
    size = stats.freeMbytes*1024llu*1024llu;
    LOG("Free space: %llu KB\n", size / 1024);
    return size;
}

static uint32_t fs_find_first(void *arg, uint32_t root, uint32_t *count)
{
    uint32_t handle;
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    vfs::DirectoryEntry entry;

    if (root != 0 && root != 0xFFFFFFFF)
        return 0;

    // TODO: assume, every entry is a file. Filter out directories
    fs->dirList = vfs.listdir("/");
    *count = fs->dirList.size();
    fs->dirEntry = fs->dirList.begin();
    entry = *fs->dirEntry++;
    handle = mtp_db_add(fs->db, entry.fileName.c_str());
    return handle;
}

static uint32_t fs_find_next(void* arg)
{
    uint32_t handle = 0;
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    vfs::DirectoryEntry entry;

    if (fs->dirEntry != fs->dirList.end()) {
        entry = *fs->dirEntry++;
        handle = mtp_db_add(fs->db, entry.fileName.c_str());
    }
    return handle;
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
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    const char *filename = mtp_db_get(fs->db, handle);

    if (!filename) {
        // TODO: invalid handle
        return -1;
    }
    LOG("[%u]: Get info for: %s\n", handle, filename);
    if (1) { /* !ff_stat(abspath(filename), &stat) */
        memset(info, 0, sizeof(mtp_object_info_t));
        info->storage_id = 0x00010001;
        info->created = 1580371617;
        info->modified = 1580371617;
        info->format_code = ext_to_format_code(filename);
        info->size = (uint64_t) 100; /* stat.st_size; */
        *(uint32_t*)(info->uuid) = handle;
        strncpy(info->filename, filename, 64);
        return 0;
    } else {
        LOG("[%u]: Stat error: %s\n", handle, filename);
    }
    return -1;
}

static int fs_rename(void *arg, uint32_t handle, const char *new_name)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

static int fs_create(void *arg, const mtp_object_info_t *info, uint32_t *handle)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

static int fs_open(void *arg, uint32_t handle, const char *mode)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

static int fs_read(void *arg, void *buffer, size_t count)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

static int fs_write(void *arg, void *buffer, size_t count)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

static int fs_remove(void *arg, uint32_t handle)
{
    return -1;
}

static void fs_close(void *arg)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
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

