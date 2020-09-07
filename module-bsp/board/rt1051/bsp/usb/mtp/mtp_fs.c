#include <string.h>
#include <assert.h>
#include "mtp_responder.h"
#include "mtp_db.h"
#include "FreeRTOS.h"

#define LOG(...)

static mtp_storage_properties_t disk_properties =  {
    .type = MTP_STORAGE_FIXED_RAM,
    .fs_type = MTP_STORAGE_FILESYSTEM_FLAT,
    .access_caps = 0x0000,
    .volume_id = "1234567890abcdef",
    .description = "SD Card",
};

struct mtp_fs {
    struct mtp_db *db;
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
    uint32_t capacity;

    LOG("Calculating free spece and capacity...\n");

    data->freespace = 0;
    data->capacity = 0;

    disk_properties.capacity = data->capacity;

    LOG("Capacity: %llu B, free: %llu\n", data->capacity, data->freespace);

    return &disk_properties;
}

uint64_t get_free_space(void *arg)
{
    uint64_t size = 0;
    LOG("Free space: %llu KB\n", size / 1024);
    return size;
}

uint32_t fs_find_first(void *arg, uint32_t root, uint32_t *count)
{
    uint32_t handle;
    struct mtp_fs *fs = (struct mtp_fs*)arg;

    if (root != 0 && root != 0xFFFFFFFF)
        return 0;

    return 0;
}

uint32_t fs_find_next(void* arg)
{
    return 0;
}

uint16_t ext_to_format_code(const char *filename)
{
    if (strstr(".jpg", filename))
        return MTP_FORMAT_EXIF_JPEG;
    if (strstr(".txt", filename))
        return MTP_FORMAT_TEXT;
    if (strstr(".wav", filename))
        return MTP_FORMAT_WAV;
    return MTP_FORMAT_UNDEFINED;
}

int fs_stat(void *arg, uint32_t handle, mtp_object_info_t *info)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

int fs_rename(void *arg, uint32_t handle, const char *new_name)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

int fs_create(void *arg, const mtp_object_info_t *info, uint32_t *handle)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

int fs_open(void *arg, uint32_t handle, const char *mode)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

int fs_read(void *arg, void *buffer, size_t count)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

int fs_write(void *arg, void *buffer, size_t count)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
    return -1;
}

int fs_remove(void *arg, uint32_t handle)
{
    return -1;
}

void fs_close(void *arg)
{
    struct mtp_fs *fs = (struct mtp_fs*)arg;
}

const struct mtp_storage_api simple_fs_api =
{
    .get_properties = get_disk_properties,
    .get_free_space = get_free_space,
    .find_first = fs_find_first,
    .find_next = fs_find_next,
    .stat = fs_stat,
    .rename = fs_rename,
    .create = fs_create,
    .remove = fs_remove,
    .open = fs_open,
    .read = fs_read,
    .write = fs_write,
    .close = fs_close
};

struct mtp_fs* mtp_fs_alloc(void *disk)
{
    struct mtp_fs* fs = pvPortMalloc(sizeof(struct mtp_fs));
    if (fs) {
        memset(fs, 0, sizeof(struct mtp_fs));
        if (!(fs->db = mtp_db_alloc())) {
            vPortFree(fs);
            return NULL;
        }
    }
    return fs;
}

