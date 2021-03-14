// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "rt1051_cellular.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "dma_config.h"
#include "fsl_cache.h"
#include <common_data/EventStore.hpp>
#include <ticks.hpp>

#include <algorithm>

#if _RT1051_UART_DEBUG == 1
#define logUARTdebug(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logUARTdebug(...)
#endif

namespace bsp::cellular
{
    void notifyReceivedNew()
    {
        BaseType_t hp = pdFALSE;
        if (bsp::RT1051Cellular::untilReceivedNewHandle != nullptr) {
            vTaskNotifyGiveFromISR(bsp::RT1051Cellular::untilReceivedNewHandle, &hp);
        }
        portEND_SWITCHING_ISR(hp);
    }
}; // namespace bsp::cellular

extern "C"
{
    void LPUART1_IRQHandler(void)
    {
        uint32_t isrReg = LPUART_GetStatusFlags(CELLULAR_UART_BASE);

        if (bsp::RT1051Cellular::uartRxStreamBuffer != NULL) {
            auto RxDmaStatus = LPUART_TransferGetReceiveCountEDMA(
                CELLULAR_UART_BASE,
                &bsp::RT1051Cellular::uartDmaHandle,
                reinterpret_cast<uint32_t *>(&bsp::RT1051Cellular::RXdmaReceivedCount));

            if (isrReg & bsp::RT1051Cellular::startIRQMask) {
                if (RxDmaStatus == kStatus_NoTransferInProgress) {
                    LPUART_DisableInterrupts(CELLULAR_UART_BASE, bsp::RT1051Cellular::startIRQMaskEnable);
                    bsp::RT1051Cellular::RXdmaReceivedCount = -1;
                    if (not bsp::RT1051Cellular::StartReceive(bsp::RT1051Cellular::GetFreeStreamBufferSize())) {
                        bsp::RT1051Cellular::RestartReceivingManually = true;
                        bsp::RT1051Cellular::FinishReceive();
                    }
                    logUARTdebug("[RX] on Incoming data");
                }
            }
            if (isrReg & bsp::RT1051Cellular::finishIRQMask) {
                if (RxDmaStatus != kStatus_NoTransferInProgress) {
                    LPUART_TransferAbortReceiveEDMA(CELLULAR_UART_BASE, &bsp::RT1051Cellular::uartDmaHandle);
                    logUARTdebug("[RX idle] stopped engine");
                }
                logUARTdebug("[RX idle], received %d bytes", bsp::RT1051Cellular::RXdmaReceivedCount);
                // the main exit path on transmission done
                if (bsp::RT1051Cellular::RXdmaReceivedCount >= 0) {
                    if (bsp::RT1051Cellular::RXdmaReceivedCount > 0) {
                        bsp::RT1051Cellular::MoveRxDMAtoStreamBuf(bsp::RT1051Cellular::RXdmaReceivedCount);
                        bsp::RT1051Cellular::RXdmaReceivedCount = -1;
                    }
                    if (not bsp::RT1051Cellular::RestartReceivingManually) {
                        bsp::RT1051Cellular::FinishReceive();
                    }
                }
            }

            LPUART_ClearStatusFlags(CELLULAR_UART_BASE, isrReg);
        }
    }
};

namespace bsp
{
    uint8_t RT1051Cellular::RXdmaBuffer[RXdmaBufferSize] = {0};
    ssize_t RT1051Cellular::RXdmaReceivedCount           = -1;
    size_t RT1051Cellular::RXdmaMaxReceivedCount         = -1;
    bool RT1051Cellular::RestartReceivingManually        = false;

    using namespace drivers;

    lpuart_edma_handle_t RT1051Cellular::uartDmaHandle      = {};
    TaskHandle_t RT1051Cellular::untilReceivedNewHandle     = nullptr;
    StreamBufferHandle_t RT1051Cellular::uartRxStreamBuffer = nullptr;

    RT1051Cellular::RT1051Cellular()
    {
        MSPInit();
        /// to set Store::GSM sim state and to log debug
        Store::GSM::get()->tray =
            GPIO_PinRead(GPIO2, BSP_CELLULAR_SIM_TRAY_INSERTED_PIN) == 0 ? Store::GSM::Tray::IN : Store::GSM::Tray::OUT;
        DMAInit();

        uartRxStreamBuffer = xStreamBufferCreate(rxStreamBufferLength, rxStreamBufferNotifyWatermark);
        if (uartRxStreamBuffer == NULL) {
            LOG_ERROR("Could not create the RX stream buffer!");
            return;
        }

        driverLPUART = drivers::DriverLPUART::Create(
            "cellular", static_cast<drivers::LPUARTInstances>(BoardDefinitions::CELLULAR_LPUART_INSTANCE));

        lpuart_config_t s_cellularConfig;

        LPUART_GetDefaultConfig(&s_cellularConfig);

        s_cellularConfig.baudRate_Bps  = baudrate;
        s_cellularConfig.dataBitsCount = kLPUART_EightDataBits;
        s_cellularConfig.parityMode    = kLPUART_ParityDisabled;
        s_cellularConfig.isMsb         = false;
        s_cellularConfig.rxIdleType    = kLPUART_IdleTypeStartBit;
#if _RT1051_UART_DEBUG
        s_cellularConfig.rxIdleConfig = kLPUART_IdleCharacter4; // Logs take time
#else
        s_cellularConfig.rxIdleConfig = kLPUART_IdleCharacter1;
#endif
        s_cellularConfig.enableTx    = false;
        s_cellularConfig.enableRx    = false;
        s_cellularConfig.enableTxCTS = true;
        s_cellularConfig.txCtsConfig = kLPUART_CtsSampleAtStart; // be nice, allow to stop mid txfer
        s_cellularConfig.enableRxRTS = true;

        if (LPUART_Init(CELLULAR_UART_BASE, &s_cellularConfig, GetPerphSourceClock(PerphClock_LPUART)) !=
            kStatus_Success) {
            LOG_ERROR("Could not initialize the uart!");
            return;
        }

        static_assert(rxStreamBufferLength >= 6, "Minimum buffer size (i.e. sufficient to enable flow control)");
        static_assert(rxStreamBufferLength >= 1024, "To be able to fit entire response");

        // CANNOT disable FIFO, dma *needs* it :o
        // CELLULAR_UART_BASE->FIFO &= ~LPUART_FIFO_RXFE_MASK;

        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        NVIC_ClearPendingIRQ(LPUART1_IRQn);
        NVIC_SetPriority(LPUART1_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(LPUART1_IRQn);

        isInitialized = true;
    }

    void RT1051Cellular::SetSpeed(uint32_t portSpeed)
    {
        LOG_DEBUG("[RT1051] Setting %" PRIu32 " baudrate", portSpeed);
        LPUART_SetBaudRate(CELLULAR_UART_BASE, portSpeed, GetPerphSourceClock(PerphClock_LPUART));
    }

    RT1051Cellular::~RT1051Cellular()
    {

        if (uartRxStreamBuffer) {
            vStreamBufferDelete(uartRxStreamBuffer);
            uartRxStreamBuffer = nullptr;
        }

        DisableRx();
        DisableTx();

        NVIC_DisableIRQ(LPUART1_IRQn);
        LPUART_DisableInterrupts(CELLULAR_UART_BASE,
                                 kLPUART_IdleLineInterruptEnable | kLPUART_RxDataRegFullInterruptEnable);
        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        NVIC_ClearPendingIRQ(LPUART1_IRQn);

        LPUART_Deinit(CELLULAR_UART_BASE);

        MSPDeinit();
        DMADeinit();

        memset(&uartDmaHandle, 0, sizeof uartDmaHandle);
        untilReceivedNewHandle = nullptr;
    }

    void RT1051Cellular::PowerUp()
    {
        const TickType_t POWER_UP_DELAY_MS = 500;

        DisableRx();

        ExitSleep();

        TickType_t tick = xTaskGetTickCount();
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 1);
        vTaskDelayUntil(&tick, POWER_UP_DELAY_MS);
        // BSP_CellularSetPowerState(CellularPowerStateTurningOn);
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 0);

        EnableRx();
    }

    void RT1051Cellular::PowerDown()
    {
        const uint16_t POWER_DOWN_DELAY_MS = 700;

        ExitSleep();

        TickType_t tick = xTaskGetTickCount();
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 1);
        vTaskDelayUntil(&tick, POWER_DOWN_DELAY_MS);
        // BSP_CellularSetPowerState(CellularPowerStateTurningOff);
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN), 0);
        // vTaskDelay(pdMS_TO_TICKS(POWER_DOWN_IN_PROGRESS_DELAY_MS));

        /*        while (s_cellularPowerState != CellularPowerStatePoweredDown)
                {
                    vTaskDelay(pdMS_TO_TICKS(200));
                }*/

        DisableRx();
    }

    void RT1051Cellular::Restart()
    {
        const uint16_t RESET_DELAY_MS = 460;

        ExitSleep();

        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN), 1);
        vTaskDelay(pdMS_TO_TICKS(RESET_DELAY_MS));
        gpio_2->WritePin(static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN), 0);
    }

    ssize_t RT1051Cellular::Write(void *buf, size_t nbytes)
    {
        lpuart_transfer_t sendXfer;
#if _RT1051_UART_DEBUG
        LOG_PRINTF("[TX: %d]", nbytes);
        uint8_t *ptr = (uint8_t *)buf;
        LOG_PRINTF("\n{");
        for (size_t i = 0; i < nbytes; i++)
            LOG_PRINTF("%02X ", (uint8_t)*ptr++);
        LOG_PRINTF("}\n<");
        ptr = (uint8_t *)buf;
        for (size_t i = 0; i < nbytes; i++)
            LOG_PRINTF("%c", (uint8_t)*ptr++);
        LOG_PRINTF(">");
        LOG_PRINTF("\n");
#endif
        sendXfer.data     = static_cast<uint8_t *>(buf);
        sendXfer.dataSize = nbytes;

        ExitSleep();

        uartDmaHandle.userData = xTaskGetCurrentTaskHandle();
        SCB_CleanInvalidateDCache();

        EnableTx();
        if (LPUART_SendEDMA(CELLULAR_UART_BASE, &uartDmaHandle, &sendXfer) != kStatus_Success) {
            LOG_ERROR("Cellular: TX Failed!");
            DisableTx();
            return -1;
        }

        // wait for Tx to finish
        auto ulNotificationValue = ulTaskNotifyTake(pdFALSE, 100);

        if (ulNotificationValue == 0) {
            LOG_ERROR("Cellular Uart error: TX Transmission timeout");
            DisableTx();
            return -1;
        }

        DisableTx();

        return nbytes;
    }

    bool RT1051Cellular::MoveRxDMAtoStreamBuf(size_t nbytes)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        assert(nbytes > 0);

        if (nbytes > GetFreeStreamBufferSize()) {
            LOG_ERROR("Cannot dump DMA buffer. Data is lost (%d>%d)", nbytes, GetFreeStreamBufferSize());
            return false;
        }

#if _RT1051_UART_DEBUG
        auto ret =
#endif
            xStreamBufferSendFromISR(uartRxStreamBuffer, (void *)&RXdmaBuffer, nbytes, &xHigherPriorityTaskWoken);
        logUARTdebug("[RX] moved %d bytes to streambuf", ret);

        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        return true;
    }

    size_t RT1051Cellular::GetFreeStreamBufferSize()
    {
        return xStreamBufferSpacesAvailable(bsp::RT1051Cellular::uartRxStreamBuffer);
    }

    bool RT1051Cellular::StartReceive(size_t nbytes)
    {
        if (!(GetFreeStreamBufferSize() > 0)) {
            logUARTdebug("Not starting RX DMA, stream buffer is full. Be aware");
            return false;
        }

        else {
            // sanitize input
            RXdmaMaxReceivedCount = std::min(nbytes, static_cast<size_t>(RXdmaBufferSize));
            logUARTdebug("Starting DMA RX, max %d bytes", RXdmaMaxReceivedCount);
        }
        assert(RXdmaMaxReceivedCount <= RXdmaBufferSize);

        // start RXfer if there is a byte incoming and no pending RXfer
        lpuart_transfer_t receiveXfer;
        receiveXfer.data     = RXdmaBuffer;
        receiveXfer.dataSize = RXdmaMaxReceivedCount;

        // rx config
        auto status = LPUART_ReceiveEDMA(CELLULAR_UART_BASE, &uartDmaHandle, &receiveXfer);
        switch (status) {
        case kStatus_Success:
            break;
        case kStatus_LPUART_RxBusy:
            LOG_WARN("UART RX DMA already busy");
            return false;
        case kStatus_InvalidArgument:
            LOG_WARN("UART RX DMA invalid argument");
            return false;
        }
        return true;
    }

    void RT1051Cellular::FinishReceive()
    {
        logUARTdebug("[RX] finish");
        bsp::cellular::notifyReceivedNew();
    }

    ssize_t RT1051Cellular::Read(void *buf, size_t nbytes)
    {
        ExitSleep();
        ssize_t ret = xStreamBufferReceive(uartRxStreamBuffer, buf, nbytes, 0);
#if _RT1051_UART_DEBUG
        if (ret > 0) {
            LOG_PRINTF("[RX: %d]", ret);
            uint8_t *ptr = (uint8_t *)buf;
            LOG_PRINTF("\n{");
            for (ssize_t i = 0; i < ret; i++)
                LOG_PRINTF("%02X ", (uint8_t)*ptr++);
            LOG_PRINTF("}\n<");
            ptr = (uint8_t *)buf;
            for (ssize_t i = 0; i < ret; i++)
                LOG_PRINTF("%c", (uint8_t)*ptr++);
            LOG_PRINTF(">");
        }
        else {
            LOG_PRINTF("[RX] GOT NOTHING (requested %d)", nbytes);
        }
        LOG_PRINTF("\n");
#endif
        if (RestartReceivingManually) {
            logUARTdebug("[RX] resume on Paused");
            // need to start manually, as RegBuf might be already full, therefore no Active Edge Interrupt
            if (StartReceive(GetFreeStreamBufferSize())) {
                RestartReceivingManually = false;
            }
            else {
                // do not lose information as whether manual start needs to be initiated and schedule next poke on Read
                LOG_WARN("Modem is waiting to send data. Stream buffer likely too small");
                FinishReceive();
            }
        }
        return ret;
    }

    uint32_t RT1051Cellular::Wait(uint32_t timeout)
    {
        logUARTdebug("[WAIT]");
        if (untilReceivedNewHandle != nullptr) {
            LOG_FATAL("Wait called simultaneously from more than one thread!");
            return 0;
        }

        // no need to wait: buffer already contains something
        if (xStreamBufferBytesAvailable(uartRxStreamBuffer)) {
            return 1;
        }

        EnableRx();

        // …start waiting for [timeout] until a new xfer from modem is received
        untilReceivedNewHandle = xTaskGetCurrentTaskHandle();
        auto ret               = ulTaskNotifyTake(pdTRUE, timeout);
        untilReceivedNewHandle = nullptr;
        return ret;
    }

    void RT1051Cellular::InformModemHostAsleep()
    {
        gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN),
                         !CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE);
    }

    void RT1051Cellular::InformModemHostWakeup()
    {
        gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN),
                         CELLULAR_BSP_AP_READY_PIN_ACTIVE_STATE);
    }

    void RT1051Cellular::EnterSleep()
    {
        if (!isInSleepMode) {
            isInSleepMode = true;

            gpio_3->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN), 1);
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN), 1);

            // Host sleep information must be before UART disable
            InformModemHostAsleep();

            if (driverLPUART) {
                driverLPUART->Disable();
            }
        }
    }

    void RT1051Cellular::ExitSleep()
    {
        // reset sleep timer countdown
        lastCommunicationTimestamp = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());

        if (isInSleepMode) {
            isInSleepMode = false;

            if (driverLPUART) {
                driverLPUART->Enable();
            }

            gpio_3->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN), 0);
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN), 0);
            vTaskDelay(pdMS_TO_TICKS(15));

            // Host wake up information must be after UART enable
            InformModemHostWakeup();
        }
    }

    void RT1051Cellular::MSPInit()
    {

        gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_1), DriverGPIOParams{});
        gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_2), DriverGPIOParams{});
        gpio_3 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_3), DriverGPIOParams{});

        gpio_1->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN) |
                                    1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));

        gpio_2->ClearPortInterrupts(
            1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN) |
            1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN) |
            1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN));

        gpio_1->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN) |
                                 1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));

        gpio_2->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN) |
                                 1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN) |
                                 1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN));

        // INPUTS
        gpio_1->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                            .defLogic = 1,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                .defLogic = 1,
                                .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN)});

        // OUTPUTS
        gpio_1->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::USB_FUNCTION_MUX_SELECT)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_APRDY_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_POWER_PIN)});

        gpio_2->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RESET_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIMSEL_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIMCARD_PRESENCE_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_WAKEUP_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN)});

        gpio_2->ConfPin(
            DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                .defLogic = 0,
                                .pin      = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_USB_BOOT_PIN)});

        gpio_3->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                            .defLogic = 0,
                                            .pin = static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_3_DTR_PIN)});
        ;
        // ENABLE INTERRUPTS

        gpio_1->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));
        gpio_2->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN) |
                                1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN));
    }

    void RT1051Cellular::MSPDeinit()
    {
        gpio_2->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_SIM_TRAY_INSERTED_PIN));
        gpio_2->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_2_RI_PIN));
        gpio_1->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_CTS_PIN));
        gpio_1->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN));
    }

    void RT1051Cellular::DMAInit()
    {

        // TODO:M.P add PLL support
        // pll = DriverInterface<DriverPLL>::Create(static_cast<PLLInstances >(BoardDefinitions
        // ::AUDIO_PLL),DriverPLLParams{});
        dmamux =
            DriverDMAMux::Create(static_cast<DMAMuxInstances>(BoardDefinitions::CELLULAR_DMAMUX), DriverDMAMuxParams{});
        dma = DriverDMA::Create(static_cast<DMAInstances>(BoardDefinitions::CELLULAR_DMA), DriverDMAParams{});

        txDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL));
        rxDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::CELLULAR_RX_DMA_CHANNEL));

        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL), kDmaRequestMuxLPUART1Tx);
        dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_RX_DMA_CHANNEL), kDmaRequestMuxLPUART1Rx);

        LPUART_TransferCreateHandleEDMA(CELLULAR_UART_BASE,
                                        &uartDmaHandle,
                                        uartDMACallback,
                                        NULL,
                                        reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle()),
                                        reinterpret_cast<edma_handle_t *>(rxDMAHandle->GetHandle()));
    }

    void RT1051Cellular::DMADeinit()
    {
        dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_TX_DMA_CHANNEL));
        dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::CELLULAR_RX_DMA_CHANNEL));
    }

    void RT1051Cellular::uartDMACallback(LPUART_Type *base,
                                         lpuart_edma_handle_t *handle,
                                         status_t status,
                                         void *userData)
    {
        BaseType_t higherPriorTaskWoken = pdFALSE;

        switch (status) {
        case kStatus_LPUART_TxIdle: {
            logUARTdebug("[TX done]");
            // task handle to be released in userData
            vTaskNotifyGiveFromISR((TaskHandle_t)userData, &higherPriorTaskWoken);

            portEND_SWITCHING_ISR(higherPriorTaskWoken);

            break;
        }
        case kStatus_LPUART_RxIdle:
            logUARTdebug("[RX] Chunk done. Flow control must hold the gate from now on");
            if (MoveRxDMAtoStreamBuf(RXdmaMaxReceivedCount) and StartReceive(GetFreeStreamBufferSize())) {
                // usual mode: append a chunk and wait for line idle to finish receive
            }
            else {
                // the auxiliary exit path on stream buffer full
                RestartReceivingManually = true;
                FinishReceive();
            }
            RXdmaReceivedCount = -1;
            break;
        }
    }

    void RT1051Cellular::SetSendingAllowed(bool state)
    {
        pv_SendingAllowed = state;
    }
    bool RT1051Cellular::GetSendingAllowed()
    {
        return pv_SendingAllowed;
    }

    void RT1051Cellular::SelectAntenna(bsp::cellular::antenna antenna)
    {
        if (antenna == bsp::cellular::antenna::lowBand) {
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN),
                             CELLULAR_BSP_ANTSEL_PIN_A_STATE);
            LOG_INFO("Selecting Antenna A");
        }
        else if (antenna == bsp::cellular::antenna::highBand) {
            gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN),
                             CELLULAR_BSP_ANTSEL_PIN_B_STATE);
            LOG_INFO("Selecting Antenna B");
        }
    }

    bsp::cellular::antenna RT1051Cellular::GetAntenna()
    {
        // make sure ANTSEL pin has Software Input On Field set
        bool whichAntenna = gpio_2->ReadPin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_ANTSEL_PIN));
        return (whichAntenna == CELLULAR_BSP_ANTSEL_PIN_A_STATE ? bsp::cellular::antenna::lowBand
                                                                : bsp::cellular::antenna::highBand);
    }

    namespace cellular
    {
        static xQueueHandle qhandle = nullptr;

        auto init(QueueHandle_t qHandle) -> int
        {
            qhandle = qHandle;
            return 0;
        }

        namespace USB
        {
            BootPinState getBootPin()
            {
                auto gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_2),
                                                 DriverGPIOParams{});
                bool state  = gpio_2->ReadPin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_USB_BOOT_PIN));

                return (state == true ? BootPinState::FIRMWARE_UPGRADE : BootPinState::NORMAL_BOOT);
            }
            PassthroughState getPassthrough()
            {
                auto gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_1),
                                                 DriverGPIOParams{});
                bool state  = gpio_1->ReadPin(magic_enum::enum_integer(BoardDefinitions::USB_FUNCTION_MUX_SELECT));

                return (state == true ? PassthroughState::ENABLED : PassthroughState::DISABLED);
            }

            // modem needs a reboot to enter DFU (Firmware Upgrade) mode
            void setBootPin(BootPinState bootPin)
            {
                auto gpio_2 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_2),
                                                 DriverGPIOParams{});
                gpio_2->WritePin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_2_USB_BOOT_PIN),
                                 bootPin == BootPinState::FIRMWARE_UPGRADE ? 1 : 0);
            }

            void setPassthrough(PassthroughState pass)
            {
                auto gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_1),
                                                 DriverGPIOParams{});
                gpio_1->WritePin(magic_enum::enum_integer(BoardDefinitions::USB_FUNCTION_MUX_SELECT),
                                 pass == PassthroughState::ENABLED ? 1 : 0);
            }
        } // namespace USB

        namespace status
        {
            bsp::cellular::status::value getStatus()
            {
                auto gpio_1 = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::CELLULAR_GPIO_1),
                                                 DriverGPIOParams{});
                auto state =
                    gpio_1->ReadPin(magic_enum::enum_integer(BoardDefinitions::CELLULAR_GPIO_1_STATUS_PIN)) == 0
                        ? value::ACTIVE
                        : value::INACTIVE;

                return state;
            }
            BaseType_t statusIRQhandler()
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                if (qhandle != NULL) {
                    uint8_t val = static_cast<uint8_t>(IRQsource::statusPin);
                    xQueueSendFromISR(qhandle, &val, &xHigherPriorityTaskWoken);
                }
                return xHigherPriorityTaskWoken;
            }

        } // namespace status

        namespace sim
        {

            auto trayIRQ_handler() -> BaseType_t
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;

                if (qhandle) {
                    uint8_t val = static_cast<uint8_t>(IRQsource::trayPin);
                    xQueueSendFromISR(qhandle, &val, &xHigherPriorityTaskWoken);
                }
                return xHigherPriorityTaskWoken;
            }

            auto getTray() -> Store::GSM::Tray
            {
                auto state = GPIO_PinRead(GPIO2, BSP_CELLULAR_SIM_TRAY_INSERTED_PIN) == 0 ? Store::GSM::Tray::IN
                                                                                          : Store::GSM::Tray::OUT;
                return state;
            }

            void hotswap_trigger()
            {
                GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIM_CARD_PRESENCE_PIN, 1);
                vTaskDelay(100); // sleep for 100 ms...
                GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIM_CARD_PRESENCE_PIN, 0);
            }

            void sim_sel()
            {
                if (Store::GSM::get()->selected == Store::GSM::SIM::SIM2) {
                    GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIMSEL_PIN, 1);
                }
                else {
                    GPIO_PinWrite(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT, BSP_CELLULAR_SIMSEL_PIN, 0);
                }
            }
        } // namespace sim

        namespace ringIndicator
        {
            auto riIRQ_handler() -> BaseType_t
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                if (qhandle != NULL) {
                    uint8_t val = static_cast<uint8_t>(IRQsource::ringIndicatorPin);
                    xQueueSendFromISR(qhandle, &val, &xHigherPriorityTaskWoken);
                }
                return xHigherPriorityTaskWoken;
            }
        } // namespace ringIndicator

    } // namespace cellular
} // namespace bsp
