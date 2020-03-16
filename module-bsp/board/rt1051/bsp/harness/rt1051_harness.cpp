#include "FreeRTOS.h"
#include "board.h"
#include "drivers/dma/DriverDMA.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "drivers/pll/DriverPLL.hpp"
#include "fsl_lpuart.h"
#include "fsl_lpuart_edma.h"
#include "task.h"

#include "rt1051_harness.hpp"
#include "stream_buffer.h"
#include <cstring>
#include "bsp/harness/bsp_pinmap.hpp"

static StreamBufferHandle_t uartRxStreamBuffer = nullptr;
static xQueueHandle qHandleIrq                 = NULL;

extern "C"
{
    void LPUART3_IRQHandler(void)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        uint32_t isrReg               = LPUART_GetStatusFlags(LPUART3);
        static char characterReceived = 0;
        if (qHandleIrq != NULL && uartRxStreamBuffer != NULL) {
            if (isrReg & kLPUART_RxDataRegFullFlag) {
                characterReceived = LPUART_ReadByte(LPUART3);

                if (characterReceived == STX) {
                    uint8_t stx = STX;
                    xQueueSendFromISR(qHandleIrq, &stx, &xHigherPriorityTaskWoken);
                    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
                }
                else if (characterReceived == ETX) {
                    uint8_t etx = ETX;
                    xQueueSendFromISR(qHandleIrq, &etx, &xHigherPriorityTaskWoken);
                    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
                }
                else {
                    xStreamBufferSendFromISR(
                        uartRxStreamBuffer, (void *)&characterReceived, 1, &xHigherPriorityTaskWoken);
                    // port switching ignored here... just do read when told
                }
            }
        }
        LPUART_ClearStatusFlags(LPUART3, isrReg);
    }
};

int hwInit(xQueueHandle qHandle)
{
    LOG_DEBUG("Initialize test harness!");
    const static uint32_t rxStreamBufferLength = 1024;
    uartRxStreamBuffer                         = xStreamBufferCreate(rxStreamBufferLength, 2);
    qHandleIrq                                 = qHandle;
    lpuart_config_t s_harnessConfig;
    LPUART_GetDefaultConfig(&s_harnessConfig);
    const static uint32_t baudrate = 115200;
    s_harnessConfig.baudRate_Bps   = baudrate;
    s_harnessConfig.dataBitsCount  = kLPUART_EightDataBits;
    s_harnessConfig.parityMode     = kLPUART_ParityDisabled;
    s_harnessConfig.isMsb          = false;
    s_harnessConfig.rxIdleType     = kLPUART_IdleTypeStartBit;
    s_harnessConfig.rxIdleConfig   = kLPUART_IdleCharacter1;
    s_harnessConfig.enableTx       = false;
    s_harnessConfig.enableRx       = false;

    if (LPUART_Init(LPUART3, &s_harnessConfig, GetPerphSourceClock(PerphClock_LPUART)) != kStatus_Success) {
        LOG_FATAL("Could not initialize the uart!");
        return -1;
    }

    LPUART_ClearStatusFlags(LPUART3, 0xFFFFFFFF);
    NVIC_ClearPendingIRQ(LPUART3_IRQn);
    NVIC_SetPriority(LPUART3_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(LPUART3_IRQn);

    LPUART_ClearStatusFlags(LPUART3, 0xFFFFFFFF);
    LPUART_EnableInterrupts(LPUART3, kLPUART_RxDataRegFullInterruptEnable);
    LPUART_EnableRx(LPUART3, true);
    LPUART_EnableTx(LPUART3, true);
    return 0;
}

std::string hwRead()
{
    static size_t oldsize   = 0;
    static const char *buff = nullptr;
    size_t size             = xStreamBufferBytesAvailable(uartRxStreamBuffer);
    if (size > oldsize) {
        delete buff;
        buff    = new char[size];
        oldsize = size;
    }
    std::memset((void *)buff, 0, size);
    ssize_t ret = xStreamBufferReceive(uartRxStreamBuffer, (void *)buff, size, 0);

    std::string str(buff, buff + size);
    if (ret != str.size()) {
        LOG_ERROR("read error: %d vs %d", ret, str.size());
    }
    str.shrink_to_fit();
    return str;
}

bool hwFlush()
{
    return (xStreamBufferReset(uartRxStreamBuffer) == pdPASS);
}

// TODO write/read with DMA
bool hwEmit(const std::string &str)
{
    uint8_t val = STX;
    LPUART_WriteBlocking(LPUART3, &val, 1);
    LPUART_WriteBlocking(LPUART3, (uint8_t *)str.c_str(), str.length());
    val = ETX;
    LPUART_WriteBlocking(LPUART3, &val, 1);
    return true;
}
