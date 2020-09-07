#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <stdint.h>

int put_16(uint8_t *buffer, uint16_t value)
{
    return (int)mock(buffer, value);
}

int put_32(uint8_t *buffer, uint32_t value)
{
    return (int)mock(buffer, value);
}

int put_64(uint8_t *buffer, uint64_t value)
{
    return (int)mock(buffer, value);
}

int put_array(uint8_t *buffer, const void *array, int length, int element_size)
{
    return (int)mock(buffer, array, length, element_size);
}

int put_string(uint8_t *buffer, const char *text)
{
    return (int)mock(buffer, text);
}

int get_string(uint8_t *buffer, char *text, int length)
{
    return (int)mock(buffer, text, length);
}

int put_date(uint8_t *buffer, time_t time)
{
    return (int)mock(buffer, time);
}

int get_date(uint8_t *buffer, time_t *time)
{
    return (int)mock(buffer, time);
}
