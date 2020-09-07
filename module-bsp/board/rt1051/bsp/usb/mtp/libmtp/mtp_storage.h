#ifndef _MTP_STORAGE_H
#define _MTP_STORAGE_H

#include <stdint.h>
#include <time.h>

typedef struct mtp_object_info {
    uint32_t storage_id;
    time_t created;
    time_t modified;
    uint16_t association_type;
    uint16_t format_code;
    uint16_t protection;
    uint32_t association_desc;
    uint32_t parent;
    uint64_t size;
    uint8_t uuid[16];
    char filename[64];
} mtp_object_info_t;

typedef struct mtp_storage_props {
    uint16_t type;
    uint16_t fs_type;
    uint16_t access_caps;
    uint64_t capacity;
    const char *description;
    const char *volume_id;
} mtp_storage_properties_t;

typedef struct mtp_storage_api {
    const mtp_storage_properties_t* (*get_properties)(void* arg);
    uint32_t (*find_first)(void *arg, uint32_t parent, uint32_t *count);
    uint32_t (*find_next)(void* arg);
    uint64_t (*get_free_space)(void* arg);
    int (*stat)(void *arg, uint32_t handle, mtp_object_info_t *info);
    int (*rename)(void *arg, uint32_t handle, const char *new_name);
    int (*create)(void *arg, const mtp_object_info_t *info, uint32_t *handle);
    int (*remove)(void *arg, uint32_t handle);
    int (*open)(void *arg, uint32_t handle, const char *mode);
    int (*read)(void *arg, void *buffer, size_t count);
    int (*write)(void *arg,void *buffer, size_t count);
    void (*close)(void *arg);
} mtp_storage_api_t;

typedef struct {
    uint32_t id;
    const struct mtp_storage_api *api;
    void *api_arg;
} mtp_storage_t;


uint32_t serialize_storage_list(mtp_storage_t *storage, uint32_t parent, uint8_t *data);
uint32_t serialize_storage_info(mtp_storage_t *storage, uint8_t *data);
uint32_t serialize_storage_ids(mtp_storage_t *storage, int count, uint8_t *data);
uint32_t serialize_object_info(mtp_object_info_t* info, uint8_t *data);
uint32_t serialize_object_props_supported(uint8_t *data);
uint32_t serialize_object_prop_desc(uint16_t prop_code, uint8_t *data);
uint32_t serialize_object_prop_value(uint16_t prop_code, mtp_object_info_t *info, uint8_t *data);
int deserialize_object_prop_value(uint16_t prop_code, const uint8_t *data, void *value);

int deserialize_object_info(const uint8_t *data, size_t length, mtp_object_info_t *info);

#endif /* _MTP_STORAGE_H */
