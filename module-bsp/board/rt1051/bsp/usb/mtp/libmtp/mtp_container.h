#ifndef _MTP_CONTAINER_H
#define _MTP_CONTAINER_H

#include <stdint.h>
#include "defines.h"

__attribute__((packed))
struct mtp_cntr_hdr
{
    uint32_t length;
    uint16_t type;
    union {
        uint16_t operation_code;
        uint16_t response_code;
        uint16_t event_code;
    };
    uint32_t transaction_id;
};

__attribute__((packed))
struct mtp_data_cntr
{
    struct mtp_cntr_hdr header;
    uint8_t payload[];
};

__attribute__((packed))
struct mtp_op_cntr
{
    struct mtp_cntr_hdr header;
    uint32_t parameter[];
};

__attribute__((packed))
struct mtp_event
{
    uint16_t code;
    uint32_t transaction_id;
};

typedef struct mtp_cntr_hdr mtp_cntr_hdr_t;
typedef struct mtp_data_cntr mtp_data_cntr_t;
typedef struct mtp_op_cntr mtp_op_cntr_t;
typedef struct mtp_op_cntr mtp_resp_cntr_t;
typedef struct mtp_event mtp_event_t;

int mtp_container_get_param_count(const mtp_op_cntr_t *c);

#endif /* _MTP_CONTAINER_H */

