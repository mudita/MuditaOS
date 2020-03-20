#include "../../segger/log/log.hpp"
#include "fsl_lpuart.h"

#define LOG_UART_DEV LPUART3

void log_WriteToDevice(const uint8_t *pBuffer, unsigned NumBytes)
{
    LPUART_WriteBlocking(LOG_UART_DEV, pBuffer, NumBytes);
}
