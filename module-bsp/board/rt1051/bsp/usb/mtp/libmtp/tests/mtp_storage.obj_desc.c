#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"

static uint8_t given[512];
static uint32_t given_length;

Describe(obj_prop_desc);

BeforeEach(obj_prop_desc)
{
}

AfterEach(obj_prop_desc)
{
}

Ensure(obj_prop_desc, storage_id_property)
{
    uint8_t expected[] = {
        0x01, 0xDC,
        0x06, 0x00,
        0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_STORAGE_ID, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(obj_prop_desc, object_format_code_property)
{
    uint8_t expected[] = {
        0x02, 0xDC,
        0x04, 0x00,
        0x00,
        0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_OBJECT_FORMAT, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(obj_prop_desc, object_size_property)
{
    uint8_t expected[] = {
        0x04, 0xDC,
        0x08, 0x00,
        0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_OBJECT_SIZE, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(obj_prop_desc, object_file_name_property)
{
    uint8_t expected[] = {
        0x07, 0xDC,
        0xFF, 0xFF,
        0x01,
        0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_OBJECT_FILE_NAME, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(obj_prop_desc, object_date_created_property)
{
    uint8_t expected[] = {
        0x08, 0xDC,
        0xFF, 0xFF,
        0x00,
        0x00,
        0x00, 0x00, 0x00, 0x00,
        0x03
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_DATE_CREATED, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(obj_prop_desc, object_date_modified_property)
{
    uint8_t expected[] = {
        0x09, 0xDC,
        0xFF, 0xFF,
        0x01,
        0x00,
        0x00, 0x00, 0x00, 0x00,
        0x03
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_DATE_MODIFIED, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(obj_prop_desc, object_parent_property)
{
    uint8_t expected[] = {
        0x0B, 0xDC,
        0x06, 0x00,
        0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_PARENT_OBJECT, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(obj_prop_desc, object_uid)
{
    uint8_t expected[] = {
        0x41, 0xDC,
        0x0A, 0x00,
        0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_PERSISTENT_UID, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(obj_prop_desc, object_name_property)
{
    uint8_t expected[] = {
        0x44, 0xDC,
        0xFF, 0xFF,
        0x01,
        0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00
    };
    given_length = serialize_object_prop_desc(MTP_PROPERTY_NAME, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

