#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"

#include "mock_mtp_storage_api.h"

static mtp_responder_t *mtp = NULL;
static size_t given_length;
static uint16_t error;
static uint8_t given_data[512];

const uint8_t operation_request[] = {
    0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0d, 0x10,
    0xe3, 0x03, 0x00, 0x00,
};

Describe(set_object);

BeforeEach(set_object)
{
    mtp = mtp_responder_alloc();
    mtp_responder_init(mtp);
    mtp_responder_set_data_buffer(mtp, given_data, sizeof(given_data));
    mtp_responder_set_storage(mtp, 0x00010001, &mock_api, NULL);
    given_length = 0xaabbccdd;
    memset(given_data, 0xaa, sizeof(given_data));
    error = 0xaa;
}

AfterEach(set_object)
{
    mtp_responder_free(mtp);
}

Ensure(set_object, operation_fails_if_no_previous_send_object_info)
{
}

Ensure(set_object, data_fails_if_no_previous_operation)
{
    const uint8_t data_request[] = {
        0x30, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0d, 0x10,
        0xe3, 0x03, 0x00, 0x00, 0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
    };

    error = mtp_responder_handle_request(mtp, data_request, sizeof(data_request));
    assert_that(error, is_equal_to(MTP_RESPONSE_INVALID_TRANSACTION_ID));
}

Ensure(set_object, data_writes_expected_amount_of_data)
{
    const uint8_t data_request[] = {
        0x30, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0d, 0x10,
        0xe3, 0x03, 0x00, 0x00, 0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
    };
    expect(mock_open, will_return(0));
    expect(mock_write,
          will_return(35));
    expect(mock_close);

    mtp_responder_handle_request(mtp, operation_request, sizeof(operation_request));
    error = mtp_responder_handle_request(mtp, data_request, sizeof(data_request));
    assert_that(error, is_equal_to(MTP_RESPONSE_OK));
}


