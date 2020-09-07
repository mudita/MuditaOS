#include "mtp_util.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

inline int put_16(uint8_t *buffer, uint16_t value)
{
    *(uint16_t*)buffer = value;
    return sizeof(uint16_t);
}

inline int put_32(uint8_t *buffer, uint32_t value)
{
    *(uint32_t*)buffer = value;
    return sizeof(uint32_t);
}

inline int put_64(uint8_t *buffer, uint64_t value)
{
    *(uint32_t*)buffer = value;
    *(uint32_t*)(buffer+4) = (uint32_t)(value>>32);
    return sizeof(uint64_t);
}

inline int put_array(uint8_t *buffer, const void *array, int length, int element_size)
{
    if (array)
    {
        buffer += put_32(buffer, (uint32_t)length);
        memcpy(buffer, array, length*element_size);
    }
    else
    {
        length = 0;
        put_32(buffer, 0);
    }

    return sizeof(uint32_t) + length*element_size;
}

int put_string(uint8_t *buffer, const char *text)
{
    int i;
    int length;
    uint16_t *utf16;

    if (!text)
    {
        buffer[0] = 0x01;
        buffer[1] = 0;
        buffer[2] = 0;
        return 3;
    }

    length = strlen(text);
    *buffer++ = (uint8_t)length + 1;
    utf16 = (uint16_t*)buffer;

    for(i = 0; i < length; i++, utf16++)
    {
        *utf16 = (uint16_t)text[i];
    }
    *utf16 = 0;

    /* add null termination char, each char is 2 bytes, 1 byte for string length */
    return (length + 1)*sizeof(uint16_t) + 1;
}

int get_string(const uint8_t *buffer, char *text, int length)
{
    uint8_t parsed_len = *buffer;
    uint16_t *unicode = (uint16_t*)&buffer[1];
    int i;

    if (parsed_len > length)
        return -1;

    for(i = 0; i < parsed_len; i++)
    {
        text[i] = *(char*)unicode;
        unicode++;
    }

    return 1 + parsed_len*2;
}

int put_date(uint8_t *buffer, time_t time)
{
    struct tm *tms;
    char date[16];
    tms = localtime(&time);
    sprintf(date, "%04u%02u%02uT%02u%02u%02u",
            tms->tm_year + 1900,
            tms->tm_mon + 1,
            tms->tm_mday,
            tms->tm_hour,
            tms->tm_min,
            tms->tm_sec);
    return put_string(buffer, date);
}

int get_date(const uint8_t *buffer, time_t *time)
{
    struct tm tms = {0};
    int parsed_len;
    int fields;
    char date[16];
    parsed_len = get_string(buffer, date, 16);

    if (parsed_len < 0)
        return parsed_len;

    fields = sscanf(date, "%04u%02u%02uT%02u%02u%02u",
            &tms.tm_year,
            &tms.tm_mon,
            &tms.tm_mday,
            &tms.tm_hour,
            &tms.tm_min,
            &tms.tm_sec);
    if (fields != 6)
        return -1;
    tms.tm_year -= 1900;
    tms.tm_mon -= 1;
    tms.tm_isdst = 0;
    *time = mktime(&tms);
    return parsed_len;
}
