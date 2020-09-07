#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include "mtp_responder.h"
#include "mtp_storage.h"

bool is_format_code_supported(uint16_t format_code)
{
    return (bool)mock(format_code);
}

uint32_t serialize_device_info(const mtp_device_info_t *info, uint8_t *data)
{
    return (uint32_t)mock(info, data);
}
