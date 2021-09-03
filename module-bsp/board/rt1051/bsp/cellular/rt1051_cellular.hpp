// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <FreeRTOS.h>
#include <message_buffer.h>
#include <timers.h>

#include "board.h"
#include "fsl_lpuart.h"
#include "fsl_lpuart_edma.h"

#include "bsp/cellular/bsp_cellular.hpp"
#include "bsp/cellular/CellularResult.hpp"
#include "board/BoardDefinitions.hpp"

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

        void logData(const std::string &title, uint8_t *buffer, size_t nbytes);

      public:
        RT1051Cellular();

        ~RT1051Cellular();

        void powerUp() override final;
        void powerDown() override final;
        void restart() override final;
        uint32_t wait(std::chrono::milliseconds timeoutMs) override final;
        ssize_t read(void *buffer, size_t nbytes, std::chrono::milliseconds timeoutMs) override final;
        ssize_t write(void *buf, size_t nbytes) override final;
        void informModemHostAsleep() override final;
        void informModemHostWakeup() override final;
        void enterSleep() override final;
        void exitSleep() override final;
        void setSpeed(uint32_t portSpeed) override final;
        void setSendingAllowed(bool state) override final;
        bool getSendingAllowed() const noexcept override final;
        void selectAntenna(bsp::cellular::antenna antenna) override final;
        bsp::cellular::antenna getAntenna() override final;

        static MessageBufferHandle_t uartRxBuffer;
        static lpuart_edma_handle_t uartDmaHandle;
        static bool RestartReceivingManually;
        static constexpr std::size_t RXdmaBufferSize = bsp::cellular::CellularResultStructMaxDataSize;
        static std::size_t RXdmaMaxReceivedCount;

        static constexpr std::uint32_t edgeIRQMaskEnable  = kLPUART_RxActiveEdgeInterruptEnable;
        static constexpr std::uint32_t idleIRQMaskEnable  = kLPUART_IdleLineInterruptEnable;
        static constexpr std::uint32_t edgeIRQMask        = kLPUART_RxActiveEdgeFlag;
        static constexpr std::uint32_t idleIRQMask        = kLPUART_IdleLineFlag;
        static constexpr std::uint32_t dataRegFullIRQMask = kLPUART_RxDataRegFullFlag;

        static bool sendRxDmaResult(bsp::cellular::CellularResultCode reason);
        static std::size_t getMaxBufferDataSize();
        static bool startReceive(std::size_t nbytes);

        static TaskHandle_t untilReceivedNewHandle;

      private:
        void MSPInit();
        void MSPDeinit();
        void DMAInit();
        void DMADeinit();

        static void uartDMACallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);

        static inline void enableRx(uint32_t irqMask = edgeIRQMaskEnable | idleIRQMaskEnable | dataRegFullIRQMask)
        {
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
            LPUART_EnableInterrupts(CELLULAR_UART_BASE, irqMask);
            LPUART_EnableRx(CELLULAR_UART_BASE, true);
        }

        static inline void disableRx(uint32_t irqMask = edgeIRQMaskEnable | idleIRQMaskEnable | dataRegFullIRQMask)
        {
            LPUART_DisableInterrupts(CELLULAR_UART_BASE, irqMask);
            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, irqMask);
            LPUART_EnableRx(CELLULAR_UART_BASE, false);
        }

        inline void enableTx()
        {
            LPUART_EnableTx(CELLULAR_UART_BASE, true);
        }

        inline void disableTx()
        {
            LPUART_EnableTx(CELLULAR_UART_BASE, false);
        }

        std::shared_ptr<drivers::DriverPLL> pll;
        std::shared_ptr<drivers::DriverGPIO> gpio_1;
        std::shared_ptr<drivers::DriverGPIO> gpio_2;
        std::shared_ptr<drivers::DriverGPIO> gpio_3;
        std::shared_ptr<drivers::DriverDMAMux> dmamux;
        std::shared_ptr<drivers::DriverDMA> dma;
        std::unique_ptr<drivers::DriverDMAHandle> txDMAHandle;
        std::unique_ptr<drivers::DriverDMAHandle> rxDMAHandle;

        static constexpr std::uint32_t baudrate                               = 115200;
        static constexpr std::uint32_t rxMessageBufferLength                  = 1024;
        static constexpr std::uint16_t messageBufferOverheadSize              = 4;
        static constexpr std::uint32_t CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE = 1;
        static constexpr std::uint32_t CELLULAR_BSP_ANTSEL_PIN_A_STATE        = 0;
        static constexpr std::uint32_t CELLULAR_BSP_ANTSEL_PIN_B_STATE        = 1;
    };
} // namespace bsp
