#ifndef _MOCK_MTP_STORAGE_API_H
#define _MOCK_MTP_STORAGE_API_H

extern const struct mtp_storage_api mock_api;

const mtp_storage_properties_t* mock_get_properties(void* arg);
uint32_t mock_find_first(void *arg, uint32_t parent);
uint32_t mock_find_next(void *arg);
uint64_t mock_free_space(void *arg);
int mock_stat(void *arg, uint32_t handle, mtp_object_info_t *info);
int mock_create(void *arg, const mtp_object_info_t *info, uint32_t *handle);
int mock_remove(void *arg, uint32_t handle);
int mock_open(void *arg, uint32_t handle);
int mock_read(void *arg, uint32_t handle, void *buffer, size_t count);
int mock_write(void *arg, uint32_t handle, void *buffer, size_t count);
void mock_close(void *arg, uint32_t handle);

#endif /* _MOCK_MTP_STORAGE_API_H */
