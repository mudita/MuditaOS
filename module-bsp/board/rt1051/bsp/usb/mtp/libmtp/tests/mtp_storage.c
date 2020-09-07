#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_responder.h"
#include "mtp_container.h"
#include "mtp_storage.h"
#include "mtp_util.h"

#include "mock_mtp_storage_api.h"

static mtp_storage_t storage;
static uint8_t given[512];
static uint32_t given_length;

static const mtp_storage_properties_t props =
{
    .type = MTP_STORAGE_FIXED_ROM,
    .fs_type = MTP_STORAGE_FILESYSTEM_FLAT,
    .access_caps = 0xaabb,
    .capacity = 0x55aadeadbeefaa55,
    .description = "Volume 1",
    .volume_id = NULL
};

Describe(mtp_storage);

BeforeEach(mtp_storage)
{
    storage.id = 0xabcdbeef;
    storage.api = &mock_api;
}

AfterEach(mtp_storage)
{
}

Ensure(mtp_storage, list_serialize_empty_array_if_no_files)
{
    uint8_t expected[] = { 0x00, 0x00, 0x00, 0x00 };
    expect(mock_find_first,
            when(parent, is_equal_to(0)),
            will_return(0));
    given_length = serialize_storage_list(&storage, 0, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_storage, list_serialize_exactly_into_array)
{
    uint8_t expected[] = { 0x02, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x81,
                           0x00, 0x00, 0x00, 0x82};
    uint32_t COUNT = 2;
    expect(mock_find_first,
            when(parent, is_equal_to(0)),
            will_set_contents_of_parameter(count, &COUNT, sizeof(uint32_t)),
            will_return(0x81000000));
    expect(mock_find_next,
            will_return(0x82000000));
    expect(mock_find_next,
            will_return(0));

    given_length = serialize_storage_list(&storage, 0, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_storage, storage_info)
{
    expect(mock_get_properties,
            will_return(&props));

    expect(mock_free_space,
            will_return(1024));

    expect(put_16, when(value, is_equal_to(MTP_STORAGE_FIXED_ROM)), will_return(2));          /* Storage Type */
    expect(put_16, when(value, is_equal_to(MTP_STORAGE_FILESYSTEM_FLAT)), will_return(2));    /* Filesystem Type */
    expect(put_16, when(value, is_equal_to(0xaabb)), will_return(2));                         /* Access Capability */
    expect(put_64, when(value, is_equal_to(0x55aadeadbeefaa55)), will_return(8));             /* Max Capacity, can't be 0! */
    expect(put_64, when(value, is_equal_to(1024)), will_return(8));                           /* Free Space In Bytes */
    expect(put_32, when(value, is_equal_to(0xFFFFFFFF)), will_return(4));                     /* Free Space In Objects */
    expect(put_string,
            when(text, is_equal_to_contents_of("Volume 1", 8)),
            will_return(1+9*2));                                                              /* Storage Description */
    expect(put_string,
            when(text, is_equal_to(NULL)),
            will_return(1));                                                                  /* Volume Identifier */

    given_length = serialize_storage_info(&storage, given);
    assert_that(given_length, is_equal_to(46));
}

Ensure(mtp_storage, storage_ids)
{
    const uint8_t expected[] = {
        0x01, 0x00, 0x00, 0x00,
        0xef, 0xbe, 0xcd, 0xab
    };

    given_length = serialize_storage_ids(&storage, 1, given);
    assert_that(given_length, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_storage, object_info)
{
    mtp_object_info_t info = {
        .storage_id = storage.id,
        .format_code = 0x8001,
        .protection = 0xFFFF,
        .parent = 0xE0000003,
        .association_type = 0xE003,
        .association_desc = 0xC0000007,
        .filename = "file.name.txt",
        .created = 1580322989,
        .modified = 1580323100,
        .size = 0xabcdbeef,
    };

    /* YYYYMMDDThhmmss */
    expect(put_32, when(value, is_equal_to(storage.id)), will_return(4)); /* Storage ID*/
    expect(put_16, when(value, is_equal_to(0x8001)), will_return(2));     /* Object Format */
    expect(put_16, when(value, is_equal_to(0xFFFF)), will_return(2));     /* Protection Status*/
    expect(put_32, when(value, is_equal_to(0xabcdbeef)), will_return(4)); /* Object Compressed Size*/
    expect(put_16, when(value, is_equal_to(0)), will_return(2));          /* Thumb Format */
    expect(put_32, when(value, is_equal_to(0)), will_return(4));          /* Thumb Compressed Size */
    expect(put_32, when(value, is_equal_to(0)), will_return(4));          /* Thumb Pix Width */
    expect(put_32, when(value, is_equal_to(0)), will_return(4));          /* Thumb Pix Height */
    expect(put_32, when(value, is_equal_to(0)), will_return(4));          /* Image Pix Width */
    expect(put_32, when(value, is_equal_to(0)), will_return(4));          /* Image Pix Height */
    expect(put_32, when(value, is_equal_to(0)), will_return(4));          /* Image Bit Depth */
    expect(put_32, when(value, is_equal_to(0xE0000003)), will_return(4)); /* Parent Object */
    expect(put_16, when(value, is_equal_to(0xE003)), will_return(2));     /* Association Type */
    expect(put_32, when(value, is_equal_to(0xC0000007)), will_return(4)); /* Association Descritpion */
    expect(put_32, when(value, is_equal_to(0)), will_return(4));          /* Sequence Number */
    expect(put_string,
            when(text, is_equal_to_contents_of("file.name.txt", 13)),
            will_return(1+14*2));                                         /* Filename */
    expect(put_date,
            when(time, is_equal_to(1580322989)),
            will_return(1+16*2));                                         /* Date created */
    expect(put_date,
            when(time, is_equal_to(1580323100)),
            will_return(1+16*2));                                         /* Date modified */
    expect(put_string,
            when(text, is_equal_to(NULL)),
            will_return(3));

    given_length = serialize_object_info(&info, given);
    assert_that(given_length, is_equal_to(150));
}

Ensure(mtp_storage, serialize_supported_object_properties)
{
     uint16_t expected_list[] = {
        MTP_PROPERTY_STORAGE_ID,
        MTP_PROPERTY_OBJECT_FORMAT,
        MTP_PROPERTY_OBJECT_SIZE,
        MTP_PROPERTY_PERSISTENT_UID,
        MTP_PROPERTY_NAME,
        MTP_PROPERTY_DATE_CREATED,
        MTP_PROPERTY_DATE_MODIFIED,
        MTP_PROPERTY_PARENT_OBJECT,
        MTP_PROPERTY_OBJECT_FILE_NAME,
    };
    const int number_items = sizeof(expected_list)/sizeof(uint16_t);

    given_length = serialize_object_props_supported(given);
    assert_that(given_length, is_equal_to(4+sizeof(expected_list)));
    assert_that(*(uint32_t*)given, is_equal_to(number_items));
    assert_that(&given[4], is_equal_to_contents_of(expected_list, number_items));
}
