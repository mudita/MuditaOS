#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"

#include "mock_mtp_storage_api.h"

static mtp_responder_t *mtp = NULL;
static size_t given_data_size;
static uint16_t error;
static uint8_t given_data[512];
static const mtp_op_cntr_t *given = (mtp_op_cntr_t*)given_data;

Describe(get_object_info);

BeforeEach(get_object_info)
{
    mtp = mtp_responder_alloc();
    mtp_responder_init(mtp);
    mtp_responder_set_data_buffer(mtp, given_data, sizeof(given_data));
    mtp_responder_set_storage(mtp, 0x00010001, &mock_api, NULL);
    given_data_size = 0xaabbccdd;
    memset(given_data, 0xaa, sizeof(given_data));
    error = 0xaa;
}

AfterEach(get_object_info)
{
    mtp_responder_free(mtp);
}

Ensure(get_object_info, return_error_if_requested_wrong_handle)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x10,
        0x0F, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00
    };

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_INVALID_OBJECT_HANDLE));
    assert_that(given_data_size, is_equal_to(0));
}

Ensure(get_object_info, returns_error_code_if_storage_does_not_find_object)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x10,
        0x0F, 0x00, 0x00, 0xF0, 0x0a, 0x00, 0x00, 0x00
    };

    expect(mock_stat,
            when(handle, is_equal_to(0x0000000a)),
            will_return(-1));

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_INVALID_OBJECT_HANDLE));
    assert_that(given_data_size, is_equal_to(0));

}

Ensure(get_object_info, calls_for_serializator_and_returns_container)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x10,
        0x0F, 0x00, 0x00, 0xF0, 0x0a, 0x00, 0x00, 0x00
    };

    const uint32_t expected_length = 12 + 100;

    expect(mock_stat,
            when(handle, is_equal_to(0x0000000a)),
            will_return(0));

    expect(serialize_object_info,
            will_return(100));

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_OK));
    assert_that(given_data_size, is_equal_to(expected_length));
    assert_that(given->header.type, is_equal_to(MTP_CONTAINER_TYPE_DATA));
    assert_that(given->header.operation_code, is_equal_to(MTP_OPERATION_GET_OBJECT_INFO));
    assert_that(given->header.transaction_id, is_equal_to(0xF000000F));
    assert_that(given->header.length, is_equal_to(expected_length));
}
