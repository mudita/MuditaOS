
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

#include "bsp/BoardDefinitions.hpp"

#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#include "drivers/gpio/DriverGPIO.hpp"

namespace bsp
{

    class RT1051Cellular : public Cellular
    {
      private:
        bool pv_SendingAllowed = true;

      public:
        RT1051Cellular();

        ~RT1051Cellular();

        void PowerUp() override final;

        void PowerDown() override final;

        void Restart() override final;

        uint32_t Wait(uint32_t timeout) override final;

        ssize_t Read(void *buf, size_t nbytes) override final;

        ssize_t Write(void *buf, size_t nbytes) override final;

        void InformModemHostAsleep() override final;

        void InformModemHostWakeup() override final;

        void EnterSleep() override final;

        void ExitSleep() override final;

        void SetSpeed(uint32_t portSpeed) override final;

        void SetSendingAllowed(bool state) override final;
        bool GetSendingAllowed() override final;

        static StreamBufferHandle_t uartRxStreamBuffer;
        static TimerHandle_t rxTimeoutTimer;

        void SelectAntenna(uint8_t antenna) override final;

      private:
        void MSPInit();

        void MSPDeinit();

        void DMAInit();

        void DMADeinit();

        inline void EnableRx()
        {
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
            LPUART_EnableInterrupts(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
            LPUART_EnableRx(CELLULAR_UART_BASE, true);
        }

        inline void DisableRx()
        {
            LPUART_DisableInterrupts(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
            LPUART_EnableRx(CELLULAR_UART_BASE, false);
        }

        inline void EnableTx()
        {
            LPUART_EnableTx(CELLULAR_UART_BASE, true);
        }

        inline void DisableTx()
        {
            LPUART_EnableTx(CELLULAR_UART_BASE, false);
        }

        // M.P: It is important to destroy these drivers in specific order
        std::shared_ptr<drivers::DriverPLL> pll;
        std::shared_ptr<drivers::DriverGPIO> gpio_1;
        std::shared_ptr<drivers::DriverGPIO> gpio_2;
        std::shared_ptr<drivers::DriverGPIO> gpio_3;
        std::shared_ptr<drivers::DriverDMAMux> dmamux;
        std::shared_ptr<drivers::DriverDMA> dma;
        std::unique_ptr<drivers::DriverDMAHandle> txDMAHandle;

        static lpuart_edma_handle_t uartDmaHandle;

        static void DMATxCompletedCb(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);

      public:
        static TaskHandle_t blockedTaskHandle;

      private:
        // Constants
        const static uint32_t baudrate                               = 115200;
        const static uint32_t rxStreamBufferLength                   = 1024;
        const static uint32_t rxStreamBufferNotifyWatermark          = 1;
        const static uint32_t CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE = 1;
        const static uint32_t CELLULAR_BSP_ANTSEL_PIN_A_STATE        = 0;
        const static uint32_t CELLULAR_BSP_ANTSEL_PIN_B_STATE        = 1;
    };

} // namespace bsp

#endif // PUREPHONE_RT1501_CELLULAR_HPP
