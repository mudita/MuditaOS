#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"

static uint8_t given[512];
static uint32_t given_length;
static mtp_object_info_t info = {
        .storage_id = 0xdeadbeef,
        .format_code = 0x8001,
        .protection = 0xFFFF,
        .parent = 0xE0000003,
        .association_type = 0xE003,
        .association_desc = 0xC0000007,
        .filename = "file",
        .created = 1580322989,
        .modified = 1580323100,
        .size = 0xabcdbeef,
        .uuid = {0x01, 0x02, 0x03, 0x04,
                 0x01, 0x02, 0x03, 0x04,
                 0x01, 0x02, 0x03, 0x04,
                 0x01, 0x02, 0x03, 0x04},
    };

Describe(get_obj_prop_value);

BeforeEach(get_obj_prop_value)
{
}

AfterEach(get_obj_prop_value)
{
}

Ensure(get_obj_prop_value, storage_id)
{
    uint8_t expected[] = {
        0xef, 0xbe, 0xad, 0xde,
    };
    given_length = serialize_object_prop_value(MTP_PROPERTY_STORAGE_ID, &info, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(get_obj_prop_value, format_code)
{
    uint8_t expected[] = {
        0x01, 0x80,
    };
    given_length = serialize_object_prop_value(MTP_PROPERTY_OBJECT_FORMAT, &info, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(get_obj_prop_value, size)
{
    uint8_t expected[] = {
        0xef, 0xbe, 0xcd, 0xab, 0x00, 0x00, 0x00, 0x00,
    };
    given_length = serialize_object_prop_value(MTP_PROPERTY_OBJECT_SIZE, &info, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(get_obj_prop_value, uid)
{
    uint8_t expected[] = {
        0x01, 0x02, 0x03, 0x04,
        0x01, 0x02, 0x03, 0x04,
        0x01, 0x02, 0x03, 0x04,
        0x01, 0x02, 0x03, 0x04
    };
    given_length = serialize_object_prop_value(MTP_PROPERTY_PERSISTENT_UID, &info, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(get_obj_prop_value, name)
{
    expect(put_string,
            when(text, is_equal_to_contents_of("file", 5)),
            will_return(5));
    given_length = serialize_object_prop_value(MTP_PROPERTY_NAME, &info, given);
    assert_that(given_length, is_equal_to(5));
}

Ensure(get_obj_prop_value, date_created)
{
    expect(put_date,
            when(time, is_equal_to(1580322989)),
            will_return(11));
    given_length = serialize_object_prop_value(MTP_PROPERTY_DATE_CREATED, &info, given);
    assert_that(given_length, is_equal_to(11));
}

Ensure(get_obj_prop_value, date_modified)
{
    expect(put_date,
            when(time, is_equal_to(1580323100)),
            will_return(11));
    given_length = serialize_object_prop_value(MTP_PROPERTY_DATE_MODIFIED, &info, given);
    assert_that(given_length, is_equal_to(11));
}

Ensure(get_obj_prop_value, parent)
{
    uint8_t expected[] = {
        0x03, 0x00, 0x00, 0xE0
    };
    given_length = serialize_object_prop_value(MTP_PROPERTY_PARENT_OBJECT, &info, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));

}

Ensure(get_obj_prop_value, file_name)
{
    expect(put_string,
            when(text, is_equal_to_contents_of("file", 5)),
            will_return(5));
    given_length = serialize_object_prop_value(MTP_PROPERTY_OBJECT_FILE_NAME, &info, given);
    assert_that(given_length, is_equal_to(5));
}

