#include "mtp_responder.h"
#include "mtp_storage.h"
#include "mtp_util.h"
#include <string.h>

typedef struct {
    uint16_t id;
    uint16_t type;
    bool writeable;
    uint8_t form;
    size_t offset;
} obj_property_t;

static const obj_property_t properties[] =
{
    // Mandatory:
    { MTP_PROPERTY_STORAGE_ID,       MTP_TYPE_UINT32, false, 0, offsetof(mtp_object_info_t, storage_id)},
    { MTP_PROPERTY_OBJECT_FORMAT,    MTP_TYPE_UINT16, false, 0, offsetof(mtp_object_info_t, format_code)},
    { MTP_PROPERTY_OBJECT_SIZE,      MTP_TYPE_UINT64, false, 0, offsetof(mtp_object_info_t, size)},
    { MTP_PROPERTY_PERSISTENT_UID,   MTP_TYPE_UINT128,false, 0, offsetof(mtp_object_info_t, uuid)},
    { MTP_PROPERTY_NAME,             MTP_TYPE_STR,    true,  0, offsetof(mtp_object_info_t, filename)},
    // Optional:
    { MTP_PROPERTY_DATE_CREATED,     MTP_TYPE_STR,    false, 3, offsetof(mtp_object_info_t, created)},
    { MTP_PROPERTY_DATE_MODIFIED,    MTP_TYPE_STR,    true,  3, offsetof(mtp_object_info_t, modified)},
    { MTP_PROPERTY_PARENT_OBJECT,    MTP_TYPE_UINT32, false, 0, offsetof(mtp_object_info_t, parent)},
    { MTP_PROPERTY_OBJECT_FILE_NAME, MTP_TYPE_STR,    true,  0, offsetof(mtp_object_info_t, filename)},
};
static const int properties_num = sizeof(properties)/sizeof(obj_property_t);

uint32_t serialize_storage_list(mtp_storage_t *storage, uint32_t parent, uint8_t *data)
{
    uint32_t handle = 0;
    uint32_t count = 0;
    uint32_t *ptr = (uint32_t*)(data + 4);

    if ((handle = storage->api->find_first(storage->api_arg, parent, &count))) {
        *ptr++ = handle;
        while((handle = storage->api->find_next(storage->api_arg))) {
            *ptr++ = handle;
        }
    }
    *(uint32_t*)data = count;
    return (1 + count)*sizeof(uint32_t);
}

uint32_t serialize_object_info(mtp_object_info_t* info, uint8_t *data)
{
    uint32_t length = 0;
    length += put_32(data + length, info->storage_id);
    length += put_16(data + length, info->format_code);
    length += put_16(data + length, info->protection);
    length += put_32(data + length, info->size); /* size is 64 bit, but this call can handle only 32 bit */
    length += put_16(data + length, 0);
    length += put_32(data + length, 0);
    length += put_32(data + length, 0);
    length += put_32(data + length, 0);
    length += put_32(data + length, 0);
    length += put_32(data + length, 0);
    length += put_32(data + length, 0);
    length += put_32(data + length, info->parent);
    length += put_16(data + length, info->association_type);
    length += put_32(data + length, info->association_desc);
    length += put_32(data + length, 0);
    length += put_string(data + length, info->filename);
    length += put_date(data + length, info->created);
    length += put_date(data + length, info->modified);
    length += put_string(data + length, NULL);
    return length;
}

int deserialize_object_info(const uint8_t *data, size_t length, mtp_object_info_t *info)
{
    const uint8_t *ptr = data;
    info->storage_id = *(uint32_t*)ptr; ptr += 4;
    info->format_code = *(uint16_t*)ptr; ptr += 2;
    info->protection =  *(uint16_t*)ptr; ptr += 2;
    info->size = *(uint32_t*)ptr; ptr += 4;
    ptr += 1*sizeof(uint16_t);
    ptr += 6*sizeof(uint32_t);
    info->parent = *(uint32_t*)ptr; ptr += 4;
    info->association_type =  *(uint16_t*)ptr; ptr += 2;
    info->association_desc = *(uint32_t*)ptr; ptr += 4;
    ptr += sizeof(uint32_t);
    ptr += get_string(ptr, info->filename, sizeof(info->filename));
    ptr += get_date(ptr, &info->created);
    ptr += get_date(ptr, &info->modified);
    // TODO: lenght check
    return 0;
}

uint32_t serialize_object_props_supported(uint8_t *data)
{
    int i;
    uint16_t *item;
    uint32_t length = 0;

    item = (uint16_t*)(data + 4);
    for(i = 0; i < properties_num; i++)
        item[i] = properties[i].id;
    *(uint32_t*)data = properties_num;
    length=  4 + 2*properties_num;
    return length;
}

uint32_t serialize_storage_info(mtp_storage_t *storage, uint8_t *data)
{
    const struct mtp_storage_api *api = storage->api;
    const struct mtp_storage_props *p = api->get_properties(storage->api_arg);
    uint32_t length = 0;
    length += put_16(data + length, p->type);
    length += put_16(data + length, p->fs_type);
    length += put_16(data + length, p->access_caps);
    length += put_64(data + length, p->capacity);
    length += put_64(data + length, api->get_free_space(storage->api_arg));
    length += put_32(data + length, 0xffffffff);
    length += put_string(data + length, p->description);
    length += put_string(data + length, p->volume_id);
    return length;
}

uint32_t serialize_storage_ids(mtp_storage_t *storage, int count, uint8_t *data)
{
    int i;
    uint32_t *size = (uint32_t*)data;
    uint32_t *values = (uint32_t*)&data[4];

    *size = (uint32_t)count;
    for(i = 0; i < count; i++)
        values[i] = storage->id;

    return 4*(count+1);
}

static uint32_t serialize_type_desc_uint16(const obj_property_t *prop, uint8_t *data)
{
    *(uint16_t*)data = prop->id;                     /* Property Code*/
    *(uint16_t*)(data + 2) = MTP_TYPE_UINT16;        /* Datatype */
    *(uint8_t*)(data + 4) = prop->writeable ? 1 : 0; /* Get/GetSet */
    *(uint16_t*)(data + 5) = 0;                      /* Default value*/
    *(uint32_t*)(data + 7) = 0;                      /* Group */
    *(uint8_t*)(data + 11) = 0;                      /* Form */
    return 12;
}

static uint32_t serialize_type_desc_uint32(const obj_property_t *prop, uint8_t *data)
{
    *(uint16_t*)data = prop->id;                      /* Property Code*/
    *(uint16_t*)(data + 2) = MTP_TYPE_UINT32;         /* Datatype */
    *(uint8_t*)(data + 4) = prop->writeable ? 1 : 0;  /* Get/GetSet */
    *(uint32_t*)(data + 5) = 0;                       /* Default value*/
    *(uint32_t*)(data + 9) = 0;                       /* Group */
    *(uint8_t*)(data + 13) = 0;                       /* Form */
    return 14;
}

static uint32_t serialize_type_desc_uint64(const obj_property_t *prop, uint8_t *data)
{
    *(uint16_t*)data = prop->id;                       /* Property Code*/
    *(uint16_t*)(data + 2) = MTP_TYPE_UINT64;          /* Datatype */
    *(uint8_t*)(data + 4) = prop->writeable ? 1 : 0;   /* Get/GetSet */
    *(uint32_t*)(data + 5) = 0;                        /* Default value*/
    *(uint32_t*)(data + 9) = 0;
    *(uint32_t*)(data + 13) = 0;                       /* Group */
    *(uint8_t*)(data + 17) = 0;                        /* Form */
    return 18;
}

static uint32_t serialize_type_desc_uint128(const obj_property_t *prop, uint8_t *data)
{
    *(uint16_t*)data = prop->id;                      /* Property Code*/
    *(uint16_t*)(data + 2) = MTP_TYPE_UINT128;        /* Datatype */
    *(uint8_t*)(data + 4) = prop->writeable ? 1 : 0;  /* Get/GetSet */
    *(uint32_t*)(data + 5) = 0;                       /* Default value*/
    *(uint32_t*)(data + 9) = 0;
    *(uint32_t*)(data + 13) = 0;
    *(uint32_t*)(data + 17) = 0;
    *(uint32_t*)(data + 21) = 0;                      /* Group */
    *(uint8_t*)(data + 25) = 0;                       /* Form */
    return 26;
}


static uint32_t serialize_type_desc_str(const obj_property_t *prop, uint8_t *data)
{
    *(uint16_t*)data = prop->id;                      /* Property Code*/
    *(uint16_t*)(data + 2) = MTP_TYPE_STR;            /* Datatype */
    *(uint8_t*)(data + 4) = prop->writeable ? 1 : 0;  /* Get/GetSet */
    *(uint8_t*)(data + 5) = 0;                        /* Default value*/
    *(uint32_t*)(data + 6) = 0;                       /* Group */
    *(uint8_t*)(data + 10) = prop->form;              /* Form */
    return 11;
}

uint32_t serialize_type_desc(const obj_property_t* prop, uint8_t *data)
{
    uint32_t length = 0;
    switch(prop->type)
    {
        case MTP_TYPE_UINT16:
            length = serialize_type_desc_uint16(prop, data);
            break;
        case MTP_TYPE_UINT32:
            length = serialize_type_desc_uint32(prop, data);
            break;
        case MTP_TYPE_UINT64:
            length = serialize_type_desc_uint64(prop, data);
            break;
        case MTP_TYPE_UINT128:
            length = serialize_type_desc_uint128(prop, data);
            break;
        case MTP_TYPE_STR:
            length = serialize_type_desc_str(prop, data);
            break;
    }
    return length;
}

uint32_t serialize_object_prop_desc(uint16_t prop_code, uint8_t *data)
{
    uint32_t length = 0;
    int i;
    for(i = 0; i < properties_num; i++)
    {
        if (properties[i].id == prop_code)
        {
            length = serialize_type_desc(&properties[i], data);
            break;
        }
    }
    return length;
}

static uint32_t serialize_type_value_uint16(const obj_property_t *prop, mtp_object_info_t *info, uint8_t *data)
{
    *(uint16_t*)data = *(uint16_t*)((uint8_t*)info + prop->offset);
    return 2;
}

static uint32_t serialize_type_value_uint32(const obj_property_t *prop, mtp_object_info_t *info, uint8_t *data)
{
    *(uint32_t*)data = *(uint32_t*)((uint8_t*)info + prop->offset);
    return 4;
}

static uint32_t serialize_type_value_uint64(const obj_property_t *prop, mtp_object_info_t *info, uint8_t *data)
{
    memcpy(data, (uint8_t*)info + prop->offset, 8);
    return 8;
}

static uint32_t serialize_type_value_uint128(const obj_property_t *prop, mtp_object_info_t *info, uint8_t *data)
{
    memcpy(data, (uint8_t*)info + prop->offset, 16);
    return 16;
}

static uint32_t serialize_type_value_str(const obj_property_t *prop, mtp_object_info_t *info, uint8_t *data)
{
    uint32_t length = 0;
    if (prop->form == 0)
    {
        length = put_string(data, (char*)info + prop->offset);
    }
    else if (prop->form == 3)
    {
        time_t *t  = (time_t*)((uint8_t*)info + prop->offset);
        length = put_date(data, *t);
    }
    return length;
}

uint32_t serialize_prop_value(const obj_property_t *prop, mtp_object_info_t *info, uint8_t *data)
{
    uint32_t length = 0;
    switch(prop->type)
    {
        case MTP_TYPE_UINT16:
            length = serialize_type_value_uint16(prop, info, data);
            break;
        case MTP_TYPE_UINT32:
            length = serialize_type_value_uint32(prop, info, data);
            break;
        case MTP_TYPE_UINT64:
            length = serialize_type_value_uint64(prop, info, data);
            break;
        case MTP_TYPE_UINT128:
            length = serialize_type_value_uint128(prop, info, data);
            break;
        case MTP_TYPE_STR:
            length = serialize_type_value_str(prop, info, data);
            break;
    }
    return length;
}

uint32_t serialize_object_prop_value(uint16_t prop_code, mtp_object_info_t *info, uint8_t *data)
{
    uint32_t length = 0;
    int i;
    for(i = 0; i < properties_num; i++)
    {
        if (properties[i].id == prop_code)
        {
            length += serialize_prop_value(&properties[i], info, data);
            break;
        }
    }
    return length;
}

uint32_t deserialize_prop_value(const obj_property_t *prop, const uint8_t *data, void *value)
{
    uint32_t length = 0;
    switch(prop->type)
    {
        case MTP_TYPE_STR:
            length = get_string(data, value, 64);
            break;
    }
    return length;
}

int deserialize_object_prop_value(uint16_t prop_code, const uint8_t *data, void *value)
{
    uint32_t length = 0;
    int i;
    for(i = 0; i < properties_num; i++)
    {
        if (properties[i].id == prop_code && properties[i].writeable)
        {
            length += deserialize_prop_value(&properties[i], data, value);
            break;
        }
    }
    return length;

}
