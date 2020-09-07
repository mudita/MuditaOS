#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"

#include "mock_mtp_storage_api.h"

static mtp_responder_t *mtp = NULL;
static uint16_t error;
static uint8_t given_data[512];
static size_t given_data_size;
static const mtp_op_cntr_t *given = (mtp_op_cntr_t*)given_data;

Describe(get_object_handles);

BeforeEach(get_object_handles)
{
    mtp = mtp_responder_alloc();
    mtp_responder_init(mtp);
    mtp_responder_set_data_buffer(mtp, given_data, sizeof(given_data));
    given_data_size = 0xaabbccdd;
    memset(given_data, 0xaa, sizeof(given_data));
    error = 0xaa;
}

AfterEach(get_object_handles)
{
    mtp_responder_free(mtp);
}

Ensure(get_object_handles, returns_error_when_format_code_specified)
{
    const uint8_t request[] = {
        0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x07, 0x10,
        0x01, 0x00, 0x00, 0x30, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    };

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_SPECIFICATION_BY_FORMAT_UNSUPPORTED));
    assert_that(given_data_size, is_equal_to(0));
}

Ensure(get_object_handles, returns_error_when_no_storages_defined)
{
    const uint8_t request[] = {
        0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x07, 0x10,
        0x01, 0x00, 0x00, 0x30, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    };

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_STORE_NOT_AVAILABLE));
    assert_that(given_data_size, is_equal_to(0));
}

static void test_return_list_for(const uint8_t *request, size_t len)
{
    /*124 max in one frame */
    const uint32_t COUNT = 127;
    const uint32_t expected_length = 524;
    int i;
    expect(mock_find_first,
            when(parent, is_equal_to(0xFFFFFFFF)),
            will_set_contents_of_parameter(count, &COUNT, sizeof(uint32_t)),
            will_return(0x0000abcd));

    for(i = 0; i < COUNT; i++)
        expect(mock_find_next,
               will_return(i+1));

    expect(mock_find_next,
            will_return(0));

    mtp_responder_set_storage(mtp, 0x00010001, &mock_api, NULL);
    error = mtp_responder_handle_request(mtp, request, len);
    assert_that(error, is_equal_to(MTP_RESPONSE_OK));

    given_data_size = mtp_responder_get_data(mtp);
    assert_that(given->header.type, is_equal_to(MTP_CONTAINER_TYPE_DATA));
    assert_that(given->header.operation_code, is_equal_to(MTP_OPERATION_GET_OBJECT_HANDLES));
    assert_that(given->header.transaction_id, is_equal_to(0x30000001));
    assert_that(given->header.length, is_equal_to(expected_length));
    assert_that(given_data_size, is_equal_to(512));

    given_data_size = mtp_responder_get_data(mtp);
    assert_that(given_data_size, is_equal_to(12));
    uint32_t* given_list = (uint32_t*)given_data;
    assert_that(given_list[0], is_equal_to(125));
}

Ensure(get_object_handles, return_list_from_specific_storage)
{
    const uint8_t request[] = {
        0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x07, 0x10,
        0x01, 0x00, 0x00, 0x30, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    };

    test_return_list_for(request, sizeof(request));
}

Ensure(get_object_handles, returns_for_all_storages)
{
    const uint8_t request[] = {
        0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x07, 0x10,
        0x01, 0x00, 0x00, 0x30, 0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    };

    test_return_list_for(request, sizeof(request));
}

Ensure(get_object_handles, return_error_when_unknown_storage_id)
{
    const uint8_t request[] = {
        0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x07, 0x10,
        0x01, 0x00, 0x00, 0x30, 0x01, 0x02, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    };

    mtp_responder_set_storage(mtp, 0x00010001, &mock_api, NULL);
    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_INVALID_STORAGE_ID));
    assert_that(given_data_size, is_equal_to(0));

}

