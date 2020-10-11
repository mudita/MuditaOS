#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "defines.h"
#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_dataset.h"
#include "mtp_util.h"

#if 0
#   include <stdio.h>
#   define log_info(format, ...) printf(format"\n", __VA_ARGS__)
#   define log_error(format,...) printf(format"\n", __VA_ARGS__)
#else
#   define log_info(...) while(0) {}
#   define log_error(...) while(0) {}
#endif

struct mtp_responder
{
    bool session_open;
    uint32_t session_id;            /* not really used in USB implementation */

    mtp_storage_t storage;
    const mtp_device_info_t *device_info;

    struct {
        uint32_t id;
        uint16_t prop_code;
        uint16_t opcode;
        uint32_t handle;
        size_t total;
        size_t in_buffer;
        bool file_open;
        bool keep;
        union {
            size_t sent;
            size_t received;
        };
    } transaction;
    union {
        void *buffer;
        mtp_data_cntr_t *cntr;
    };
    size_t buf_size;
};

typedef struct {
    const char *text;
    uint16_t id;
} dbg_map_entry_t;

static const char* dbg_operation(uint16_t op)
{
    static const dbg_map_entry_t ops[] = {
        { "MTP_OPERATION_GET_DEVICE_INFO", 0x1001 },
        { "MTP_OPERATION_OPEN_SESSION", 0x1002 },
        { "MTP_OPERATION_CLOSE_SESSION", 0x1003 },
        { "MTP_OPERATION_GET_STORAGE_IDS", 0x1004 },
        { "MTP_OPERATION_GET_STORAGE_INFO", 0x1005 },
        { "MTP_OPERATION_GET_NUM_OBJECTS", 0x1006 },
        { "MTP_OPERATION_GET_OBJECT_HANDLES", 0x1007 },
        { "MTP_OPERATION_GET_OBJECT_INFO", 0x1008 },
        { "MTP_OPERATION_GET_OBJECT", 0x1009 },
        { "MTP_OPERATION_GET_THUMB", 0x100A },
        { "MTP_OPERATION_DELETE_OBJECT", 0x100B },
        { "MTP_OPERATION_SEND_OBJECT_INFO", 0x100C },
        { "MTP_OPERATION_SEND_OBJECT", 0x100D },
        { "MTP_OPERATION_INITIATE_CAPTURE", 0x100E },
        { "MTP_OPERATION_FORMAT_STORE", 0x100F },
        { "MTP_OPERATION_RESET_DEVICE", 0x1010 },
        { "MTP_OPERATION_SELF_TEST", 0x1011 },
        { "MTP_OPERATION_SET_OBJECT_PROTECTION", 0x1012 },
        { "MTP_OPERATION_POWER_DOWN", 0x1013 },
        { "MTP_OPERATION_GET_DEVICE_PROP_DESC", 0x1014 },
        { "MTP_OPERATION_GET_DEVICE_PROP_VALUE", 0x1015 },
        { "MTP_OPERATION_SET_DEVICE_PROP_VALUE", 0x1016 },
        { "MTP_OPERATION_RESET_DEVICE_PROP_VALUE", 0x1017 },
        { "MTP_OPERATION_TERMINATE_OPEN_CAPTURE", 0x1018 },
        { "MTP_OPERATION_MOVE_OBJECT", 0x1019 },
        { "MTP_OPERATION_COPY_OBJECT", 0x101A },
        { "MTP_OPERATION_GET_PARTIAL_OBJECT", 0x101B },
        { "MTP_OPERATION_INITIATE_OPEN_CAPTURE", 0x101C },
        { "MTP_OPERATION_GET_OBJECT_PROPS_SUPPORTED", 0x9801 },
        { "MTP_OPERATION_GET_OBJECT_PROP_DESC", 0x9802 },
        { "MTP_OPERATION_GET_OBJECT_PROP_VALUE", 0x9803 },
        { "MTP_OPERATION_SET_OBJECT_PROP_VALUE", 0x9804 },
        { "MTP_OPERATION_GET_OBJECT_PROP_LIST", 0x9805 },
        { "MTP_OPERATION_SET_OBJECT_PROP_LIST", 0x9806 },
        { "MTP_OPERATION_GET_INTERDEPENDENT_PROP_DESC", 0x9807 },
        { "MTP_OPERATION_SEND_OBJECT_PROP_LIST", 0x9808 },
        { "MTP_OPERATION_GET_OBJECT_REFERENCES", 0x9810 },
        { "MTP_OPERATION_SET_OBJECT_REFERENCES", 0x9811 },
        { "MTP_OPERATION_SKIP", 0x9820 },
        { NULL, 0 }
    };
    const dbg_map_entry_t *e = ops;
    while(e->id != 0)
    {
        if (e->id == op)
            return e->text;
        e++;
    }
    return "Operation not found!";
}

static const char* dbg_event(uint16_t event)
{
    static const dbg_map_entry_t events[] = {
        { "MTP_EVENT_UNDEFINED", 0x4000 },
        { "MTP_EVENT_CANCEL_TRANSACTION", 0x4001 },
        { "MTP_EVENT_OBJECT_ADDED", 0x4002 },
        { "MTP_EVENT_OBJECT_REMOVED", 0x4003 },
        { "MTP_EVENT_STORE_ADDED", 0x4004 },
        { "MTP_EVENT_STORE_REMOVED", 0x4005 },
        { "MTP_EVENT_DEVICE_PROP_CHANGED", 0x4006 },
        { "MTP_EVENT_OBJECT_INFO_CHANGED", 0x4007 },
        { "MTP_EVENT_DEVICE_INFO_CHANGED", 0x4008 },
        { "MTP_EVENT_REQUEST_OBJECT_TRANSFER", 0x4009 },
        { "MTP_EVENT_STORE_FULL", 0x400A },
        { "MTP_EVENT_DEVICE_RESET", 0x400B },
        { "MTP_EVENT_STORAGE_INFO_CHANGED", 0x400C },
        { "MTP_EVENT_CAPTURE_COMPLETE", 0x400D },
        { "MTP_EVENT_UNREPORTED_STATUS", 0x400E },
        { "MTP_EVENT_OBJECT_PROP_CHANGED", 0xC801 },
        { "MTP_EVENT_OBJECT_PROP_DESC_CHANGED", 0xC802 },
        { "MTP_EVENT_OBJECT_REFERENCES_CHANGED", 0xC803 },
        { NULL, 0 }
    };
    const dbg_map_entry_t *e = events;
    while(e->id != 0)
    {
        if (e->id == event)
            return e->text;
        e++;
    }
    return "Event not found!";
}

static const char* dbg_result(uint16_t result)
{
    static const dbg_map_entry_t results[] = {
        { "MTP_RESPONSE_UNDEFINED", 0x2000 },
        { "MTP_RESPONSE_OK", 0x2001 },
        { "MTP_RESPONSE_GENERAL_ERROR", 0x2002 },
        { "MTP_RESPONSE_SESSION_NOT_OPEN", 0x2003 },
        { "MTP_RESPONSE_INVALID_TRANSACTION_ID", 0x2004 },
        { "MTP_RESPONSE_OPERATION_NOT_SUPPORTED", 0x2005 },
        { "MTP_RESPONSE_PARAMETER_NOT_SUPPORTED", 0x2006 },
        { "MTP_RESPONSE_INCOMPLETE_TRANSFER", 0x2007 },
        { "MTP_RESPONSE_INVALID_STORAGE_ID", 0x2008 },
        { "MTP_RESPONSE_INVALID_OBJECT_HANDLE", 0x2009 },
        { "MTP_RESPONSE_DEVICE_PROP_NOT_SUPPORTED", 0x200A },
        { "MTP_RESPONSE_INVALID_OBJECT_FORMAT_CODE", 0x200B },
        { "MTP_RESPONSE_STORAGE_FULL", 0x200C },
        { "MTP_RESPONSE_OBJECT_WRITE_PROTECTED", 0x200D },
        { "MTP_RESPONSE_STORE_READ_ONLY", 0x200E },
        { "MTP_RESPONSE_ACCESS_DENIED", 0x200F },
        { "MTP_RESPONSE_NO_THUMBNAIL_PRESENT", 0x2010 },
        { "MTP_RESPONSE_SELF_TEST_FAILED", 0x2011 },
        { "MTP_RESPONSE_PARTIAL_DELETION", 0x2012 },
        { "MTP_RESPONSE_STORE_NOT_AVAILABLE", 0x2013 },
        { "MTP_RESPONSE_SPECIFICATION_BY_FORMAT_UNSUPPORTED", 0x2014 },
        { "MTP_RESPONSE_NO_VALID_OBJECT_INFO", 0x2015 },
        { "MTP_RESPONSE_INVALID_CODE_FORMAT", 0x2016 },
        { "MTP_RESPONSE_UNKNOWN_VENDOR_CODE", 0x2017 },
        { "MTP_RESPONSE_CAPTURE_ALREADY_TERMINATED", 0x2018 },
        { "MTP_RESPONSE_DEVICE_BUSY", 0x2019 },
        { "MTP_RESPONSE_INVALID_PARENT_OBJECT", 0x201A },
        { "MTP_RESPONSE_INVALID_DEVICE_PROP_FORMAT", 0x201B },
        { "MTP_RESPONSE_INVALID_DEVICE_PROP_VALUE", 0x201C },
        { "MTP_RESPONSE_INVALID_PARAMETER", 0x201D },
        { "MTP_RESPONSE_SESSION_ALREADY_OPEN", 0x201E },
        { "MTP_RESPONSE_TRANSACTION_CANCELLED", 0x201F },
        { "MTP_RESPONSE_SPECIFICATION_OF_DESTINATION_UNSUPPORTED", 0x2020 },
        { "MTP_RESPONSE_INVALID_OBJECT_PROP_CODE", 0xA801 },
        { "MTP_RESPONSE_INVALID_OBJECT_PROP_FORMAT", 0xA802 },
        { "MTP_RESPONSE_INVALID_OBJECT_PROP_VALUE", 0xA803 },
        { "MTP_RESPONSE_INVALID_OBJECT_REFERENCE", 0xA804 },
        { "MTP_RESPONSE_GROUP_NOT_SUPPORTED", 0xA805 },
        { "MTP_RESPONSE_INVALID_DATASET", 0xA806 },
        { "MTP_RESPONSE_SPECIFICATION_BY_GROUP_UNSUPPORTED", 0xA807 },
        { "MTP_RESPONSE_SPECIFICATION_BY_DEPTH_UNSUPPORTED", 0xA808 },
        { "MTP_RESPONSE_OBJECT_TOO_LARGE", 0xA809 },
        { "MTP_RESPONSE_OBJECT_PROP_NOT_SUPPORTED", 0xA80A },
        { NULL, 0 }
    };
    const dbg_map_entry_t *e = results;
    while(e->id != 0)
    {
        if (e->id == result)
            return e->text;
        e++;
    }
    return "Result not found!";

}

void mtp_responder_init(mtp_responder_t *mtp)
{
    assert(mtp);
    memset(mtp, 0, sizeof(mtp_responder_t));
}

mtp_responder_t* mtp_responder_alloc(void)
{
    return malloc(sizeof(mtp_responder_t));
}

void mtp_responder_free(mtp_responder_t *mtp)
{
    assert(mtp);
    free(mtp);
}

void mtp_responder_set_data_buffer(mtp_responder_t *mtp, void *buffer, size_t size)
{
    assert(mtp && buffer);
    mtp->buffer = buffer;
    mtp->buf_size = size;
}

int mtp_responder_set_device_info(mtp_responder_t *mtp,
                                  const mtp_device_info_t *info)
{
    assert(mtp && info);
    if (!info || !info->manufacturer
              || !info->model
              || !info->serial
              || !info->version
              || (strlen(info->serial) != 32))
    {
        return -1;
    }
    mtp->device_info = info;
    return 0;
}

int mtp_responder_set_storage(mtp_responder_t *mtp,
        uint32_t storage_id,
        const struct mtp_storage_api *api,
        void *api_arg)
{
    assert(mtp && api);
    if (!storage_id || !api)
        return -1;
    mtp->storage.api = api;
    mtp->storage.id = storage_id;
    mtp->storage.api_arg = api_arg;
    return 0;
}

static void set_data_header(mtp_responder_t *mtp)
{
    mtp->cntr->header.type = MTP_CONTAINER_TYPE_DATA;
    mtp->cntr->header.operation_code = mtp->transaction.opcode;
    mtp->cntr->header.transaction_id = mtp->transaction.id;
    mtp->cntr->header.length = MTP_CONTAINER_HEADER_SIZE + mtp->transaction.total;
}

static uint16_t operation_open_session(mtp_responder_t *mtp, const mtp_op_cntr_t *request)
{
    if (mtp_container_get_param_count(request) > 0)
        mtp->session_id = request->parameter[0];

    mtp->session_open = true;
    return MTP_RESPONSE_OK;
}

static uint16_t operation_get_device_info(mtp_responder_t *mtp, const mtp_op_cntr_t *request)
{
  uint8_t *payload = (uint8_t *)mtp->cntr->payload;
  uint32_t total = serialize_device_info(mtp->device_info, payload);
  mtp->transaction.total = total;
  mtp->transaction.in_buffer = total;

  return MTP_RESPONSE_OK;
}

static uint16_t operation_close_session(mtp_responder_t *mtp, const mtp_op_cntr_t *request)
{
    uint16_t error;
    if (mtp->session_open)
    {
        mtp->session_open = false;
        error = MTP_RESPONSE_OK;
    }
    else
    {
        error = MTP_RESPONSE_SESSION_NOT_OPEN;
    }
    return error;
}

static uint16_t operation_get_storage_ids(mtp_responder_t *mtp, const mtp_op_cntr_t *request)
{
  uint8_t *payload = (uint8_t *)mtp->cntr->payload;
  uint32_t total = serialize_storage_ids(&mtp->storage, 1, payload);
  mtp->transaction.total = total;
  mtp->transaction.in_buffer = total;

  return MTP_RESPONSE_OK;
}

static uint16_t operation_get_storage_info(mtp_responder_t *mtp, const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint32_t storageID = request->parameter[0];

    if (storageID == mtp->storage.id)
    {
      uint8_t *payload = (uint8_t *)mtp->cntr->payload;
      uint32_t total = serialize_storage_info(&mtp->storage, payload);
      mtp->transaction.total = total;
      mtp->transaction.in_buffer = total;
      error = MTP_RESPONSE_OK;
    }
    else
    {
        error = MTP_RESPONSE_INVALID_STORAGE_ID;
    }

    return error;
}

static uint16_t operation_get_object_handles(mtp_responder_t *mtp, const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint8_t *payload = (uint8_t *)mtp->cntr->payload;

    uint32_t storageID = request->parameter[0];
    uint32_t objectFormatCode = request->parameter[1];
    uint32_t parent_handle = request->parameter[2];

    if (objectFormatCode != 0)
    {
        error = MTP_RESPONSE_SPECIFICATION_BY_FORMAT_UNSUPPORTED;
        goto get_object_handles_exit;
    }

    if (!mtp->storage.id || !mtp->storage.api)
    {
        error = MTP_RESPONSE_STORE_NOT_AVAILABLE;
        goto get_object_handles_exit;
    }

    if (storageID != 0xFFFFFFFF && storageID != mtp->storage.id)
    {
        error = MTP_RESPONSE_INVALID_STORAGE_ID;
        goto get_object_handles_exit;
    }

    uint32_t count = 0;
    uint32_t handle = 0;
    uint32_t *ptr = (uint32_t*)payload;

    handle = mtp->storage.api->find_first(mtp->storage.api_arg, parent_handle, &count);
    *ptr++ = count;

    if (count)
    {
        uint32_t available = (mtp->buf_size - MTP_CONTAINER_HEADER_SIZE) / sizeof(uint32_t) - 1; // -1 beacuse 4 bytes for array length
        uint32_t fit_to_buf = count > available ? available : count;
        int i;

        *ptr++ = handle;
        // (fit_to_buf - 1) because first already is in buffer
        for (i = 0; i < (fit_to_buf - 1); i++)
        {
            handle = mtp->storage.api->find_next(mtp->storage.api_arg);
            if(!handle) {
                error = MTP_RESPONSE_STORE_NOT_AVAILABLE;
                goto get_object_handles_exit;
            }
            *ptr++ = handle;
        }
        mtp->transaction.in_buffer = (1 + fit_to_buf) * sizeof(uint32_t);
    } else {
        mtp->transaction.in_buffer = (1) * sizeof(uint32_t);
    }

    mtp->transaction.total = (1 + count) * sizeof(uint32_t);

    error = MTP_RESPONSE_OK;

get_object_handles_exit:
    return error;
}

static uint16_t operation_get_object_info(mtp_responder_t *mtp, const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint8_t *payload = (uint8_t *)mtp->cntr->payload;
    uint32_t obj_handle = request->parameter[0];
    mtp_object_info_t info = {0};

    if (!obj_handle || mtp->storage.api->stat(mtp->storage.api_arg, obj_handle, &info) != 0)
    {
        error = MTP_RESPONSE_INVALID_OBJECT_HANDLE;
        goto get_object_info_exit;
    }

    uint32_t total = serialize_object_info(&info, payload);
    mtp->transaction.total = total;
    mtp->transaction.in_buffer = total;
    error = MTP_RESPONSE_OK;

get_object_info_exit:
    return error;
}

static uint16_t operation_get_object_props_supported(mtp_responder_t *mtp,
        const mtp_op_cntr_t *request)
{
    uint8_t *payload = (uint8_t *)mtp->cntr->payload;
    uint16_t error = MTP_RESPONSE_INVALID_OBJECT_FORMAT_CODE;
    uint32_t format_code = request->parameter[0];

    if (is_format_code_supported(format_code))
    {
        uint32_t total = serialize_object_props_supported(payload);
        mtp->transaction.total = total;
        mtp->transaction.in_buffer = total;
        error = MTP_RESPONSE_OK;
    }

    return error;
}

static uint16_t operation_get_object_prop_desc(mtp_responder_t *mtp,
        const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint8_t *payload = (uint8_t *)mtp->cntr->payload;
    uint16_t prop_code = request->parameter[0];
    uint16_t format_code = request->parameter[1];

    if (!is_format_code_supported(format_code))
    {
        error = MTP_RESPONSE_INVALID_OBJECT_FORMAT_CODE;
        goto get_object_prop_desc_exit;
    }

    uint32_t total = serialize_object_prop_desc(prop_code, payload);
    if (!total)
    {
        error = MTP_RESPONSE_INVALID_OBJECT_PROP_CODE;
        goto get_object_prop_desc_exit;
    }

    mtp->transaction.total = total;
    mtp->transaction.in_buffer = total;
    error = MTP_RESPONSE_OK;

get_object_prop_desc_exit:
    return error;
}

static uint16_t operation_get_object_prop_value(mtp_responder_t *mtp,
        const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint8_t *payload = (uint8_t *)mtp->cntr->payload;
    uint32_t obj_handle = request->parameter[0];
    uint16_t prop_code = request->parameter[1];
    mtp_object_info_t info = {0};

    if (!obj_handle || mtp->storage.api->stat(mtp->storage.api_arg, obj_handle, &info) != 0)
    {
        error = MTP_RESPONSE_INVALID_OBJECT_HANDLE;
        goto get_object_prop_value_exit;
    }

    uint32_t total = serialize_object_prop_value(prop_code, &info, payload);
    if (total == 0)
    {
        error = MTP_RESPONSE_INVALID_OBJECT_PROP_CODE;
        goto get_object_prop_value_exit;
    }

    mtp->transaction.total = total;
    mtp->transaction.in_buffer = total;
    error = MTP_RESPONSE_OK;

get_object_prop_value_exit:
    return error;
}

static uint16_t operation_set_object_prop_value(mtp_responder_t *mtp,
        const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint32_t obj_handle = request->parameter[0];
    uint16_t prop_code = request->parameter[1];
    mtp_object_info_t info = {0};

    if (!obj_handle || mtp->storage.api->stat(mtp->storage.api_arg, obj_handle, &info) != 0)
    {
        error = MTP_RESPONSE_INVALID_OBJECT_HANDLE;
        goto set_object_prop_value_exit;
    }

    mtp->transaction.handle = obj_handle;
    mtp->transaction.prop_code = prop_code;;
    error = 0;

set_object_prop_value_exit:
    return error;
}


static uint16_t operation_get_object(mtp_responder_t *mtp,
        const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint32_t obj_handle = request->parameter[0];
    mtp_object_info_t info;

    if (!obj_handle ||
           mtp->storage.api->stat(mtp->storage.api_arg, obj_handle, &info) ||
           mtp->storage.api->open(mtp->storage.api_arg, obj_handle, "r"))
    {
        mtp->transaction.file_open = true;
        error = MTP_RESPONSE_INVALID_OBJECT_HANDLE;
        goto get_object_exit;
    }

    size_t empty_space = (mtp->buf_size - MTP_CONTAINER_HEADER_SIZE);
    uint32_t total = info.size;

    int data_read = mtp->storage.api->read(mtp->storage.api_arg,
                           mtp->cntr->payload,
                           empty_space);
    if (data_read < 0)
    {
        mtp->storage.api->close(mtp->storage.api_arg);
        error = MTP_RESPONSE_INCOMPLETE_TRANSFER;
        goto get_object_exit;

    }

    mtp->transaction.total = total;
    mtp->transaction.in_buffer = data_read;
    error = MTP_RESPONSE_OK;

get_object_exit:
    return error;
}

static uint16_t operation_delete_object(mtp_responder_t *mtp,
        const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint32_t obj_handle = request->parameter[0];

    if (!obj_handle)
    {
        error = MTP_RESPONSE_INVALID_OBJECT_HANDLE;
        goto delete_object_exit;
    }
    if (mtp->storage.api->remove(mtp->storage.api_arg, obj_handle))
    {
        error = MTP_RESPONSE_INVALID_OBJECT_HANDLE;
        goto delete_object_exit;
    }
    error = MTP_RESPONSE_OK;

delete_object_exit:
    return error;
}

static uint16_t operation_send_object_info(mtp_responder_t *mtp,
        const mtp_op_cntr_t *request)
{
    uint16_t error;
    uint32_t storage_id = request->parameter[0];
    uint32_t parent_handle = request->parameter[1];
    (void)parent_handle;

    if (storage_id == mtp->storage.id)
    {
        error = 0;
    }
    else
    {
        error = MTP_RESPONSE_INVALID_STORAGE_ID;
    }

    return error;
}

static uint16_t operation_send_object(mtp_responder_t *mtp,
        const mtp_op_cntr_t *request)
{
    uint16_t error = 0;
    /* TODO: restore open mode, we're opening for write. */
    if (mtp->storage.api->open(mtp->storage.api_arg, mtp->transaction.handle, "w+"))
    {
        error = MTP_RESPONSE_STORE_NOT_AVAILABLE;
    } else {
        mtp->transaction.file_open = true;
    }

    return error;
}

static uint16_t handle_command(mtp_responder_t *mtp, const mtp_op_cntr_t *request)
{
    uint16_t error = MTP_RESPONSE_UNDEFINED;

    mtp->transaction.id = request->header.transaction_id;
    mtp->transaction.opcode = request->header.operation_code;
    if (!mtp->transaction.keep)
    {
        mtp->transaction.in_buffer = 0;
        mtp->transaction.total = 0;
    }

    log_info("OP> %s (0x%x)", dbg_operation(request->header.operation_code), (unsigned int) mtp->transaction.id);

    switch(request->header.operation_code)
    {
        case MTP_OPERATION_OPEN_SESSION:
            error = operation_open_session(mtp, request);
            break;
        case MTP_OPERATION_GET_DEVICE_INFO:
            error = operation_get_device_info(mtp, request);
            break;
        case MTP_OPERATION_CLOSE_SESSION:
            error = operation_close_session(mtp, request);
            break;
        case MTP_OPERATION_GET_STORAGE_IDS:
            error = operation_get_storage_ids(mtp, request);
            break;
        case MTP_OPERATION_GET_STORAGE_INFO:
            error = operation_get_storage_info(mtp, request);
            break;
        case MTP_OPERATION_GET_OBJECT_HANDLES:
            error = operation_get_object_handles(mtp, request);
            break;
        case MTP_OPERATION_GET_OBJECT_INFO:
            error = operation_get_object_info(mtp, request);
            break;
        case MTP_OPERATION_GET_OBJECT_PROPS_SUPPORTED:
            error = operation_get_object_props_supported(mtp, request);
            break;
        case MTP_OPERATION_GET_OBJECT_PROP_DESC:
            error = operation_get_object_prop_desc(mtp, request);
            break;
        case MTP_OPERATION_GET_OBJECT_PROP_VALUE:
            error = operation_get_object_prop_value(mtp, request);
            break;
        case MTP_OPERATION_SET_OBJECT_PROP_VALUE:
            error = operation_set_object_prop_value(mtp, request);
            break;
        case MTP_OPERATION_GET_OBJECT:
            error = operation_get_object(mtp, request);
            break;
        case MTP_OPERATION_DELETE_OBJECT:
            error = operation_delete_object(mtp, request);
            break;
        case MTP_OPERATION_SEND_OBJECT_INFO:
            error = operation_send_object_info(mtp, request);
            break;
        case MTP_OPERATION_SEND_OBJECT:
            error = operation_send_object(mtp, request);
            break;
        default:
            error = MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
            log_error("Operation %s not supported\n", dbg_operation(request->header.operation_code));
    }


    return error;
}


static uint16_t data_set_object_prop_value(mtp_responder_t *mtp, const mtp_data_cntr_t* incoming, size_t size)
{
    char name[64];
    uint16_t error;
    if (!deserialize_object_prop_value(mtp->transaction.prop_code, incoming->payload, name)
            || mtp->storage.api->rename(mtp->storage.api_arg, mtp->transaction.handle, name))
    {
        error = MTP_RESPONSE_INVALID_OBJECT_PROP_VALUE;
        goto data_set_object_prop_value_exit;
    }
    error = MTP_RESPONSE_OK;

data_set_object_prop_value_exit:
    return error;
}

static uint16_t data_send_object_info(mtp_responder_t *mtp, const mtp_data_cntr_t* incoming, size_t size)
{
    uint16_t error;
    mtp_object_info_t info;
    uint32_t obj_handle = 0;
    size_t plen = incoming->header.length - MTP_CONTAINER_HEADER_SIZE;

    if (deserialize_object_info(incoming->payload, plen, &info))
    {
        error = MTP_RESPONSE_INVALID_DATASET;
        goto send_object_info_exit;
    }

    if (!is_format_code_supported(info.format_code))
    {
        error = MTP_RESPONSE_INVALID_OBJECT_FORMAT_CODE;
        goto send_object_info_exit;
    }

    if (mtp->storage.api->create(mtp->storage.api_arg, &info, &obj_handle))
    {
        error = MTP_RESPONSE_STORE_NOT_AVAILABLE;
        goto send_object_info_exit;
    }

    mtp->transaction.handle = obj_handle;
    mtp->transaction.total = info.size;
    mtp->transaction.received = 0;
    mtp->transaction.keep = true;
    error = MTP_RESPONSE_OK;
send_object_info_exit:
    return error;
}

static uint16_t data_send_object(mtp_responder_t *mtp, const mtp_data_cntr_t* incoming, size_t size)
{
    uint16_t error;
    size_t plen = size - MTP_CONTAINER_HEADER_SIZE;

    if (plen > 0)
    {
        if (mtp->storage.api->write(mtp->storage.api_arg, (void*)incoming->payload, plen) < 0)
        {
            error = MTP_RESPONSE_OBJECT_TOO_LARGE;
            mtp->transaction.keep = false;
            mtp->storage.api->close(mtp->storage.api_arg);
            mtp->transaction.file_open = false;
            goto data_send_object_exit;
        }
    }

    if (plen >= mtp->transaction.total)
    {
        mtp->storage.api->close(mtp->storage.api_arg);
        mtp->transaction.file_open = false;
        mtp->transaction.keep = false;
        error = MTP_RESPONSE_OK;
    }
    else
    {
        mtp->transaction.received = plen;
        error = 0;
    }
data_send_object_exit:
    return error;
}

static uint16_t handle_data(mtp_responder_t *mtp, const mtp_data_cntr_t *incoming, size_t size)
{
    uint16_t error = MTP_RESPONSE_UNDEFINED;

    log_info("DT> %s", dbg_operation(incoming->header.operation_code));
    if (mtp->transaction.id != incoming->header.transaction_id
            && mtp->transaction.opcode != incoming->header.operation_code)
    {
        error = MTP_RESPONSE_INVALID_TRANSACTION_ID;
        goto handle_data_exit;
    }

    switch(incoming->header.operation_code)
    {
        case MTP_OPERATION_SET_OBJECT_PROP_VALUE:
            error = data_set_object_prop_value(mtp, incoming, size);
            break;
        case MTP_OPERATION_SEND_OBJECT_INFO:
            error = data_send_object_info(mtp, incoming, size);
            break;
        case MTP_OPERATION_SEND_OBJECT:
            error = data_send_object(mtp, incoming, size);
            break;
        default:
            error = MTP_RESPONSE_OPERATION_NOT_SUPPORTED;
    }
handle_data_exit:
    return error;
}

void mtp_cancel_transaction_from_event(mtp_responder_t *mtp)
{
}

uint16_t mtp_responder_handle_request(mtp_responder_t *mtp, const void *request, const size_t req_size)
{
    assert(mtp && request);

    uint16_t error;
    mtp_cntr_hdr_t *header = (mtp_cntr_hdr_t*)request;

    switch(header->type)
    {
        case MTP_CONTAINER_TYPE_COMMAND:
            error = handle_command(mtp, (mtp_op_cntr_t*)request);
            break;
        case MTP_CONTAINER_TYPE_DATA:
            error = handle_data(mtp, (mtp_data_cntr_t*)request, req_size);
            break;
        case MTP_CONTAINER_TYPE_EVENT:
        case MTP_CONTAINER_TYPE_RESPONSE:
        default:
            /* TODO: logging */
            error = MTP_RESPONSE_UNDEFINED;
    }
    return error;
}

size_t mtp_responder_get_data(mtp_responder_t *mtp)
{
    assert(mtp);
    size_t cntr_length = 0;

    if (mtp->transaction.in_buffer)
    {
        set_data_header(mtp);
        cntr_length = MTP_CONTAINER_HEADER_SIZE + mtp->transaction.in_buffer;
        mtp->transaction.sent = mtp->transaction.in_buffer;
        log_info("DT> %s: %d", dbg_operation(mtp->transaction.opcode), mtp->transaction.in_buffer);
        mtp->transaction.in_buffer = 0;
    }
    else if (mtp->transaction.opcode == MTP_OPERATION_GET_OBJECT_HANDLES)
    {
        if (mtp->transaction.sent < mtp->transaction.total)
        {
            uint32_t available = (mtp->buf_size) / sizeof(uint32_t);
            uint32_t *ptr = (uint32_t*)mtp->buffer;
            int i;
            for (i = 0; i < available; i++, ptr++)
            {
                *ptr = mtp->storage.api->find_next(mtp->storage.api_arg);
                if (*ptr == 0)
                    break;
            }
            cntr_length = i * sizeof(uint32_t);
            mtp->transaction.sent += cntr_length;
        }
    }
    else if (mtp->transaction.opcode == MTP_OPERATION_GET_OBJECT)
    {
        if (mtp->transaction.sent < mtp->transaction.total)
        {
            cntr_length = mtp->storage.api->read(mtp->storage.api_arg,
                           mtp->buffer,
                           mtp->buf_size);
            mtp->transaction.sent += cntr_length;

            log_info("DT+> %s: +%d", dbg_operation(mtp->transaction.opcode), cntr_length);
        } else if (mtp->transaction.sent && mtp->transaction.sent >= mtp->transaction.total)
        {
            mtp->storage.api->close(mtp->storage.api_arg);
            mtp->transaction.file_open = false;
        }
    }
    return cntr_length;
}

bool mtp_responder_data_transaction_open(mtp_responder_t *mtp)
{
    return (mtp->transaction.received) > 0 && (mtp->transaction.received < mtp->transaction.total);
}

uint16_t mtp_responder_cancel_data_transaction(mtp_responder_t *mtp)
{
    if (mtp->transaction.opcode == MTP_OPERATION_SEND_OBJECT) {
        mtp->storage.api->close(mtp->storage.api_arg);
        mtp->transaction.file_open = false;
        mtp->storage.api->remove(mtp->storage.api_arg, mtp->transaction.handle);
    } else if (mtp->transaction.opcode == MTP_OPERATION_GET_OBJECT) {
        mtp->storage.api->close(mtp->storage.api_arg);
        mtp->transaction.file_open = false;
    }
    mtp->transaction.received = 0;
    mtp->transaction.total = 0;
    mtp->transaction.handle = 0;
    mtp->transaction.keep = false;
    return MTP_RESPONSE_TRANSACTION_CANCELLED;
}

uint16_t mtp_responder_set_data(mtp_responder_t *mtp, void *incoming, size_t size)
{
    uint16_t error = 0;
    uint32_t size_left = mtp->transaction.total - mtp->transaction.received;

    // TODO: more elegant way to detect short write
    if ((size_left > 512 && size < 512))
    {
        error = MTP_RESPONSE_INCOMPLETE_TRANSFER;
        log_error("DT< %s: SHORT READ: %u", dbg_operation(mtp->transaction.opcode), size);
        goto mtp_responder_receive_data_exit;
    }

    if (mtp->storage.api->write(mtp->storage.api_arg, incoming, size) < 0)
    {
        error = MTP_RESPONSE_OBJECT_TOO_LARGE;
        mtp->storage.api->close(mtp->storage.api_arg);
        mtp->transaction.file_open = false;
        mtp->transaction.keep = false;

        log_error("DT< %s Wrrite error", dbg_operation(mtp->transaction.opcode));
        goto mtp_responder_receive_data_exit;
    }

    mtp->transaction.received += size;

    if (mtp->transaction.received >= mtp->transaction.total)
    {
        error = MTP_RESPONSE_OK;
        mtp->storage.api->close(mtp->storage.api_arg);
        mtp->transaction.file_open = false;
        mtp->transaction.keep = false;
        goto mtp_responder_receive_data_exit;
    }

    log_info("DT< %s: %d", dbg_operation(mtp->transaction.opcode), size);
mtp_responder_receive_data_exit:
    return error;
}

void mtp_responder_get_response(mtp_responder_t *mtp, uint16_t code, void *data_out, size_t *size)
{
    assert(mtp && data_out && size);

    mtp_resp_cntr_t *response = (mtp_resp_cntr_t*)data_out;
    response->header.type = MTP_CONTAINER_TYPE_RESPONSE;
    response->header.response_code = code;
    response->header.transaction_id = mtp->transaction.id;
    memset(response->parameter, 0, 5*sizeof(uint32_t));

    if (code == MTP_RESPONSE_TRANSACTION_CANCELLED)
        log_info("CANCELED TID: %x", (unsigned int) mtp->transaction.id);

    if (mtp->transaction.handle)
    {
        response->parameter[0] = mtp->storage.id;
        response->parameter[1] = 0xFFFFFFFF;
        response->parameter[2] = mtp->transaction.handle;
    }

    response->header.length = 12 + 5*sizeof(uint32_t);
    *size = response->header.length;
    log_info("RP> %s: %s (0x%x)", dbg_operation(mtp->transaction.opcode), dbg_result(code), code);
}

void mtp_responder_transaction_reset(mtp_responder_t *mtp)
{
    if (mtp->transaction.opcode == MTP_OPERATION_SEND_OBJECT) {
        mtp->storage.api->close(mtp->storage.api_arg);
        mtp->storage.api->remove(mtp->storage.api_arg, mtp->transaction.handle);
    } else if (mtp->transaction.opcode == MTP_OPERATION_GET_OBJECT) {
        mtp->storage.api->close(mtp->storage.api_arg);
    }

    mtp->transaction.keep = false;
    mtp->transaction.sent = 0;
    mtp->transaction.received = 0;
    mtp->transaction.in_buffer = 0;
    mtp->transaction.handle = 0;
    log_info("mtp_responder: reset %u", (unsigned int) mtp->transaction.id);
}

void mtp_responder_get_event(mtp_responder_t *mtp, uint16_t code, void *data_out, size_t *size)
{
    assert(mtp && data_out && size);

    mtp_resp_cntr_t *response = (mtp_resp_cntr_t*)data_out;
    response->header.type = MTP_CONTAINER_TYPE_EVENT;
    response->header.response_code = code;
    response->header.transaction_id = mtp->transaction.id;
    response->header.length = 12;
    *size = response->header.length;
    log_info("EVT> %s: %s", dbg_operation(mtp->transaction.opcode), dbg_result(code));
}
