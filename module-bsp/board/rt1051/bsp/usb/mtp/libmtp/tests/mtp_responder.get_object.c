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

Describe(get_object);

BeforeEach(get_object)
{
    mtp = mtp_responder_alloc();
    mtp_responder_init(mtp);
    mtp_responder_set_data_buffer(mtp, given_data, sizeof(given_data));
    mtp_responder_set_storage(mtp, 0x00010001, &mock_api, NULL);
    given_data_size = 0xaabbccdd;
    memset(given_data, 0xaa, sizeof(given_data));
    error = 0xaa;
}

AfterEach(get_object)
{
    mtp_responder_free(mtp);
}

Ensure(get_object, returns_error_when_object_handle_is_invalid)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x10,
        0x06, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00,
    };
    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);
    assert_that(error, is_equal_to(MTP_RESPONSE_INVALID_OBJECT_HANDLE));
    assert_that(given_data_size, is_equal_to(0));
}

Ensure(get_object, returns_error_when_file_doesnt_exists)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x10,
        0x06, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x01,
    };

    expect(mock_stat,
            when(info, is_not_equal_to(NULL)),
            will_return(-1));
    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);
    assert_that(error, is_equal_to(MTP_RESPONSE_INVALID_OBJECT_HANDLE));
    assert_that(given_data_size, is_equal_to(0));
}

Ensure(get_object, returns_error_when_unable_open_the_file)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x10,
        0x06, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x01,
    };

    mtp_object_info_t dummy_file = {
      .filename = "welcome.txt",
      .created = 1580371617,
      .modified = 1580371617,
      .format_code = MTP_FORMAT_TEXT,
      .parent = 0,
      .size = 35,
    };

    expect(mock_stat,
            when(info, is_not_equal_to(NULL)),
            will_set_contents_of_parameter(info, &dummy_file, sizeof(mtp_object_info_t)),
            will_return(0));
    expect(mock_open,
            will_return(-1));
    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);
    assert_that(error, is_equal_to(MTP_RESPONSE_INVALID_OBJECT_HANDLE));
    assert_that(given_data_size, is_equal_to(0));
}

Ensure(get_object, returns_error_when_read_from_file_failed)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x10,
        0x06, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x01,
    };

    mtp_object_info_t dummy_file = {
      .filename = "welcome.txt",
      .created = 1580371617,
      .modified = 1580371617,
      .format_code = MTP_FORMAT_TEXT,
      .parent = 0,
      .size = 35,
    };

    expect(mock_stat,
            when(info, is_not_equal_to(NULL)),
            will_set_contents_of_parameter(info, &dummy_file, sizeof(mtp_object_info_t)),
            will_return(0));
    expect(mock_open,
            will_return(0));
    expect(mock_read,
            when(count, is_greater_than(35)),
            will_return(-1));
    expect(mock_close);
    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);
    assert_that(error, is_equal_to(MTP_RESPONSE_INCOMPLETE_TRANSFER));
    assert_that(given_data_size, is_equal_to(0));
}

Ensure(get_object, returns_data_when_file_content_fits_in_one_frame)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x10,
        0x06, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x01,
    };

    mtp_object_info_t dummy_file = {
      .filename = "welcome.txt",
      .created = 1580371617,
      .modified = 1580371617,
      .format_code = MTP_FORMAT_TEXT,
      .parent = 0,
      .size = 200,
    };

    expect(mock_stat,
            when(info, is_not_equal_to(NULL)),
            will_set_contents_of_parameter(info, &dummy_file, sizeof(mtp_object_info_t)),
            will_return(0));
    expect(mock_open,
            will_return(0));
    expect(mock_read,
            when(count, is_equal_to(500)),
            will_return(200));
    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    given_data_size = mtp_responder_get_data(mtp);
    assert_that(error, is_equal_to(MTP_RESPONSE_OK));
    assert_that(given_data_size, is_equal_to(212));
}

Ensure(get_object, returns_data_when_file_content_exceeds_one_frame)
{
    const uint8_t request[] = {
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x10,
        0x06, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x01,
    };

    mtp_object_info_t dummy_file = {
      .filename = "welcome.txt",
      .created = 1580371617,
      .modified = 1580371617,
      .format_code = MTP_FORMAT_TEXT,
      .parent = 0,
      .size = 1024,
    };

    expect(mock_stat,
            when(info, is_not_equal_to(NULL)),
            will_set_contents_of_parameter(info, &dummy_file, sizeof(mtp_object_info_t)),
            will_return(0));
    expect(mock_open,
            will_return(0));
    expect(mock_read,
            when(count, is_equal_to(500)),
            will_return(500));
    expect(mock_read,
            when(count, is_equal_to(512)),
            will_return(512));
    expect(mock_read,
            when(count, is_equal_to(512)),
            will_return(12));
    expect(mock_close);

    error = mtp_responder_handle_request(mtp, request, sizeof(request));
    assert_that(error, is_equal_to(MTP_RESPONSE_OK));

    given_data_size = mtp_responder_get_data(mtp);
    assert_that(given_data_size, is_equal_to(512));
    assert_that(given->header.length, is_equal_to(1024+12));
    given_data_size = mtp_responder_get_data(mtp);
    assert_that(given_data_size, is_equal_to(512));
    given_data_size = mtp_responder_get_data(mtp);
    assert_that(given_data_size, is_equal_to(12));
    given_data_size = mtp_responder_get_data(mtp);
    assert_that(given_data_size, is_equal_to(0));

}

