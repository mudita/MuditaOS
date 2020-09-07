#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "mtp_util.h"

Describe(mtp_util);

BeforeEach(mtp_util)
{
    setenv("TZ", "UTC", 1);
}

AfterEach(mtp_util)
{
}

Ensure(mtp_util, puts_16_bit_value)
{
    const uint8_t expected[4] = { 0x00, 0xcd, 0xab, 0x00 };
    uint8_t given[4] = {0};
    int given_len = -1;
    given_len = put_16(&given[1],0xabcd);
    assert_that(given_len, is_equal_to(2));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, puts_32_bit_value)
{
    const uint8_t expected[6] = { 0x00, 0xef, 0xbe, 0xcd, 0xab, 0x00 };
    uint8_t given[6] = {0};
    int given_len = -1;
    given_len = put_32(&given[1],0xabcdbeef);
    assert_that(given_len, is_equal_to(4));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, puts_64_bit_value)
{
    const uint8_t expected[10] = { 0x00, 0xbb, 0xaa, 0xef, 0xbe, 0xad, 0xde, 0xcd, 0xab, 0x00 };
    uint8_t given[sizeof(expected)] = {0};
    int given_len = -1;
    given_len = put_64(&given[1], 0xabcddeadbeefaabb);
    assert_that(given_len, is_equal_to(8));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, puts_array_values_with_length_for_uint16)
{
    const uint8_t expected[] = { 0x03, 0x00, 0x00, 0x00, 0xef, 0xbe, 0xcd, 0xab, 0xaa, 0x55 };
    const uint16_t array[] = {0xbeef, 0xabcd, 0x55aa };
    uint8_t given[10] = {0};
    int given_len = -1;
    given_len = put_array(given, array, sizeof(array)/sizeof(uint16_t), sizeof(uint16_t));
    assert_that(given_len, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, puts_array_values_with_length_for_uint32)
{
    const uint8_t expected[] = { 0x02, 0x00, 0x00, 0x00, 0xcd, 0xab, 0xef, 0xbe, 0x88, 0x77, 0x66, 0x55 };
    const uint32_t array[] = { 0xbeefabcd, 0x55667788 };
    uint8_t given[16] = {0};
    int given_len = -1;
    given_len = put_array(given, array, sizeof(array)/sizeof(uint32_t), sizeof(uint32_t));
    assert_that(given_len, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}


Ensure(mtp_util, puts_data_for_zero_length_array)
{
    const uint8_t expected[] = { 0x00, 0x00, 0x00, 0x00 };
    const uint16_t array[] = { 1, 2, 3};
    uint8_t given[10] = {0};
    int given_len = -1;
    given_len = put_array(given, array, 0, 0);
    assert_that(given_len, is_equal_to(4));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, puts_data_for_null_array)
{
    const uint8_t expected[] = { 0x00, 0x00, 0x00, 0x00 };
    uint8_t given[10] = {0};
    int given_len = -1;
    given_len = put_array(given, NULL, 123, 0);
    assert_that(given_len, is_equal_to(4));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, puts_string_as_utf16_with_length_and_null_char_at_the_end)
{
    const uint8_t expected[] = {5, 't', 0x00, 'e', 0x00, 's', 0x00, 't', 0x00, 0x00, 0x00};
    uint8_t given[16];
    int given_len = -1;
    memset(given, 0xaa, sizeof(given));
    given_len = put_string(given, "test");
    assert_that(given_len, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, get_string_parse_correctly)
{
    char expected[] = "test test test";
    uint8_t unicode[256];
    char given[256];
    int length;
    memset(given, 0x55, sizeof(given));
    put_string(unicode, expected);
    length = get_string(unicode, given, sizeof(given));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
    assert_that(length, is_equal_to(1 + 2*(strlen(expected) + 1)));
}

Ensure(mtp_util, puts_single_zero_if_text_is_null)
{
    const uint8_t expected[] = { 0x01, 0x00, 0x00 };
    uint8_t given[16] = {0};
    int given_len = -1;
    given_len = put_string(given, NULL);
    assert_that(given_len, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, time_func_reversible)
{
    time_t expected = 1580322989;
    time_t given = 123;
    given = mktime(localtime(&expected));
    assert_that(given, is_equal_to(expected));
}

Ensure(mtp_util, puts_date_in_iso_format)
{
    //2020 01 29 T183629
    const uint8_t expected[] = { 16,
         '2', 0x00, '0', 0x00, '2', 0x00, '0', 0x00,
         '0', 0x00, '1', 0x00, '2', 0x00, '9', 0x00,
         'T', 0x00,
         '1', 0x00, '8', 0x00, '3', 0x00, '6', 0x00, '2', 0x00, '9', 0x00,
         0x00, 0x00};
    uint8_t given[33] = {0};
    int given_len = -1;
    given_len = put_date(given, 1580322989);
    assert_that(given_len, is_equal_to(sizeof(expected)));
    assert_that(given, is_equal_to_contents_of(expected, sizeof(expected)));
}

Ensure(mtp_util, get_date_is_revers_to_put_date)
{
    time_t expected = 1580322989;
    time_t given = 123;
    uint8_t encoded[64];
    int length;
    put_date(encoded, expected);
    length = get_date(encoded, &given);
    assert_that(length, is_equal_to(1+16*2));
    assert_that(given, is_equal_to(expected));
}
