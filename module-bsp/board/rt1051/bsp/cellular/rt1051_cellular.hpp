// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

        void SelectAntenna(bsp::cellular::antenna antenna) override final;
        bsp::cellular::antenna GetAntenna() override final;

        static lpuart_edma_handle_t uartDmaHandle;

        static void FinishReceive();

        static bool RestartReceivingManually;

      private:
        void MSPInit();

        void MSPDeinit();

        void DMAInit();

        void DMADeinit();

      public:
        static constexpr uint32_t startIRQMaskEnable =
            kLPUART_RxActiveEdgeInterruptEnable | kLPUART_RxDataRegFullInterruptEnable;
        static constexpr uint32_t finishIRQMaskEnable = kLPUART_IdleLineInterruptEnable;

        static constexpr uint32_t startIRQMask  = kLPUART_RxActiveEdgeFlag | kLPUART_RxDataRegFullFlag;
        static constexpr uint32_t finishIRQMask = kLPUART_IdleLineFlag;

      private:
        static inline void EnableRx(uint32_t irqMask = startIRQMaskEnable | finishIRQMaskEnable)
        {
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
            LPUART_EnableInterrupts(CELLULAR_UART_BASE, irqMask);
            LPUART_EnableRx(CELLULAR_UART_BASE, true);
        }

        static inline void DisableRx(uint32_t irqMask = startIRQMaskEnable | finishIRQMaskEnable)
        {
            LPUART_DisableInterrupts(CELLULAR_UART_BASE, irqMask);
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, irqMask);
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
        std::unique_ptr<drivers::DriverDMAHandle> rxDMAHandle;

        static void uartDMACallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);

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

      public:
        static constexpr size_t RXdmaBufferSize = 16;
        static uint8_t RXdmaBuffer[RXdmaBufferSize];
        static ssize_t RXdmaReceivedCount;
        static size_t RXdmaMaxReceivedCount;
        static bool MoveRxDMAtoStreamBuf(size_t nbytes);
        static size_t GetFreeStreamBufferSize();
        static bool StartReceive(size_t nbytes);

        static TaskHandle_t untilReceivedNewHandle;
    };

} // namespace bsp

#endif // PUREPHONE_RT1501_CELLULAR_HPP
