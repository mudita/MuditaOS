#include "../../segger/log/log.hpp"
#include "../../segger/log/rtt/SEGGER_RTT.h"

#define LOG_RTT_BUFF_INDEX 0

void log_WriteToDevice(const uint8_t *pBuffer, unsigned NumBytes)
{
    SEGGER_RTT_Write(LOG_RTT_BUFF_INDEX, pBuffer, NumBytes);
}
