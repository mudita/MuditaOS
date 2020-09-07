#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "mtp_responder.h"
#include "mtp_storage.h"

uint32_t serialize_storage_list(mtp_storage_t *storage, uint32_t parent, uint8_t *data)
{
    return (uint32_t)mock(storage, parent, data);
}

uint32_t serialize_storage_info(mtp_storage_t *storage, uint8_t *data)
{
    return (uint32_t)mock(storage, data);
}

uint32_t serialize_storage_ids(mtp_storage_t *storage, int count,  uint8_t *data)
{
    return (uint32_t)mock(storage, data);
}

uint32_t serialize_object_info(mtp_object_info_t *info, uint8_t *data)
{
    return (uint32_t)mock(info, data);
}

uint32_t serialize_object_props_supported(uint8_t *data)
{
    return (uint32_t)mock(data);
}

uint32_t serialize_object_prop_desc(uint16_t prop_code, uint8_t *data)
{
    return (uint32_t)mock(prop_code, data);
}

uint32_t serialize_object_prop_value(uint16_t prop_code, mtp_object_info_t *info, uint8_t *data)
{
    return (uint32_t)mock(prop_code, info, data);
}

int deserialize_object_prop_value(uint16_t prop_code, const uint8_t *data, void *value)
{
    return (int)mock(prop_code, data, value);
}

int deserialize_object_info(const uint8_t *data, size_t length, mtp_object_info_t *info)
{
    return (int)mock(data, length, info);
}
