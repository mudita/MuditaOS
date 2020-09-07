#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"

Describe(deser);

BeforeEach(deser)
{
}

AfterEach(deser)
{
}

Ensure(deser, object_info)
{
    time_t zero_value = 0;
    mtp_object_info_t given;
    int error;
    const uint8_t dataset[] = {
        0x01, 0x00, 0x01, 0x00,
        0x04, 0x30,
        0x00, 0x00,
        0x00, 0x00, 0x10, 0x00,
        0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0xff, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x0c, 0x77, 0x00, 0x65, 0x00, 0x6c, 0x00, 0x63,
        0x00, 0x6f, 0x00, 0x6d, 0x00, 0x65, 0x00, 0x2e,
        0x00, 0x74, 0x00, 0x78, 0x00, 0x74, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };
    memset(&given, 0x55, sizeof(given));

    expect(get_string,
            when(buffer, is_equal_to(&dataset[52])),
            will_set_contents_of_parameter(text, "welcome.txt", sizeof("welcome.txt")),
            will_return(1 + 2*0x0c));

    expect(get_date,
            when(buffer, is_equal_to(&dataset[77])),
            will_set_contents_of_parameter(time, &zero_value, sizeof(time_t)),
            will_return(1));

    expect(get_date,
            when(buffer, is_equal_to(&dataset[78])),
            will_set_contents_of_parameter(time, &zero_value, sizeof(time_t)),
            will_return(1));

    error = deserialize_object_info(dataset, sizeof(dataset), &given);
    assert_that(error, is_equal_to(0));
    assert_that(given.storage_id, is_equal_to(0x00010001));
    assert_that(given.format_code, is_equal_to(0x3004));
    assert_that(given.protection, is_equal_to(0));
    assert_that(given.size, is_equal_to(1024*1024));
    assert_that(given.parent, is_equal_to(0xFFFFFFFF));
    assert_that(given.association_type, is_equal_to(0x8080));
    assert_that(given.association_desc, is_equal_to(0x80808080));
    assert_that(given.filename, is_equal_to_contents_of("welcome.txt", 12));
    assert_that(given.created, is_equal_to(0));
    assert_that(given.modified, is_equal_to(0));
}

