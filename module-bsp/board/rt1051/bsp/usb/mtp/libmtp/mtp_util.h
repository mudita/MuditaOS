#ifndef _MTP_UTIL_H
#define _MTP_UTIL_H

#include <time.h>
#include <stdint.h>

int put_16(uint8_t *buffer, uint16_t value);
int put_32(uint8_t *buffer, uint32_t value);
int put_64(uint8_t *buffer, uint64_t value);
int put_array(uint8_t *buffer, const void *array, int length, int element_size);
int put_string(uint8_t *buffer, const char *text);
int get_string(const uint8_t *buffer, char *text, int length);
int put_date(uint8_t *buffer, time_t time);
int get_date(const uint8_t *buffer, time_t *time);

#endif /* _MTP_UTIL_H */

