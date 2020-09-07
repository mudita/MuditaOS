#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"
#include "mtp_dataset.h"


static const mtp_device_info_t device_info =
{
    .manufacturer = "Manufacturer",
    .model = "Model",
    .version = "1.0.0",
    .serial = "10000000000000000000000000000001"
};

static uint8_t given[512];
static uint32_t given_length;

Describe(mtp_dataset);

BeforeEach(mtp_dataset)
{
}

AfterEach(mtp_dataset)
{
}


Ensure(mtp_dataset, device_info)
{
    extern const uint16_t MTP_SUPPORTED_OPERATIONS[];
    extern const uint16_t MTP_SUPPORTED_EVENTS[];
    extern const uint16_t MTP_SUPPORTED_DEVICE_PROPERTIES[];
    extern const uint16_t MTP_SUPPORTED_CAPTURE_FORMATS[];
    extern const uint16_t MTP_SUPPORTED_PLAYBACK_FORMATS[];

    expect(put_16, when(value, is_equal_to(100)), will_return(2));  /* Standard Version */
    expect(put_32, when(value, is_equal_to(6)), will_return(4)); /* MTP Vendor Extension ID */
    expect(put_16, when(value, is_equal_to(100)), will_return(2));  /* MTP Version */
    expect(put_string,
            when(text, is_equal_to_contents_of("microsoft.com: 1.0;", 15)),
            will_return(1));                                        /* MTP Extensions */
    expect(put_16, when(value, is_equal_to(0)), will_return(2));    /* Functional Mode */
    expect(put_array,
            when(array, is_equal_to(MTP_SUPPORTED_OPERATIONS)),
            when(length, is_equal_to(15)),
            when(element_size, is_equal_to(2)),
            will_return(4+15*2));                                    /* Operations Supported */
    expect(put_array,
            when(array, is_equal_to(MTP_SUPPORTED_EVENTS)),
            when(length, is_equal_to(18)),
            will_return(4+18*2));                                    /* Events Supported */
    expect(put_array,
            when(array, is_equal_to(MTP_SUPPORTED_DEVICE_PROPERTIES)),
            when(length, is_equal_to(0)),
            will_return(4+0*2));                                    /* Device Properties Supported */
    expect(put_array,
            when(array, is_equal_to(MTP_SUPPORTED_CAPTURE_FORMATS)),
            when(length, is_equal_to(3)),
            when(element_size, is_equal_to(2)),
            will_return(4+3*2));                                    /* Capture Formats */
    expect(put_array,
            when(array, is_equal_to(MTP_SUPPORTED_PLAYBACK_FORMATS)),
            when(length, is_equal_to(3)),
            when(element_size, is_equal_to(2)),
            will_return(4+3*2));                                    /* Playback Formats */
    expect(put_string,
            when(text, is_equal_to_contents_of("Manufacturer", 12)),
            will_return(1+13*2));                                   /* Manufacturer */
    expect(put_string,
            when(text, is_equal_to_contents_of("Model", 5)),
            will_return(1+6*2));                                   /* Model */
    expect(put_string,
            when(text, is_equal_to_contents_of("1.0.0", 5)),
            will_return(1+5*2));                                   /* Version */
    expect(put_string,
            when(text, is_equal_to_contents_of("10000000000000000000000000000001", 32)),
            will_return(1+33*2));                                   /* Serial Number */

    given_length = serialize_device_info(&device_info, given);

    assert_that(given_length, is_equal_to(227));
}


