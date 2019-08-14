
/*
 * @file rt1501_cellular.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_RT1501_CELLULAR_HPP
#define PUREPHONE_RT1501_CELLULAR_HPP

#include "bsp/cellular/bsp_cellular.hpp"

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "timers.h"
#include "board.h"
#include "fsl_lpuart.h"
#include "fsl_lpuart_edma.h"

#include "drivers/DriverInterface.hpp"
#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"

namespace bsp {

    class RT1051Cellular : public Cellular {
    public:

        RT1051Cellular();

        ~RT1051Cellular();

        void PowerUp() override final;

        void PowerDown() override final;

        void Restart() override final;

        uint32_t Wait(uint32_t timeout) override final;

        ssize_t Read(void *buf, size_t nbytes) override final;

        ssize_t Write(void *buf, size_t nbytes) override final;


        static StreamBufferHandle_t uartRxStreamBuffer;
        static TimerHandle_t rxTimeoutTimer;

    private:
        void MSPInit();

        void MSPDeinit();

        void DMAInit();

        void DMADeinit();

        uint32_t UartGetPeripheralClock();

        inline void EnableRx() {
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
            LPUART_EnableInterrupts(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
            LPUART_EnableRx(CELLULAR_UART_BASE, true);
        }

        inline void DisableRx() {
            LPUART_DisableInterrupts(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
            LPUART_EnableRx(CELLULAR_UART_BASE, false);
        }

        inline void EnableTx() {
            LPUART_EnableTx(CELLULAR_UART_BASE, true);
        }

        inline void DisableTx() {
            LPUART_EnableTx(CELLULAR_UART_BASE, false);
        }

        inline void WakeupModem() {
            GPIO_PinWrite(BSP_CELLULAR_UART_DTR_PORT, BSP_CELLULAR_UART_DTR_PIN, 0);
            GPIO_PinWrite(BSP_CELLULAR_WAKEUP_PORT, BSP_CELLULAR_WAKEUP_PIN, 0);

            vTaskDelay(pdMS_TO_TICKS(15));
        }

        inline void EnableModemEnterSleepMode() {
            GPIO_PinWrite(BSP_CELLULAR_UART_DTR_PORT, BSP_CELLULAR_UART_DTR_PIN, 1);
            GPIO_PinWrite(BSP_CELLULAR_WAKEUP_PORT, BSP_CELLULAR_WAKEUP_PIN, 1);
        }

        inline void InformModemHostAsleep() {
            GPIO_PinWrite(BSP_CELLULAR_AP_RDY_PORT, BSP_CELLULAR_AP_RDY_PIN, !CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE);
        }

        inline void InformModemHostWakeup() {
            GPIO_PinWrite(BSP_CELLULAR_AP_RDY_PORT, BSP_CELLULAR_AP_RDY_PIN, CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE);
        }

        std::shared_ptr<drivers::DriverPLL> pll;
        std::shared_ptr<drivers::DriverDMA> dma;
        std::shared_ptr<drivers::DriverDMAMux> dmamux;

        static lpuart_edma_handle_t uartDmaHandle;

        static void DMATxCompletedCb(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);

        static void rxTimeoutTimerHandle(TimerHandle_t xTimer);

        static TaskHandle_t blockedTaskHandle;


        // Constants
        const static uint32_t baudrate = 115200;
        const static uint32_t rxStreamBufferLength = 1024;
        const static uint32_t rxStreamBufferNotifyWatermark = 1;
        const static uint32_t CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE = 1;


    };

}


#endif //PUREPHONE_RT1501_CELLULAR_HPP
