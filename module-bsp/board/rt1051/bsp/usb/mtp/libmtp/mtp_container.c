#include "mtp_container.h"

static const char *c_type[] =
{
    "undefined",
    "command",
    "data",
    "response",
    "event",
};

const char *str_type(uint16_t id)
{
    if (id > MTP_CONTAINER_TYPE_EVENT)
        return "unknown type";
    return c_type[id];
}

const char *str_op_type(uint16_t opcode)
{
    if (opcode & 0x1000) // 0x9000
        return "operation";
    if (opcode & 0x2000)
        return "response";
    if (opcode & 0x4000)
        return "event";
    return "unknown opcode";
}

int mtp_container_get_param_count(const mtp_op_cntr_t* c)
{
    int count = (c->header.length - MTP_CONTAINER_HEADER_SIZE) / sizeof(uint32_t);
    if (count > 0 && count < 5)
        return count;
    return 0;
}

