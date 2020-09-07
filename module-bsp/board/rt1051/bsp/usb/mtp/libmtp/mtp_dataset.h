#ifndef _MTP_DATASET_H
#define _MTP_DATASET_H

#include <stdint.h>
#include "defines.h"

typedef struct {
    const char *manufacturer;
    const char *model;
    const char *version;
    const char *serial;
} mtp_device_info_t;

bool is_format_code_supported(uint16_t format_code);
uint32_t serialize_device_info(const mtp_device_info_t *info, uint8_t *data);

#endif /* _MTP_DATASET_H */

