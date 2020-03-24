#include "../../log/log.hpp"
#include "../../segger/systemview/SEGGER/SEGGER_SYSVIEW.h"

void log_WriteToDevice(const uint8_t *pBuffer, unsigned NumBytes)
{
    SEGGER_SYSVIEW_PrintfHost((const char *)pBuffer);
}
