#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "mtp_responder.h"
#include "mtp_storage.h"

const mtp_storage_properties_t* mock_get_properties(void *arg)
{
    return (mtp_storage_properties_t*)mock();
}

uint32_t mock_find_first(void *arg, uint32_t parent, uint32_t *count)
{
    return (uint32_t)mock(arg, parent, count);
}

uint32_t mock_find_next(void *arg)
{
    return (uint32_t)mock(arg);
}

uint64_t mock_free_space(void *arg)
{
    return (uint64_t)mock(arg);
}

int mock_stat(void *arg, uint32_t handle, mtp_object_info_t *info)
{
    return (int)mock(arg, handle, info);
}

int mock_create(void *arg, const mtp_object_info_t *info, uint32_t *handle)
{
    return (int)mock(arg, info, handle);
}

int mock_remove(void *arg, uint32_t handle)
{
    return (int)mock(arg, handle);
}

int mock_open(void *arg, uint32_t handle, const char *mode)
{
    return (int)mock(arg, handle, mode);
}

int mock_read(void *arg, void *buffer, size_t count)
{
    return (int)mock(arg, buffer, count);
}

int mock_write(void *arg, void *buffer, size_t count)
{
    return (int)mock(arg, buffer, count);
}

void mock_close(void *arg)
{
    mock(arg);
}

const struct mtp_storage_api mock_api =
{
    .get_properties = mock_get_properties,
    .find_first = mock_find_first,
    .find_next = mock_find_next,
    .get_free_space = mock_free_space,
    .stat = mock_stat,
    .create = mock_create,
    .remove = mock_remove,
    .open = mock_open,
    .read = mock_read,
    .write = mock_write,
    .close = mock_close,
};

