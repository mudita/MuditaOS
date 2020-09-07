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

Describe(mtp_responder_handle_request);

BeforeEach(mtp_responder_handle_request)
{
    mtp = mtp_responder_alloc();
    mtp_responder_init(mtp);
    mtp_responder_set_data_buffer(mtp, given_data, sizeof(given_data));
    mtp_responder_set_storage(mtp, 0x00010001, &mock_api, NULL);
    given_data_size = 0xaabbccdd;
    memset(given_data, 0xaa, sizeof(given_data));
    error = 0xaa;
}

AfterEach(mtp_responder_handle_request)
{
    mtp_responder_free(mtp);
}

Ensure(mtp_responder_handle_request, returns_proper_response)
{
    const uint8_t expected_response[] = {
        0x0c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x20,
        0x00, 0x00, 0x00, 0x00
    };
    error = MTP_RESPONSE_OK;
    mtp_responder_get_response(mtp, error, given_data, &given_data_size);
    assert_that(given_data_size, is_equal_to(sizeof(expected_response)));
    assert_that(given_data, is_equal_to_contents_of(expected_response, sizeof(expected_response)));
}

Ensure(mtp_responder_handle_request, open_session)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x10,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
    };

    expect(mtp_container_get_param_count, will_return(1));

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_OK));
    assert_that(given_data_size, is_equal_to(0));
}

Ensure(mtp_responder_handle_request, get_device_info)
{
    const uint8_t request[] = {
        0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x10,
        0x01, 0x00, 0x00, 0x30
    };

    const uint32_t expected_length = 185;

    expect(serialize_device_info,
            when(data, is_equal_to(&given_data[MTP_CONTAINER_HEADER_SIZE])),
            will_return(173));

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_OK));
    assert_that(given_data_size, is_equal_to(expected_length));
    assert_that(given->header.type, is_equal_to(MTP_CONTAINER_TYPE_DATA));
    assert_that(given->header.operation_code, is_equal_to(MTP_OPERATION_GET_DEVICE_INFO));
    assert_that(given->header.transaction_id, is_equal_to(0x30000001));
    assert_that(given->header.length, is_equal_to(expected_length));
}

Ensure(mtp_responder_handle_request, get_storage_ids)
{
    const uint8_t request[] = {
        0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x10,
        0x01, 0x00, 0x00, 0x30
    };

    const int TEST_NUM = CONFIG_MTP_STORAGE_NUM;
    const uint32_t expected_length = 20;

    expect(serialize_storage_ids,
            when(data, is_equal_to(&given_data[MTP_CONTAINER_HEADER_SIZE])),
            will_return(4+TEST_NUM*4));

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_OK));
    assert_that(given_data_size, is_equal_to(expected_length));
    assert_that(given->header.type, is_equal_to(MTP_CONTAINER_TYPE_DATA));
    assert_that(given->header.operation_code, is_equal_to(MTP_OPERATION_GET_STORAGE_IDS));
    assert_that(given->header.transaction_id, is_equal_to(0x30000001));
    assert_that(given->header.length, is_equal_to(expected_length));
}


Ensure(mtp_responder_handle_request, get_storage_info)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x05, 0x10,
        0x06, 0x00, 0x00, 0x30, 0x01, 0x00, 0x01, 0x00,
    };

    const uint32_t expected_length = 58;

    expect(serialize_storage_info,
            when(data, is_equal_to(&given_data[MTP_CONTAINER_HEADER_SIZE])),
            will_return(46));

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);

    assert_that(error, is_equal_to(MTP_RESPONSE_OK));
    assert_that(given_data_size, is_equal_to(expected_length));
    assert_that(given->header.type, is_equal_to(MTP_CONTAINER_TYPE_DATA));
    assert_that(given->header.operation_code, is_equal_to(MTP_OPERATION_GET_STORAGE_INFO));
    assert_that(given->header.transaction_id, is_equal_to(0x30000006));
    assert_that(given->header.length, is_equal_to(expected_length));
}


